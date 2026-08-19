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
# Hooks are not version-controlled, so this has to be run once per clone:
#     sh Tools/architect/hooks/install.sh

set -eu

REPO_ROOT=$(git rev-parse --show-toplevel)
SRC="$REPO_ROOT/Tools/architect/hooks/pre-commit"
HOOK_DIR="$REPO_ROOT/.git/hooks"
DST="$HOOK_DIR/pre-commit"

if [ -n "$(git config --get core.hooksPath || true)" ]; then
    echo "REFUSING: core.hooksPath is set to '$(git config --get core.hooksPath)'."
    echo "This repo's hooks are not being read from .git/hooks, so a copy there would do"
    echo "nothing. Install into that directory instead, or unset core.hooksPath."
    exit 1
fi

if [ -e "$DST" ]; then
    if cmp -s "$SRC" "$DST"; then
        echo "already installed and identical: $DST"
        exit 0
    fi
    echo "REFUSING: $DST already exists and differs from the one in this directory."
    echo "Not overwriting somebody else's hook. Compare them and merge by hand:"
    echo "    diff $DST $SRC"
    exit 1
fi

mkdir -p "$HOOK_DIR"
cp "$SRC" "$DST"
chmod +x "$DST" 2>/dev/null || true
echo "installed: $DST"
echo "it runs only when Tools/architect/state.md is staged; bypass with --no-verify"
