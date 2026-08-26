# Vendored kb snapshot

| | |
|---|---|
| File | `setting.md` |
| Source | `E:\MultiAgent\stratocracy-content\kb\setting.md` |
| Bytes | 2,111 |
| sha256 | `8b1a6c85ff1152ef5345f20fb36d2cf7f3829266649132ca5c3ee08eb6a2ad71` |
| git hash-object | `d8d2d0590f30f5d9153c8a12f77ed4763cae6c03` |
| Taken | 2026-08-26 |

## Why a copy exists here

`Source/StratUI/Tests/StratMatchResultModelClauses.cpp` pins GDD §2.11.4's six result
lines against `kb/setting.md`'s banned register — nine words, listed in that file's own
**Banned register** bullet under "Tone bible", which is what the clause parses. That
document lives in a **different repository** (`stratocracy-content`), and a headless
automation run may not reach outside this tree. Without a copy here, the clause has no
way to READ the banned register at run time and the words end up typed as a literal
instead — a guard that types its own reference data goes inert on its own subject
silently, which this project has already paid for more than once. So this snapshot
exists purely so the clause has something in-tree to read.

The nine words are **deliberately not reproduced in this manifest.** Typing them here
a second time would recreate the exact failure this vendoring exists to prevent, one
layer up: this file would then be a second piece of reference data that can silently
disagree with `setting.md` if the register ever changes, and — because a manifest
reads as authoritative — a future maintainer could trust the stale copy here over the
one the clause actually parses. There is exactly one place in this repo where the nine
words are written down: `setting.md`'s own **Banned register** bullet. Read it there.
Do not paste it back into this file.

## This is a snapshot, not a sync

Unlike `Source/StratRules/` and `Data/`, nothing hash-checks this file against its
source and no acceptance ID asserts over it. There is no `sync_*.py` behind it and no
gate that fails when it goes stale. It is **not** part of the vendoring discipline
those directories follow.

If `kb/setting.md` moves on, this copy silently does not. Re-take it with a plain copy
and update the hash above. Where the two disagree, **the document in
`stratocracy-content` is the real one.**

## Measure it this way, and mind the CRLF trap

`gdd_snapshot/MANIFEST.md` records that a byte count taken through a CRLF-applying
reader reported that snapshot as 449,498 bytes when it is really 446,133 — one added
byte per line. Use the same discipline here: measure with a reader that cannot
silently convert, and prove that same reading can SEE a carriage return before
trusting a count of zero.

    wc -c < setting.md                       # 2111
    tr -cd '\r' < setting.md | wc -c          # 0
    printf 'a\r\nb\r\n' | tr -cd '\r' | wc -c # 2  <- the control
    sha256sum setting.md                      # must equal the hash above
    git hash-object setting.md                # must equal the git hash-object above

The source is LF (0 carriage returns over 40 lines) and the vendored copy must stay
LF — a checkout can silently rewrite line endings, so the `git hash-object` value
above is the tripwire: if a future `git hash-object` on this file disagrees, something
rewrote it.
