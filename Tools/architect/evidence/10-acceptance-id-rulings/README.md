# 10 — the acceptance-ID rulings and the T-UI-05 → T-INT-05 rename

Evidence for the 2026-08-29 pass recorded at the top of `Tools/architect/state/global.md`'s
`## NEXT`: the acceptance-ID rulings for W4, W5, W6 and W8; the ruling that `T-UI-05` stops at
the snapshot; and the four clause names that second ruling moved to `T-INT-05`.

## Why this directory exists, and it is a process fix rather than a wave artefact

**No `strat-integration-reviewer` report had ever been persisted in this repository.** A gate
report existed only inside the session that ran it, so every sentence in the state files quoting
a gate *in its own words* was unfalsifiable from a checkout — a reader could reproduce the
measurements a report was said to have made, and could not check that it said them. The re-gate
below raised that itself, as the most serious item on a zero-finding report, and recommended
persisting reports rather than relying on the account staying faithful.

This directory is the first instance. Two reports from the same pass are here because the pass
took two gates.

| File | Verdict | Subject |
|---|---|---|
| `gate-1-block.md` | `VERDICT: BLOCK`, two findings | The whole pass: the rulings, the four renames, the record corrections |
| `gate-2-pass.md` | `VERDICT: PASS`, zero findings | Narrow re-gate of the fixes, plus what could have moved since |

## Provenance, stated rather than implied

**The reports were produced by `strat-integration-reviewer`. The files were written by the
`coordinator`, on the user's direct instruction, by transcribing each report's text as it was
returned.** `Tools/architect/` is `strat-data-steward`'s lane and this is a lane exception, of
the same kind and on the same authority as the two the pass had already taken — both of which
were the subject of `gate-1`'s findings, which is why the exception is written here and not only
in `global.md`.

**What this closes and what it does not.** It closes the gap the re-gate named: a reader of a
checkout can now read what the gate actually said instead of taking the state file's account of
it. It does NOT make the text self-attesting — the `coordinator` transcribed it, no signature or
hash covers it, and nothing outside this repository can be compared against. A reader who needs
more than transcription has to re-run the gate. That limit is stated because a persisted report
reads as stronger evidence than it is, and this project has a recorded failure of exactly that
shape: an artefact that looks like an independent record while sharing an author with the claim
it supports.

**Neither gate built the project and neither ran the suite**, and both say so in their own text.
The `Result: Succeeded` line and the suite figure for this pass are the `coordinator`'s readings,
carried in `global.md`'s banner. The clause count behind the figure is the gates', independently
derived, twice.
