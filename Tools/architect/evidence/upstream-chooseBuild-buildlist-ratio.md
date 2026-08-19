# Upstream request — `strat::chooseBuild` and the "occasional Tank" buildlist

**Filed against** `E:\MultiAgent\stratocracy-crew`, HEAD `11ef8ce10673edfaac52dba50e23930e501d16e3`
**Subject** `strat::chooseBuild` / `strat::buildPriorityLess` in `cpp_reference/Ai.good.cpp`, and
`AiState::buildlist` in `cpp_reference/Ai.h`.
**Status** Investigation only. Nothing was edited in either repo. No stage, no commit.
**UPDATED 2026-08-19 — the Director has ruled; see §6.** §§1–5 were written while the question was
open and are unchanged. The ruling is a per-type population cap, which is none of §4's Options A–D;
§4's "ask the Director to rule" recommendation is therefore DISCHARGED, while its interim advice
(author an Infantry-only buildlist until the mechanism lands) still stands. Still investigation only:
nothing is implemented, and the mechanism lives in vendored bytes this repo may not edit.

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

---

## 6. THE DIRECTOR HAS RULED — a per-type population cap (2026-08-19)

§4's recommendation was "ask the Director to rule". **That has now happened, and this section
records the ruling.** §§1–5 above are left exactly as written; they were true when written and the
measurements in them are unaffected. What changes is only that the open question at the end of §4 is
no longer open.

### 6.1 The ruling, in the Director's own terms

> A good deterministic methodology would be to instruct the AI to not have more than x number of
> units on the map at one time. Let's say for this example there are a maximum of 3 infantry units
> on the board. When that threshold is met, the AI will save its Fame points until it can buy a
> Tank.

This is **not** any of Options A–D. It is a fifth shape, and a better one than the Option C this
document recommended: it needs no cursor, no tally, and no new persisted state, because the quantity
it keys on — how many of a type are on the board — is already in `AiState` and already survives save
and replay by virtue of being the board.

### 6.2 Why it fits the module's stated constraints — read off the source, not run

Four checks, each against the vendored source:

1. **The cap is computable from state the AI already holds.** `AiState::units` is a
   `std::vector<AiUnit>`, and `AiUnit` carries `side` and `defIndex` (`Ai.h`). `UnitDef` carries
   `type` and `costFame` (`Data.h`). So "how many Infantry does this side have alive" is a loop over
   existing members. **Nothing new needs to reach the AI**, which matters because `Ai.h`'s own
   contract is "It also sees only real state (§4.7 Stub 6: 'the AI cheats at nothing'). There is no
   hidden field on `AiState` that a player could not read off the board." A unit count is the most
   player-visible fact there is — you can see it by looking at the map.
2. **Determinism survives intact.** The cap introduces no RNG, no clock, no I/O, and no state
   carried between calls. Same state in, same command out. This is the invariant that killed
   Option B, and the cap does not touch it. `T-AI-06`'s same-state-same-command clause and the
   replay/parity fixtures are unaffected.
3. **"Save its Fame" needs no new code — it is already the behaviour of the existing path.**
   `nextCommand`'s economy block reads, at the decision point:
   ```cpp
   const int defIndex = chooseBuild(s, side);
   if (defIndex < 0) continue;
   ```
   When the affordable-and-eligible set is empty, `chooseBuild` returns `-1`, no `Build` command is
   formed, and the side spends nothing that turn. Fame accrues by doing nothing. **Hoarding is
   emergent from the cap, not a second mechanism to build**, and it ends by itself the moment Fame
   reaches the dearer unit's cost or a casualty frees a slot.
4. **Q9 is untouched.** The cap filters *eligibility*; `buildPriorityLess` still orders whatever
   survives the filter. The ruled Infantry > Recon > Artillery > Tank priority is not reversed, not
   reinterpreted, and not in tension with this — which is what separates this ruling from Option D.

### 6.3 Two encodings for the cap number, and which to prefer

The cap values themselves are the only genuinely new data. There are two places to put them.

**Encoding 1 — buildlist multiplicity finally means something.** `AiState::buildlist` is already a
`std::vector<int>` that carries duplicates, and §1's second consequence is the finding that those
duplicates currently express *nothing*. Under the cap reading they express the cap directly:
`{Infantry, Infantry, Infantry, Tank}` means "at most 3 Infantry alive, at most 1 Tank alive". The
Director's example maps onto it exactly. `FStratBridge::SetBuildlistByIds` already passes an ordered
list with duplicates straight through, so **the bridge needs no change at all**, and the authored
Blueprint default becomes the balance knob.

- Cost: `chooseBuild` changes; `AiState`'s shape does not; the bridge's shape does not.
- Risk, and it is real: it silently caps every type at its multiplicity, so a single `Tank` entry
  means "at most 1 Tank alive" whether or not anyone intended a Tank cap. A list that today reads
  as a menu becomes a quota table, and every existing authored list changes meaning on the day the
  rules module is re-vendored. That is a migration, not a no-op.

**Encoding 2 — an explicit parallel cap vector.** A new `AiState` member, e.g.
`std::vector<int> buildCaps` indexed the way `unitDefs` is, with a sentinel for "uncapped".
Caller-supplied data, in the same idiom `buildlist` and `builtThisTurn` already use.

- Cost: an `AiState` member, a bridge-side field and setter, a re-vendor. More work than Encoding 1.
- Benefit: it says what it means, an uncapped type is expressible, and no existing authored list
  changes meaning behind anyone's back.

**Recommendation to the Director: Encoding 2.** Encoding 1 is more elegant and cheaper, and it is
tempting precisely because it redeems the dead multiplicity this document opened by complaining
about — but "every existing buildlist silently acquires caps equal to how many times someone happened
to type each entry" is the kind of change that reads as a bug for a year. Encoding 1 is worth putting
to upstream as the cheaper alternative and letting them weigh it; it is not worth choosing here.

### 6.4 The trap that would ship as a bug: pending builds must count toward the cap

**A cap that counts only `s.units` is wrong, and wrong in a way that only shows up on a board with
more than one factory.** `nextCommand`'s economy block loops over every held factory hex in canonical
order and calls `chooseBuild` afresh for each one, and a queued build does not become a unit
immediately — it sits in `s.economy.pending` as a `PendingBuild`, which carries `side` and
`defIndex` (`Economy.h`), holding the factory's slot until it spawns. So with 2 Infantry alive, a cap
of 3, and three held factories, a units-only count sees `2 < 3` three times in a row and queues three
more, landing at 5 alive against a cap of 3.

The population the cap tests against must therefore be **alive units of that side and type PLUS
pending builds of that side and type**. Both sources are already in `AiState`; this is a correctness
requirement on the implementation, not another design decision. It should be stated in the change
request and it should carry its own clause upstream, because it is exactly the kind of
off-by-a-factory that a single-factory test fixture passes.

### 6.5 One tension to put in front of the Director, not to resolve here

§2.9 says the AI "spends Fame and replaces losses instead of hoarding". **A cap makes it hoard** —
bounded, purposeful hoarding aimed at a specific dearer unit, which is plainly what the ruling
intends, but hoarding against the letter of a GDD sentence all the same. Option D was rejected in §4
partly for contradicting that same sentence, so consistency demands the tension be named rather than
waved through because this version is more palatable.

The honest framing for upstream: the ruling does not contradict §2.9 so much as **supply the missing
half of it**. §2.9 gives "mostly Infantry, an occasional Tank" with no mechanism, and the
anti-hoarding clause exists to stop the AI sitting on Fame indefinitely. A cap produces saving that is
bounded (it ends at the dearer unit's cost, or at the first casualty) and directed (it exists to reach
the Tank §2.9 asks for). But that is an interpretation, and §2.9 is GDD text, so it wants the
Director's explicit sign-off in the same breath as the cap itself — not an inference drawn by whoever
implements it.

### 6.6 What this ruling does NOT do

- **It does not make anything true in this repository.** The mechanism lives in `strat::chooseBuild`,
  which is vendored certified bytes under `Source/StratRules/`. Nothing here can be implemented on
  the UE side. This document remains a request to `E:\MultiAgent\stratocracy-crew`, now with a ruling
  attached instead of a question.
- **It does not change the interim advice in §4.** Until the mechanism is built and re-vendored, a
  buildlist containing Tank is still misleading, because a Tank still cannot be built. Option A
  (author Infantry-only) remains the correct interim posture, and it costs nothing to reverse.
- **It is not measured.** Every claim in §6.2 is read off the source text of `Ai.h`, `Data.h`,
  `Economy.h` and `nextCommand`'s economy block. Nothing was compiled and nothing was run. In
  particular the claim in §6.2 item 3 — that `defIndex < 0` results in no spend — is read off two
  lines of control flow, not observed in a game.
- **It does not check the shipped authored buildlist.** §5's gap stands unchanged: `Content/`
  `.uasset` Blueprint defaults were never inspected, so whether any shipped list actually contains
  both Infantry and Tank is still unconfirmed.
- **It does not set the cap numbers.** "3 Infantry" is the Director's worked example, not a balance
  decision this document records as final. The mechanism is ruled; the values are data and can move
  without touching it.

### 6.7 The change request, ready to file upstream

For `spec/ai_spec.md`'s existing "Change requests for the Director" section, in the idiom of the two
already there:

> **Build variety — a per-type population cap.** §2.9's "mostly Infantry, an occasional Tank" has no
> representation in the rules layer: `chooseBuild` returns the cheapest affordable buildlist entry,
> so with Infantry in the list the Tank entry is unreachable at every Fame level. **Ruled by the
> Director (2026-08-19): cap the number of units of a type a side may have on the board at once.**
> When a type is at its cap it is ineligible; `buildPriorityLess` orders whatever remains eligible
> and affordable; when nothing is eligible and affordable, `chooseBuild` returns -1 as it does today
> and the side accrues Fame until the cap frees or the dearer unit becomes affordable. No RNG, no
> cursor, no new persisted state — the population is board state the AI already holds and that save
> and replay already carry. Two open items for upstream: (a) where the cap numbers live — buildlist
> multiplicity (free, but silently re-reads every existing authored list as a quota table) or an
> explicit parallel cap vector (recommended); and (b) confirmation that the cap counts
> `economy.pending` alongside alive units, without which a multi-factory board overshoots the cap by
> one per extra factory in a single turn. Also wants an explicit note on §2.9's "instead of
> hoarding", which a cap bounds rather than obeys.
