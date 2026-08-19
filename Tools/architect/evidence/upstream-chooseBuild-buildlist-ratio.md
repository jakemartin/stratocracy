# Upstream request — `strat::chooseBuild` and the "occasional Tank" buildlist

**Filed against** `E:\MultiAgent\stratocracy-crew`, HEAD `11ef8ce10673edfaac52dba50e23930e501d16e3`
**Subject** `strat::chooseBuild` / `strat::buildPriorityLess` in `cpp_reference/Ai.good.cpp`, and
`AiState::buildlist` in `cpp_reference/Ai.h`.
**Status** Investigation only. Nothing was edited in either repo. No stage, no commit.

---

## 1. The measured behaviour

The characterization is **correct**. `chooseBuild` filters the buildlist to the affordable
entries and then returns the minimum under `buildPriorityLess`, which is ascending `costFame`.

`chooseBuild`, in full, from `cpp_reference/Ai.good.cpp`:

```cpp
int chooseBuild(const AiState& s, int side) {
    if (!validSide(side)) return -1;
    std::vector<int> affordable;
    for (int idx : s.buildlist) {
        if (idx < 0 || static_cast<std::size_t>(idx) >= s.unitDefs.size()) continue;
        if (s.unitDefs[idx].costFame <= s.economy.side[side].fameTotal) affordable.push_back(idx);
    }
    if (affordable.empty()) return -1;
    // §2.9: it spends and replaces losses instead of hoarding, so the cheapest
    // affordable buildlist unit is bought rather than saving for a dearer one.
    int best = affordable.front();
    for (int idx : affordable)
        if (buildPriorityLess(s.unitDefs[idx], s.unitDefs[best])) best = idx;
    return best;
}
```

The comparator, `buildPriorityLess`, same file:

```cpp
bool buildPriorityLess(const UnitDef& a, const UnitDef& b) {
    // Q9, ruled: Infantry > Recon > Artillery > Tank, which is ASCENDING §2.4 cost
    // (100/150/200/300) and NOT the order §2.4's table prints (Infantry, Tank,
    // Artillery, Recon). Cost is read from the table, so the priority follows the
    // data rather than a list rewritten here.
    if (a.costFame != b.costFame) return a.costFame < b.costFame;
    return static_cast<int>(a.type) < static_cast<int>(b.type);   // pinned enum order
}
```

Three consequences follow from those two named blocks, and only those two:

1. **Cheapest-affordable, with a cost-only primary key.** The `type`-enum leg is reached only on
   a `costFame` tie. In the shipped `Data/units.csv` no two units tie
   (`Infantry,100 / Recon,150 / Artillery,200 / Tank,300`), so on the shipped table the selection
   is purely minimum cost.

2. **A duplicate expresses nothing.** `chooseBuild` reads `s.buildlist` only as a set to filter;
   the loop takes a minimum, and a minimum is insensitive to multiplicity. `{Infantry, Tank}`,
   `{Infantry, Tank, Tank, Tank}` and `{Tank, Infantry}` all return the Infantry defIndex.
   There is no counter, no cursor, no index-into-the-list, and no state carried between calls —
   `chooseBuild` is a pure function of `(buildlist, unitDefs, fameTotal)`.

3. **With Infantry present, Tank is unreachable at any Fame level.** Tank is affordable only when
   `fameTotal >= 300`, and at `fameTotal >= 300` Infantry (100) is affordable too and strictly
   cheaper. There is no Fame value at which Tank is affordable and Infantry is not. So the Tank
   entry is dead code in the rules layer for as long as Infantry shares the list. The GDD's
   §2.9 "an occasional Tank" is, at the rules layer, **not an observable outcome**.

The crew's own gate agrees with the reading. `cpp_reference/test_ai.cpp`, in the block the file
labels `T-AI-06 determinism-and-Q9-tie-breaks`, has to *remove Infantry from the list* to get
anything but the cheapest — its comment says so:

```cpp
// Build priority: with only Tank and Recon affordable, Recon is built --
// Recon is dearer than Infantry but cheaper than Tank, and the printed table
// order would pick Tank.
...
run(t, "ai buildlist Tank Recon");
```

That is the clause proving the property; it is not evidence that a Tank can ever be built from a
list that also contains Infantry.

**Caller-side context.** `nextCommand`'s economy-phase block calls `chooseBuild(s, side)` once per
held, un-built, non-pending factory hex in canonical order, and returns the first Build command it
forms. Each call re-reads the same `fameTotal`, so within one `nextCommand` invocation the choice
cannot vary; across invocations it varies only as `fameTotal` moves. Nothing in that block biases
toward a dearer unit.

**Provenance of the phrase.** The GDD sentence being interpreted is in §2.9's economy-phase bullet:
"from a default buildlist (mostly Infantry, an occasional Tank), spawning per §2.7." The GDD gives
no ratio, no period, and no rule for "occasional".

---

## 2. Crew vs. vendored — they agree, byte for byte

`Source/StratRules/Ai.good.cpp` and `Ai.h` are **identical** to the crew repo's git blobs at crew
HEAD, and identical to what the vendoring manifest records. Upstream has **not** moved ahead on the
AI files.

```
$ cd /e/MultiAgent/stratocracy-crew && git rev-parse HEAD
11ef8ce10673edfaac52dba50e23930e501d16e3

$ python -c "... json ... StratRules.manifest.json ... ['rulesCommit']"
cb8e12b3a897c7329497ced4d1c6207630f37101

$ for f in Ai.good.cpp Ai.h; do  # crew git blob at HEAD  vs  vendored working file
Ai.good.cpp
  crew HEAD blob : 5bffcc489b23a0fb51e4fc2670cfe9810d5947f12b3df856ea8aade75492191e
  vendored file  : 5bffcc489b23a0fb51e4fc2670cfe9810d5947f12b3df856ea8aade75492191e
  manifest says  : 5bffcc489b23a0fb51e4fc2670cfe9810d5947f12b3df856ea8aade75492191e
Ai.h
  crew HEAD blob : a70a275b7c6c656f7da111ef73fe6c7ba09dc2045856342faac05484d42e024f
  vendored file  : a70a275b7c6c656f7da111ef73fe6c7ba09dc2045856342faac05484d42e024f
  manifest says  : a70a275b7c6c656f7da111ef73fe6c7ba09dc2045856342faac05484d42e024f
```

Confirmed independently by git log: **no commit touches the AI between the vendored point and crew
HEAD.**

```
$ git log --oneline cb8e12b3..HEAD -- cpp_reference/Ai.good.cpp cpp_reference/Ai.h spec/ai_spec.md
(empty)

$ git log --oneline cb8e12b3..HEAD
11ef8ce Correct the comment that said T-INT-03 does not consume the fixture
c2f5860 Widen the parity fixture to the complete command set
b5f524d Delete the closure clauses 9289c1d asserted, under the Director's ruling
5072d10 Name the unvendored module Balance, not Selfplay
9289c1d Repair the claims the bridge landing made false
5c47cc1 Vendor the parity fixture: a third kind on the data path
862a225 The parity fixture T-INT-02 replays in-engine

$ git diff --stat cb8e12b3..HEAD
 README.md                      |  16 +-
 cpp_reference/test_balance.cpp |   8 +-
 cpp_reference/test_replay.cpp  | 348 +++++++++++++++++++++++++++++++++++++++--
 cpp_reference/test_save.cpp    |   9 +-
 crew/offline.py                |  11 +-
 crew/tools.py                  |  84 ++++++----
 data/parity_fixture.save       | 181 +++++++++++++++++++++
 spec/balance_spec.md           |   6 +-
 spec/integration_spec.md       |  35 +++--
 spec/replay_spec.md            |  10 +-
 spec/save_spec.md              |   7 +-
 sync_stratdata.py              |  37 +++--
 12 files changed, 666 insertions(+), 86 deletions(-)
```

### One trap worth recording

A naive `sha256sum` of the crew **working tree** file disagrees with the vendored file — the crew
working tree is checked out CRLF, the vendored bytes are LF:

```
$ file Stratocracy/Source/StratRules/Ai.good.cpp stratocracy-crew/cpp_reference/Ai.good.cpp
Stratocracy/Source/StratRules/Ai.good.cpp:  C++ source, Unicode text, UTF-8 text
stratocracy-crew/cpp_reference/Ai.good.cpp: C++ source, Unicode text, UTF-8 text, with CRLF line terminators

$ diff -u --strip-trailing-cr Stratocracy/Source/StratRules/Ai.good.cpp stratocracy-crew/cpp_reference/Ai.good.cpp
AI.CPP DIFF EXIT=0
$ diff -u --strip-trailing-cr Stratocracy/Source/StratRules/Ai.h stratocracy-crew/cpp_reference/Ai.h
AI.H DIFF EXIT=0
```

The manifest's own note already says the check is against the **git object store**, not the working
tree, and the object-store hashes above match exactly. Anyone re-running this comparison should
compare `git cat-file blob`, not the checked-out file, or they will manufacture a false "upstream
has moved".

### Where upstream *has* moved

Not on the AI, but worth knowing before anyone plans work: the crew repo carries a **`Balance`
module** (`cpp_reference/Balance.h`, `Balance.good.cpp`, `Balance.buggy.cpp`, `test_balance.cpp`)
that has **no counterpart in the vendored `Source/StratRules/`**. `Balance.h` describes it as
self-play harness territory and `spec/balance_spec.md` repeats the same "the buildlist is data, no
ratio" position. If self-play measurement of a build mix is ever wanted, that module is the
existing home for it and should not be re-invented on the UE side.

---

## 3. Does anything upstream already address it? — No. Reporting the gap.

Searched and found **no implementation** of a ratio, weight, quota, period, cursor, or
alternating/round-robin build policy anywhere in the crew repo.

```
$ grep -rniE "ratio|weight|occasional|proportion|mix of|randomiz|random |shuffle|round.?robin|rotate" \
    --include=*.md --include=*.cpp --include=*.h .
```

Every hit is one of: `Combat.good.cpp`'s `hpRatio` damage term; hex/enumeration `order` prose;
`test_*.cpp` fixtures that deliberately `reverse`/scramble a container to prove order-independence;
or a comment *declining* to invent a ratio. No selection-policy code.

The absence is **deliberate and documented**, not an oversight. Three places say so in their own
words:

- `Ai.h`, the comment on the `buildlist` member of `AiState`:
  "§2.9 describes it as 'mostly Infantry, an occasional Tank' and gives no ratio or rule, so the
  list is DATA the caller supplies; inventing a ratio here would be a rule the GDD does not have.
  Q9's priority orders whatever it contains."
- `spec/ai_spec.md`, under **Five stated readings**, item 1 ("The buildlist is data"):
  "§2.9 calls it 'mostly Infantry, an occasional Tank' and gives no ratio, no period and no rule.
  The module takes the list as a caller-supplied parameter and applies Q9's priority among its
  affordable members. Inventing a ratio would be a balance rule the GDD has not written."
- `Driver.h`, the comment on `Session::buildlist`: "the driver invents no ratio."

`spec/ai_spec.md` has a **"Change requests for the Director"** section carrying two existing
requests (the per-turn build slot, and the single `hasActed` flag). **This is not among them** — no
open note, no TODO, and no ruled question covers build variety. Q9, the only ruled question in the
neighbourhood, rules the *tie-break priority*, not the mix.

So: the design intent upstream is that the mix is the **caller's** problem, and the caller currently
has no mechanism that can express one either. `FStratBridge::SetBuildlistByIds` /
`BuildlistDefIndexes` (in `E:\MultiAgent\Stratocracy\Source\StratBridge\StratBridge.h`) pass an
ordered list of defIndexes straight through to `AiState::buildlist` — which, per §1 above, discards
order and multiplicity. **A list is the wrong shape to carry a ratio.** That is the actual defect:
not that `chooseBuild` is wrong, but that the contract hands the caller a container whose only
expressible content is a *set*, while telling the caller the mix is theirs to decide.

---

## 4. Design options

### Option A — leave `chooseBuild` alone; change the authored buildlist

Author the list as Infantry-only, or Tank-only, per side or per scenario. Zero rules change, zero
re-vendor.

- **Cost:** none in code. But it does not deliver "an occasional Tank" — it delivers *always
  Infantry* or *always Tank*. The GDD sentence stays unsatisfied and the Tank entry stays dead.
  This is the honest do-nothing, and it should be named as such rather than described as a fix.

### Option B — weighted selection inside `chooseBuild`

Give buildlist entries weights and pick by weight. This is the shape most people reach for first,
and it is the wrong one here: it needs a source of variation, and `Ai.h`'s contract is
"**Pure function of state; no RNG anywhere; no clock; no I/O**" (`spec/ai_spec.md`, *Determinism /
constraints*). An RNG would break determinism, break `T-AI-06`'s same-state-same-command clause,
and break the replay/parity fixtures that `test_replay.cpp` and the bridge parity test depend on.

- **Cost:** high, and it breaks a load-bearing invariant. **Not recommended.**

### Option C — deterministic quota, carried as caller-supplied fact

Make the mix expressible without RNG, in the same idiom `AiState` already uses for `builtThisTurn`:
the caller supplies a **fact**, the AI reads it. Concretely, the buildlist becomes an ordered
sequence plus a caller-supplied cursor (or a per-defIndex "built so far this match" tally), and
`chooseBuild` returns the affordable entry that is furthest behind its authored share, ties by
`buildPriorityLess`.

- Determinism is preserved — same state in, same command out — because the cursor/tally is *state*,
  not randomness. It survives save/replay for free if it lands in the same place `builtThisTurn`
  does.
- **Cost:** a change to the `AiState` shape (a new member), a change to `chooseBuild`, a new spec
  invariant and at least one new `T-AI-*` clause, plus a bridge-side field and a re-vendor. It also
  needs a Director ruling first, because a quota **is** a balance rule the GDD has not written —
  the very thing `Ai.h` and `ai_spec.md` decline to invent unilaterally.

### Option D — affordability-aware ceiling (buy the dearest affordable, not the cheapest)

Invert the comparator. Rejected on sight: it contradicts §2.9's explicit "It spends Fame and
replaces losses instead of hoarding" *and* Q9's ruled Infantry > Recon > Artillery > Tank priority,
which is a **ruling**, not a stated reading. Changing it needs the ruling reversed.

- **Cost:** cheap in code, expensive in governance, and it makes Infantry the dead entry instead of
  Tank. **Not recommended.**

### Recommendation

**Ask the Director to rule, and file this as a change request in `spec/ai_spec.md`'s existing
"Change requests for the Director" section** — alongside the two already there. The request is not
"implement a ratio"; it is:

> §2.9 says "mostly Infantry, an occasional Tank". `chooseBuild` returns the cheapest affordable
> entry, and `AiState::buildlist` is a container that expresses neither order nor multiplicity, so
> "an occasional Tank" has no representation anywhere in the rules layer and the Tank entry is
> unreachable whenever Infantry shares the list. Either §2.9's phrase is decorative — in which case
> say so, and Option A's Infantry-only list is correct and complete — or a mix is intended, in which
> case the buildlist needs a shape that can carry one (Option C), which is a balance rule and
> therefore a Director call.

Cost of the recommendation itself: one spec edit upstream, no code, no re-vendor. It puts the
decision where the repo's own conventions already put decisions of this kind, and it does not spend
implementation effort on a mechanism before anyone has ruled that the mechanism should exist.

**Until it is ruled, prefer Option A and author an Infantry-only buildlist** — because a list
containing Tank is currently *misleading*: it reads as if a Tank might be built, and no Tank ever
will be.

---

## 5. What I did NOT verify

Stated in its own terms, so nothing below is quietly folded into the claims above.

- **I did not run anything.** No compile, no `python run.py`, no headless UE suite, no self-play.
  Every claim in §1 is read off the source text of `chooseBuild` and `buildPriorityLess`, not
  observed at runtime. Specifically, I did not empirically produce a game in which a Tank is never
  built; I argued it from the comparator and the shipped cost table.
- **I did not verify what buildlist the UE project actually authors.** I searched
  `Stratocracy/Source`, `Config`, `Tools` and `.agents` for `buildlist`/`BuildList` and found only
  the bridge's `SetBuildlistByIds`/`BuildlistDefIndexes` plumbing and its parity test (which seeds
  from *all* loaded unit ids, a test fixture, not shipped configuration). **I did not check
  `Content/` — `.uasset` Blueprint defaults were not inspected at all.** So the premise "Infantry
  and Tank are both present in the authored buildlist" is *plausible and consistent with what the
  bridge allows*, but I did not confirm any shipped list containing both. If the authored list is
  Infantry-only today, the Tank is unreachable in principle but nothing is currently misbehaving.
- **I did not verify Fame accounting.** `chooseBuild` compares against
  `s.economy.side[side].fameTotal`. Whether pending builds are debited from `fameTotal` before the
  next `chooseBuild` call, and therefore whether a side can queue more builds than it can pay for,
  is `Economy.h`/`Turn.h` territory that I did not read past the declaration comment. It does not
  affect the cheapest-wins conclusion — any monotone budget still admits Infantry whenever it
  admits Tank — but it is not something I measured.
- **I did not audit the `Balance` module's contents.** I established that it exists in the crew repo
  and is absent from the vendored tree, and read its header comment and `spec/balance_spec.md`'s
  buildlist paragraph. I did not read `Balance.good.cpp`, so I cannot say whether its self-play
  harness would need changing under Option C.
- **I did not diff data files beyond `units.csv`.** `units.csv` matches between crew and vendored
  modulo line endings (`diff --strip-trailing-cr` clean). `effectiveness.csv`, `terrain.csv`,
  `ferrum_crossing.json` and `parity_fixture.save` were not compared — note that
  `data/parity_fixture.save` is one of the files that changed in `cb8e12b3..HEAD`, so the **data**
  side may well be ahead of the vendored copy even though the AI **source** is not. That is a
  separate question from this one and is left open here.
- **I did not read the whole GDD.** I sliced §2.9 around the "default buildlist" phrase (two
  occurrences, both quoted or cited above). There may be balance language elsewhere in the document
  that bears on the mix and that I never saw.
