"""Codebase perception -- step 02 of the Class 7 loop, "scan the codebase".

Reads the UE project the way a new developer would: what modules exist and what they
depend on, what source files are there, which acceptance IDs are actually ASSERTED by
a test in the tree, and whether specific capabilities the GDD's open IDs need are
reachable from the code that would have to call them.

The last one is the part that matters and the part a file listing cannot give. An
acceptance ID does not close because a file exists; it closes because something can
CALL something. `Ui.h::buildUiSnapshot` takes a `UiWorld`, and the only function in
the tree that builds one takes the DRIVER's `Session` -- a type the engine-side bridge
does not have. Every file involved exists, compiles, and is vendored, and the widget
layer is still unreachable. So capability probes here ask about call paths, not about
paths on disk.

Nothing in this module writes. It is allowed to be wrong about the tree only by being
out of date, never by having changed it.
"""

from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path

SOURCE_SUFFIXES = {".h", ".cpp", ".cs"}


def strip_comments(text: str) -> str:
    """Blanks `//` and `/* */` comments, preserving line structure.

    Line count is preserved rather than the comments being deleted, so a line-anchored
    probe pattern still describes the same line of the real file.
    """
    def blank(m: re.Match[str]) -> str:
        return "".join("\n" if ch == "\n" else " " for ch in m.group(0))

    # String literals are matched first and passed through untouched, so a `//` inside
    # a path literal does not blank the rest of the line.
    return re.sub(
        r'"(?:\\.|[^"\\])*"|/\*.*?\*/|//[^\n]*',
        lambda m: m.group(0) if m.group(0).startswith('"') else blank(m),
        text,
        flags=re.S,
    )


@dataclass
class Module:
    name: str
    build_cs: Path
    public_deps: list[str]
    private_deps: list[str]

    @property
    def deps(self) -> list[str]:
        return sorted(set(self.public_deps) | set(self.private_deps))


@dataclass
class Probe:
    """One capability question, and the evidence that answered it."""
    name: str
    question: str
    present: bool
    evidence: str


@dataclass
class CodebaseFacts:
    root: Path
    modules: dict[str, Module] = field(default_factory=dict)
    files: list[Path] = field(default_factory=list)
    asserted_ids: dict[str, list[str]] = field(default_factory=dict)
    probes: dict[str, Probe] = field(default_factory=dict)
    _text: dict[Path, str] = field(default_factory=dict, repr=False)
    # The same files with comments removed. Capability probes run against THIS,
    # because a probe asking "does this operation exist?" must not be satisfied by a
    # header comment that merely discusses it -- and the headers in this project
    # discuss their own absent operations at length. Measured: `StratBridge.h`'s
    # comment block names `strat::UiWorld` four times while declaring nothing.
    _code: dict[Path, str] = field(default_factory=dict, repr=False)

    # -- lookups -----------------------------------------------------------

    def text_of(self, path: Path) -> str:
        return self._text.get(path, "")

    def grep(self, pattern: str, suffixes: set[str] | None = None,
             within: str | None = None, code_only: bool = False) -> list[tuple[Path, str]]:
        rx = re.compile(pattern)
        hits: list[tuple[Path, str]] = []
        for path in self.files:
            if suffixes and path.suffix not in suffixes:
                continue
            if within and not re.search(within, self.rel(path)):
                continue
            body = (self._code if code_only else self._text).get(path, "")
            m = rx.search(body)
            if m:
                hits.append((path, m.group(0)))
        return hits

    def has(self, probe_name: str) -> bool:
        p = self.probes.get(probe_name)
        return bool(p and p.present)

    def rel(self, path: Path) -> str:
        try:
            return str(path.relative_to(self.root)).replace("\\", "/")
        except ValueError:
            return str(path)


# ---------------------------------------------------------------------------
# Build.cs
# ---------------------------------------------------------------------------

_DEP_BLOCK = re.compile(
    r"(Public|Private)DependencyModuleNames\.AddRange\s*\(\s*new\s+string\[\]\s*\{(.*?)\}",
    re.S,
)


def _parse_build_cs(path: Path, text: str) -> Module:
    public: list[str] = []
    private: list[str] = []
    for kind, body in _DEP_BLOCK.findall(text):
        # Strip // comments before harvesting names: several Build.cs files in this
        # project carry long comment blocks INSIDE the dependency list explaining why
        # a given module is or is not there, and a name mentioned in prose is not a
        # dependency.
        body = re.sub(r"//[^\n]*", "", body)
        names = re.findall(r'"([^"]+)"', body)
        (public if kind == "Public" else private).extend(names)
    return Module(
        # `StratBridge.Build.cs` -> `StratBridge`. `Path.stem` alone leaves the
        # `.Build`, which then fails to match the module names used in every
        # dependency list and makes the module graph look edgeless.
        name=path.name[: -len(".Build.cs")],
        build_cs=path,
        public_deps=public,
        private_deps=private,
    )


# ---------------------------------------------------------------------------
# Capability probes
# ---------------------------------------------------------------------------
#
# Each probe is a question with a regex that answers it. They are declared here rather
# than inline so the blackboard can print the QUESTION beside the verdict -- a probe
# that silently returns False teaches nothing, and the whole value of this pass is that
# a reader can check whether the agent asked the right thing.
#
# `within` SCOPES A PROBE TO THE SIDE OF THE TREE THE QUESTION IS ABOUT, and it is
# load-bearing rather than an optimisation. `Source/StratRules/` is the VENDORED
# headless module: the rules side already declares `buildUiSnapshot`, so an unscoped
# probe asking "does the engine-side bridge expose a snapshot?" matches the vendored
# declaration and answers YES about a module that has nothing to do with the question.
# Measured on the first run of this file, which is why the field exists.

@dataclass(frozen=True)
class ProbeSpec:
    name: str
    question: str
    pattern: str
    suffixes: set[str]
    within: str | None = None
    code_only: bool = True


# A C++ DECLARATION LINE mentioning `token` -- parentheses and a semicolon on the same
# line, comments already stripped. Built as a helper because the first version of these
# probes assumed a shape (a free function returning the type) and therefore failed to
# recognise the out-parameter form this project actually uses -- `FStratResult
# BuildUiSnapshot(strat::UiSnapshot&) const;`. The agent then could not tell that it had
# already succeeded and regenerated the same file on all three iterations. A probe must
# ask whether an OPERATION EXISTS, not whether it was written the way the author of the
# probe imagined.
def _declares(token: str) -> str:
    return rf"(?m)^(?=[^\n]*\()(?=[^\n]*;)[^\n]*\b{token}\b[^\n]*$"


_PROBES: list[ProbeSpec] = [
    ProbeSpec(
        "gamestate_to_uiworld",
        "Can an engine-side caller obtain a Ui.h UiWorld from the bridge -- i.e. from "
        "the authoritative strat::GameState that FStratBridge owns?",
        _declares("strat::UiWorld"),
        {".h"},
        # Scoped to the bridge on purpose: `Ui.h` itself declares
        # `buildUiSnapshot(const UiWorld&)`, so an unscoped probe would answer YES
        # about the vendored contract and never about the engine-side gap.
        within=r"^Source/StratBridge/",
    ),
    ProbeSpec(
        "bridge_exposes_snapshot",
        "Does FStratBridge itself expose a UiSnapshot / view-model query to engine "
        "callers?",
        _declares("strat::UiSnapshot"),
        {".h"},
        within=r"^Source/StratBridge/",
    ),
    ProbeSpec(
        "scenario_retained",
        "Does FStratBridge retain the parsed strat::Scenario after seeding "
        "(UiWorld::guided has no other source)?",
        r"strat::Scenario\s+\w+\s*;",
        {".h"},
        within=r"^Source/StratBridge/",
    ),
    ProbeSpec(
        "scoreboard_widget",
        "Does a scoreboard UUserWidget exist (T-UI-03's subject)?",
        r"class\s+\w*_?API\s+U\w*Scoreboard\w*\s*:\s*public\s+UUserWidget",
        {".h"},
    ),
    ProbeSpec(
        "production_widget",
        "Does a production/build-menu UUserWidget exist (T-UI-04's subject)?",
        r"class\s+\w*_?API\s+U\w*(Production|BuildMenu)\w*\s*:\s*public\s+UUserWidget",
        {".h"},
    ),
    ProbeSpec(
        "buildlist_query",
        "Does a buildlist query exist on the Ui.h contract (T-UI-04's data source)?",
        r"\buiBuildlist\b|\bUiBuildEntry\b",
        {".h", ".cpp"},
    ),
    ProbeSpec(
        "bridge_event_list",
        "Does the bridge expose the §4.9 ordered event list ('events out')?",
        r"\bFStratEvent\b",
        {".h"},
    ),
    ProbeSpec(
        "ui_module_exists",
        "Is there a module that may host UMG widgets without putting a UCLASS beside "
        "the vendored strat headers?",
        r"class\s+StratUI\s*:\s*ModuleRules",
        {".cs"},
    ),
]


def _run_probes(facts: CodebaseFacts) -> dict[str, Probe]:
    out: dict[str, Probe] = {}
    for spec in _PROBES:
        name, question = spec.name, spec.question
        hits = facts.grep(spec.pattern, spec.suffixes, spec.within, spec.code_only)
        out[name] = Probe(
            name=name,
            question=question,
            present=bool(hits),
            evidence=(f"{facts.rel(hits[0][0])}: {hits[0][1].strip()[:90]}"
                      if hits else "no match in tree"),
        )
    return out


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

_ID = re.compile(r"\bT-[A-Z]+-\d+\b")


def scan(root: Path) -> CodebaseFacts:
    facts = CodebaseFacts(root=root)
    source = root / "Source"

    for path in sorted(source.rglob("*")):
        if not path.is_file() or path.suffix not in SOURCE_SUFFIXES:
            continue
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        facts.files.append(path)
        facts._text[path] = text
        facts._code[path] = strip_comments(text)

        if path.suffix == ".cs" and path.name.endswith(".Build.cs"):
            module = _parse_build_cs(path, text)
            facts.modules[module.name] = module

    # An ID is treated as ASSERTED only where a test actually names it. The GDD says
    # which IDs exist; only the tree says which are gated here. Test files are found by
    # the project's own convention -- a `Tests/` directory -- rather than by filename
    # guessing, because that convention is what StratBridgeParity.cpp and
    # StratDataParity.cpp both already follow.
    for path in facts.files:
        if path.suffix != ".cpp" or "Tests" not in path.parts:
            continue
        for found in sorted(set(_ID.findall(facts._text[path]))):
            facts.asserted_ids.setdefault(found, []).append(facts.rel(path))

    facts.probes = _run_probes(facts)
    return facts
