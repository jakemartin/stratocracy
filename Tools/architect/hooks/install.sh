#!/bin/sh
#
# Installs this directory's `pre-commit` into `.git/hooks/`, by COPY.
#
# WHY A COPY AND NOT `core.hooksPath`. This repository already carries four Git LFS hooks
# (`post-checkout`, `post-commit`, `post-merge`, `pre-push`) in `.git/hooks/`. Setting
# `core.hooksPath` redirects ALL hook lookups to one directory, so pointing it at this one
# would disable every LFS hook silently -- no error, no warning, just LFS quietly not
# running. Measured before choosing: those four exist and are LFS's.
#
# WHY IT REFUSES TO CLOBBER. `.git/hooks/pre-commit` was empty when this was written, but
# a future clone may not be, and overwriting somebody's hook to install a documentation
# check is not a trade this script gets to make on their behalf.
#
# Hooks are not version-controlled, so this has to be run once per clone -- ONCE PER CLONE,
# not once per worktree: every linked worktree of a clone shares this one directory.
#     sh Tools/architect/hooks/install.sh

set -eu

REPO_ROOT=$(git rev-parse --show-toplevel)
SRC="$REPO_ROOT/Tools/architect/hooks/pre-commit"

# `$REPO_ROOT/.git/hooks` IS WRONG IN A LINKED WORKTREE, AND IT FAILED LOUDLY THERE. In a
# worktree created by `git worktree add`, `.git` is a 55-byte FILE pointing at the common
# directory, not a directory -- so `mkdir -p "$REPO_ROOT/.git/hooks"` died with
# `mkdir: cannot create directory '.../probe/.git': Not a directory` and `set -eu` exited 1.
# Measured on a probe worktree 2026-08-20.
#
# THE SAME MEASUREMENT SHOWED THE INSTALL IS USUALLY NOT NEEDED THERE AT ALL: hook lookup in a
# linked worktree resolves through the COMMON directory, so the copy already installed in the
# main tree DOES run in every worktree. `git rev-parse --git-path hooks` in the probe returned
# `E:/MultiAgent/Stratocracy/.git/hooks` and the hook fired. Running this script from a
# worktree is therefore a no-op that reports "already installed and identical" -- which is the
# honest answer, and better than the exit-1 it used to give.
#
# `--path-format=absolute` IS LOAD-BEARING AND WAS MEASURED, NOT ASSUMED. Bare
# `--git-common-dir` answers `.git` -- RELATIVE -- from the main tree, while a worktree gets an
# absolute path. A relative answer resolves against the caller's cwd, which is not necessarily
# the repo root, so the bare form swaps one wrong path for another. Measured on git 2.53.0:
#     main tree: `--git-common-dir` -> `.git`   `--path-format=absolute ...` -> `E:/.../.git`
#     worktree : `--git-common-dir` -> `E:/.../.git` (both forms agree)
# The fallback covers a git older than 2.31, where `--path-format` does not exist; it is
# written rather than measured, and is marked as such.
GIT_COMMON=$(git rev-parse --path-format=absolute --git-common-dir 2>/dev/null || true)
if [ -z "$GIT_COMMON" ]; then
    # UNVERIFIED on this machine: no git older than 2.31 was available to run it against.
    GIT_COMMON=$(git rev-parse --git-common-dir)
    case "$GIT_COMMON" in
        /*|?:[\/]*) ;;
        *) GIT_COMMON="$REPO_ROOT/$GIT_COMMON" ;;
    esac
fi
HOOK_DIR="$GIT_COMMON/hooks"
DST="$HOOK_DIR/pre-commit"

if [ -n "$(git config --get core.hooksPath || true)" ]; then
    echo "REFUSING: core.hooksPath is set to '$(git config --get core.hooksPath)'."
    echo "This repo's hooks are not being read from .git/hooks, so a copy there would do"
    echo "nothing. Install into that directory instead, or unset core.hooksPath."
    exit 1
fi

# A HOOK THAT IS VERSIONED NEEDS AN UPDATE PATH, and the first version of this script had
# none: once the tracked `pre-commit` gained a check, every clone with the older copy hit
# "REFUSING ... differs" and had to be fixed by hand. The refusal still stands for a hook
# this project did not write -- recognised by a signature line that our own carries -- so
# somebody else's hook is still never clobbered.
SIGNATURE="Stratocracy pre-commit hook"

if [ -e "$DST" ]; then
    if cmp -s "$SRC" "$DST"; then
        echo "already installed and identical: $DST"
        exit 0
    fi
    if grep -q "$SIGNATURE" "$DST" 2>/dev/null; then
        cp "$SRC" "$DST"
        chmod +x "$DST" 2>/dev/null || true
        echo "updated an older copy of this project's hook: $DST"
        exit 0
    fi
    echo "REFUSING: $DST already exists, differs from the one in this directory, and does not"
    echo "carry this project's signature line -- so it is somebody else's hook, not a stale"
    echo "copy of ours. Not overwriting it. Compare them and merge by hand:"
    echo "    diff $DST $SRC"
    exit 1
fi

mkdir -p "$HOOK_DIR"
cp "$SRC" "$DST"
chmod +x "$DST" 2>/dev/null || true
echo "installed: $DST"
echo "it runs only when a file under Tools/architect/state/ is staged; bypass with --no-verify"
