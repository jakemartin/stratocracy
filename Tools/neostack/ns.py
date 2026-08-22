"""Drive a live Unreal editor's own MCP endpoint over HTTP, bypassing the NeoStack proxy.

AGENT-AUTHORED coordinator instrument. Deliberately NOT under `Tools/editor/`, whose
`make_scoreboard_wbp.py` header reserves that directory for hand-written tooling and says
keeping the two apart is the point; and deliberately not under `Tools/architect/`, which is
`strat-data-steward`'s lane and holds gates and evidence rather than instruments.

WHY THIS EXISTS. The `NeoStack_Connect` MCP proxy decides at Claude Code session start whether
an editor exists and latches that decision. When it latches, the session's tool surface carries
only `unreal_status` + `list_unreal_projects` and `execute_script` is absent, so
`strat-editor-builder` -- which has no Bash -- cannot work at all. The editor itself is fine:
its OWN MCP server keeps serving `execute_script` the whole time. This talks to that directly.
Measured 2026-08-21 and again 2026-08-22. Restarting Claude Code is NOT a reliable fix: a
session started with a verified-live editor already up still received the two-tool surface.

LIVENESS IS DECIDED BY THE PORT, NEVER BY `mcpRunning`. `runtimes.json` is written at editor
start and its `mcpRunning: true` is not cleared when the editor dies -- measured with a control
(`netstat` saw 33 listeners, none of them the advertised port, while the flag read true). A
reader that filters on the flag POSTs into a closed port and then debugs its own HTTP code. This
filters by an actual TCP connect, and `candidates()` is exposed so a caller can see what was
rejected. `runtimes.json` also accumulates stale entries from other checkouts advertising the
SAME port, so confirm the tree with a probe -- `lua("print(project_dir())")` -- before labelling
any evidence with a project path.

TWO HAZARDS THAT COST A SESSION EACH, both worth reading before driving PIE:

  1. `open_asset` ON A WIDGET BLUEPRINT RECOMPILES IT. During PIE that nulls the live widget's
     `WidgetTree` and produces readings that look like real findings (a live widget with zero
     children, past a control that answers in the same command). It also crashes the editor on
     PIE teardown at `PlayLevel.cpp:553`, the leak chain naming the editor's undo `TransBuffer`.
     Order every session: runtime reads first, then `playtest_stop()`, then asset work. Nothing
     measured after the first `open_asset` in a PIE session is evidence about the game.

  2. `GetAll <Class> <Property>` on a UMG-BOUND property reports the DESIGN value, because a
     binding drives the Slate attribute without writing the UPROPERTY back. Use it for unbound
     state; use `playtest_console("Shot showui")` for painted state -- the only capture route
     that composites the UMG layer.

USE IT:

    import sys; sys.path.insert(0, r"E:/MultiAgent/Stratocracy/Tools/neostack")
    import ns
    ns.connect()                       # raises if no advertised port accepts a connection
    print(ns.lua('print(project_dir())'))

    python ns.py                       # print connection info as JSON
    python ns.py somescript.lua        # run a Lua file through the editor

Console output is cp1252 on this machine and the tool's `->` arrows raise UnicodeEncodeError,
so stdout is reconfigured to utf-8 below rather than at every call site.
"""

import json
import os
import socket
import sys
import urllib.error
import urllib.request
from urllib.parse import urlparse

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")

RUNTIMES = os.path.expandvars(r"%LOCALAPPDATA%\NeoStackAI\runtimes.json")
CONNECT_TIMEOUT_S = 1.5
CALL_TIMEOUT_S = 180

_SESSION = {"url": None, "sid": None}


def _port_live(url):
    """A real TCP connect to the url's host:port. The only liveness test this module trusts."""
    u = urlparse(url)
    host = u.hostname or "127.0.0.1"
    port = u.port or (443 if u.scheme == "https" else 80)
    s = socket.socket()
    s.settimeout(CONNECT_TIMEOUT_S)
    try:
        s.connect((host, port))
        return True
    except OSError:
        return False
    finally:
        s.close()


def candidates(project_filter=None):
    """Every advertised MCP url in runtimes.json, newest heartbeat first.

    Returns tuples of (lastHeartbeatAt, editorPid, projectPath, url). Read this when connect()
    fails: it shows what was advertised, which is how you tell "no editor" from "wrong tree".
    The file is written with no BOM by the plugin; utf-8-sig tolerates either.
    """
    with open(RUNTIMES, "r", encoding="utf-8-sig") as f:
        data = json.load(f)
    out = []
    for r in data.get("runtimes", []):
        if project_filter and project_filter.lower() not in (r.get("projectPath") or "").lower():
            continue
        for srv in r.get("mcpServers", []):
            if srv.get("url"):
                out.append((r.get("lastHeartbeatAt", ""), r.get("editorPid"),
                            r.get("projectPath"), srv["url"]))
    out.sort(reverse=True)
    return out


def _post(url, body, sid=None):
    headers = {
        "Content-Type": "application/json",
        # Streamable-HTTP MCP rejects the request without BOTH of these.
        "Accept": "application/json, text/event-stream",
    }
    if sid:
        headers["MCP-Session-Id"] = sid
    req = urllib.request.Request(url, data=json.dumps(body).encode("utf-8"),
                                 headers=headers, method="POST")
    try:
        with urllib.request.urlopen(req, timeout=CALL_TIMEOUT_S) as resp:
            return resp.headers.get("MCP-Session-Id"), resp.read().decode("utf-8", "replace")
    except urllib.error.HTTPError as e:
        return None, "HTTP %s: %s" % (e.code, e.read().decode("utf-8", "replace"))


def _unwrap(raw):
    """The endpoint answers plain JSON or an SSE frame; take the last `data:` line."""
    raw = (raw or "").strip()
    if raw.startswith("event:") or raw.startswith("data:"):
        payloads = [ln[5:].strip() for ln in raw.splitlines() if ln.startswith("data:")]
        raw = payloads[-1] if payloads else raw
    try:
        return json.loads(raw)
    except ValueError:
        return {"_raw": raw}


def connect(project_filter="Stratocracy"):
    """Pick a runtime whose PORT accepts a connection, then MCP-initialize against it.

    Raises RuntimeError naming every rejected url rather than returning a falsy value, so a dead
    editor cannot be mistaken for a working connection that answered oddly.
    """
    cands = candidates(project_filter)
    live = [c for c in cands if _port_live(c[3])]
    if not live:
        raise RuntimeError("no live MCP port among %d advertised url(s): %s"
                           % (len(cands), [c[3] for c in cands]))
    heartbeat, pid, path, url = live[0]
    sid, raw = _post(url, {"jsonrpc": "2.0", "id": 1, "method": "initialize",
                           "params": {"protocolVersion": "2025-06-18",
                                      "capabilities": {},
                                      "clientInfo": {"name": "ns.py", "version": "1"}}})
    info = _unwrap(raw)
    _post(url, {"jsonrpc": "2.0", "method": "notifications/initialized"}, sid)
    _SESSION["url"], _SESSION["sid"] = url, sid
    server = info.get("result", {}).get("serverInfo", {}) if isinstance(info, dict) else {}
    return {"url": url, "editorPid": pid, "projectPath": path, "heartbeat": heartbeat,
            "session": sid, "serverInfo": server,
            "advertised": len(cands), "live": len(live)}


def call(tool, **args):
    """Call an editor MCP tool by name. Connects on first use."""
    if not _SESSION["url"]:
        connect()
    _, raw = _post(_SESSION["url"],
                   {"jsonrpc": "2.0", "id": 2, "method": "tools/call",
                    "params": {"name": tool, "arguments": args}},
                   _SESSION["sid"])
    return _unwrap(raw)


def tools():
    """The tool list the EDITOR advertises -- not the proxy's. Expect `execute_script` here."""
    if not _SESSION["url"]:
        connect()
    _, raw = _post(_SESSION["url"], {"jsonrpc": "2.0", "id": 3, "method": "tools/list"},
                   _SESSION["sid"])
    return _unwrap(raw)


def text(res):
    """Flatten an MCP tool result to its text content.

    Non-text content (images from `screenshot({mode="asset"})`) is reduced to a `<image>` marker
    rather than dropped silently -- a runner that reads only text content loses captures without
    saying so.
    """
    if not isinstance(res, dict):
        return str(res)
    if "error" in res:
        return "ERROR: " + json.dumps(res["error"])
    out = []
    for c in res.get("result", {}).get("content", []) or []:
        out.append(c.get("text", "") if c.get("type") == "text" else "<%s>" % c.get("type"))
    return "\n".join(out) if out else json.dumps(res)[:4000]


def lua(script):
    """Run Lua in the editor and return its text output."""
    return text(call("execute_script", script=script))


if __name__ == "__main__":
    if len(sys.argv) > 1:
        with open(sys.argv[1], "r", encoding="utf-8") as fh:
            body = fh.read()
        print(json.dumps(connect(), indent=2))
        print(lua(body))
    else:
        print(json.dumps(connect(), indent=2))
