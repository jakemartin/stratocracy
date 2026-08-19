# Fixtures — `08-combat-pairing-gate`

11 checked-in `.log` files for `Tools/architect/strat_combat_pairing_gate.py`. This directory
carried no such note before this pass — the `--test-path` requirement lived only in
`../blackboard.md` and the gate's own docstring, neither of which a reader opening this
directory directly would necessarily see.

**Ten of these eleven fixtures MUST be run with
`--test-path Stratocracy.Fixture.FIX-01.FixtureTest`.** They carry their own
`Test Started.` / `Test Completed.` marker pair under that path, and the gate's marker-slicing
mode hard-fails (`SliceFailure`) if it cannot find both.

**One fixture, `fail_pre_sliced_zero_events.log`, MUST be run with `--pre-sliced` and
NO `--test-path`.** `--pre-sliced` ignores `--test-path` entirely (see `run_gate`'s own
docstring in `strat_combat_pairing_gate.py`), and this file is empty on disk — it carries no
markers to slice on regardless, which is the point: it proves the `--pre-sliced` zero-event
guard refuses an empty corpus by default rather than reading `0 == 0` as a trivially-satisfied
`PASS`.

Do not run all eleven the same way. There is no single uniform invocation across this
directory.

| Fixture | Invocation | Expected |
|---|---|---|
| `pass_clean_pairing.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | PASS, EXIT=0 |
| `pass_commentary_echo_ignored.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | PASS, EXIT=0 |
| `pass_terminal_refusal_ok.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | PASS, EXIT=0 |
| `fail_ordering_scramble.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | FAIL, EXIT=1 |
| `fail_silent_path_gap.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | FAIL, EXIT=1 |
| `fail_format_drift.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | FAIL, EXIT=1 |
| `fail_divergence_present.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | FAIL, EXIT=1 |
| `fail_blocking_ai_refusal.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | FAIL, EXIT=1 |
| `fail_missing_markers.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | FAIL, EXIT=1 |
| `fail_truncated_no_completed.log` | `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` | FAIL, EXIT=1 |
| `fail_pre_sliced_zero_events.log` | `--pre-sliced` (no `--test-path`) | FAIL, EXIT=1 |

**This table is a human-readable copy, not the authority.** The re-runnable, falsifiable
binding lives in `Tools/architect/strat_fixture_verdict_binding.py` — run it
(`python Tools/architect/strat_fixture_verdict_binding.py`) rather than trusting this table by
eye; it re-derives the fixture-file count from the directory listing and re-runs each fixture
through its declared invocation against the real gate, so it cannot drift out of sync with this
directory's actual contents the way `gate_self_test_output.txt` once did.

See `../blackboard.md` ("The gate can fail" / "Fixture-verdict binding" sections) for the full
account, including the falsifiability measurement (two deliberate breaks, reverted).
