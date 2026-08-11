"""The generate step -- step 04 of the Class 7 loop, "write the code".

RAW ORCHESTRATION, which Class 7 mandates for this assignment: one direct call to the
Anthropic Messages API, a prompt this file assembles by hand, and a parser this file
owns. There is no framework between the agent and the model, so every decision about
what context the model got is visible in one place.

RECORD / REPLAY. Every live call is written to `replay/<key>.json` -- the full request
and the full response. `--offline` reads those files instead of calling the API, so a
grader with no API key runs the same agent over the same prompts and gets the same
bytes. The replay store is committed for exactly that reason. It is a RECORDING, not a
fixture: nothing in it was hand-written, and `--offline` is not a second code path
that could drift from the live one -- both go through `Generator.generate`, which
differs only in where the response comes from.

WHAT THE MODEL IS AND IS NOT TRUSTED WITH. It writes source. It does not decide what
to write, where it goes, or whether it is accepted: the candidate came from the
scorer, the paths come from the candidate, and a human reads every file before it
lands (see architect.py's `--apply` gate).
"""

from __future__ import annotations

import json
import os
import re
from dataclasses import dataclass, field
from pathlib import Path

# Opus rather than the crew's Sonnet default, and the reason is the target rather than
# a preference for the bigger model: this generates UE C++ against §4.7 Stub 8, a
# contract with 27 contractually-typed fields and an invariant that fails a projection
# for being right by the wrong derivation. One-shot correctness is worth more here than
# the token cost. Override with --model or STRATOCRACY_ARCHITECT_MODEL.
DEFAULT_MODEL = "claude-opus-5"

# 16000 was not enough and the failure was silent in a way worth recording: the
# scoreboard step returned `output_tokens = 16000` and ZERO text blocks, because the
# whole budget went to reasoning and the answer never started. The parser then reported
# "no FILE block", which was true and useless -- it described the symptom and not the
# cause. Hence both the larger budget and the stop-reason capture below.
MAX_TOKENS = 32000


@dataclass
class GeneratedFile:
    path: str          # repo-relative, forward slashes
    body: str


@dataclass
class Generation:
    key: str
    files: list[GeneratedFile] = field(default_factory=list)
    raw: str = ""
    model: str = ""
    source: str = ""   # "live" or "replay"


# ---------------------------------------------------------------------------
# Key material
# ---------------------------------------------------------------------------

def load_api_key(env_files: list[Path]) -> str | None:
    """ANTHROPIC_API_KEY from the environment, else from a .env this project already
    keeps one in. Parsed by hand rather than with python-dotenv so the agent has no
    dependency beyond the SDK itself."""
    if os.environ.get("ANTHROPIC_API_KEY"):
        return os.environ["ANTHROPIC_API_KEY"]
    for path in env_files:
        if not path.is_file():
            continue
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            line = line.strip()
            if line.startswith("ANTHROPIC_API_KEY"):
                _, _, value = line.partition("=")
                value = value.strip().strip('"').strip("'")
                if value:
                    return value
    return None


# ---------------------------------------------------------------------------
# Response parsing
# ---------------------------------------------------------------------------

_FILE_BLOCK = re.compile(
    r"^=== FILE: (?P<path>[^\n=]+?) ===\n(?P<body>.*?)^=== END FILE ===",
    re.S | re.M,
)


def parse_files(text: str) -> list[GeneratedFile]:
    """Pulls the delimited file blocks out of a response.

    The delimiter is a bare marker rather than a fenced code block because these files
    contain fenced blocks' own characters and, more to the point, C++ that includes
    backticks in comments. A parser that split on ``` would truncate a file at the
    first comment that quoted one.
    """
    out: list[GeneratedFile] = []
    for m in _FILE_BLOCK.finditer(text):
        path = m.group("path").strip().replace("\\", "/")
        body = m.group("body")
        # A model that wraps the body in a fence anyway is accommodated rather than
        # rejected -- the fence is stripped, the content is what was asked for.
        fence = re.match(r"\s*```[a-zA-Z+]*\n(.*?)```\s*$", body, re.S)
        if fence:
            body = fence.group(1)
        if not body.endswith("\n"):
            body += "\n"
        out.append(GeneratedFile(path=path, body=body))
    return out


# ---------------------------------------------------------------------------
# The generator
# ---------------------------------------------------------------------------

class Generator:
    def __init__(
        self,
        replay_dir: Path,
        offline: bool,
        model: str = DEFAULT_MODEL,
        api_key: str | None = None,
    ) -> None:
        self.replay_dir = replay_dir
        self.replay_dir.mkdir(parents=True, exist_ok=True)
        self.offline = offline
        self.model = model
        self.api_key = api_key
        self._client = None

    def _client_lazy(self):
        if self._client is None:
            import anthropic  # imported here so --offline needs no SDK at all
            self._client = anthropic.Anthropic(api_key=self.api_key)
        return self._client

    def generate(self, key: str, system: str, user: str, blackboard) -> Generation:
        """One generation. `key` names the replay slot and must be stable across runs
        for the same step, or offline replay cannot find it."""
        blackboard.record_prompt(key, system, user)
        slot = self.replay_dir / f"{key}.json"

        if self.offline:
            if not slot.is_file():
                raise FileNotFoundError(
                    f"--offline needs a recorded call at {slot}, and there is none. "
                    f"Run live once to record it."
                )
            payload = json.loads(slot.read_text(encoding="utf-8"))
            raw = payload["response_text"]
            blackboard.bullet(
                f"REPLAYED `{key}` from `{slot.name}` "
                f"(recorded {payload.get('recorded_at', 'unknown')}, "
                f"model {payload.get('model', 'unknown')})"
            )
            source, model = "replay", payload.get("model", "unknown")
        else:
            blackboard.bullet(f"calling {self.model} live for `{key}` (streaming) ...")
            # STREAMED, and not by preference: the SDK refuses a non-streaming request
            # whose `max_tokens` could take it past ten minutes, which MAX_TOKENS now
            # can. `stream()` also means a long generation reports progress instead of
            # looking hung for several minutes.
            with self._client_lazy().messages.stream(
                model=self.model,
                max_tokens=MAX_TOKENS,
                system=system,
                messages=[{"role": "user", "content": user}],
            ) as stream:
                for _ in stream.text_stream:
                    pass
                response = stream.get_final_message()
            raw = "".join(
                block.text for block in response.content
                if getattr(block, "type", None) == "text"
            )
            block_types = [getattr(b, "type", None) for b in response.content]
            stop_reason = getattr(response, "stop_reason", None)

            # Recorded BEFORE parsing, so a response the parser chokes on is still on
            # disk to be looked at rather than lost with the exception. `stop_reason`
            # and the block types are recorded because their absence is what made the
            # empty-response failure above hard to read.
            slot.write_text(json.dumps({
                "key": key,
                "model": self.model,
                "recorded_at": _utc_stamp(),
                "stop_reason": stop_reason,
                "block_types": block_types,
                "system": system,
                "user": user,
                "response_text": raw,
                "usage": {
                    "input_tokens": getattr(response.usage, "input_tokens", None),
                    "output_tokens": getattr(response.usage, "output_tokens", None),
                },
            }, indent=2), encoding="utf-8")
            blackboard.bullet(
                f"recorded live call -> `replay/{key}.json` "
                f"({getattr(response.usage, 'output_tokens', '?')} output tokens, "
                f"stop_reason `{stop_reason}`, blocks {block_types})"
            )
            if stop_reason == "max_tokens":
                blackboard.bullet(
                    f"WARNING: the model hit the {MAX_TOKENS}-token output cap. Any "
                    f"file it was mid-way through is incomplete; raise MAX_TOKENS or "
                    f"split this step rather than accepting a truncated file."
                )
            if not raw.strip():
                blackboard.bullet(
                    "WARNING: the response carried no text block at all "
                    f"(blocks: {block_types}). Nothing can be parsed from it."
                )
            source, model = "live", self.model

        files = parse_files(raw)
        gen = Generation(key=key, files=files, raw=raw, model=model, source=source)
        for f in files:
            blackboard.record_generated(f.path, f.body)
        if not files:
            blackboard.bullet(
                "WARNING: the response contained no `=== FILE: ... ===` block. "
                "Nothing will be proposed for this step."
            )
        return gen


def _utc_stamp() -> str:
    from datetime import datetime, timezone
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
