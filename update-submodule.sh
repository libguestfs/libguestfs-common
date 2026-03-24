#!/bin/bash
# (C) Copyright 2026 Red Hat Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# Generate a submodule update commit, with formatted shortlog output.
# Run this from the parent repo like: ./common/update-submodule.sh
# Then `git commit --amend ...` as desired.

set -euo pipefail

if ! git submodule status common &>/dev/null; then
    echo "Error: this script must be run from the top directory of a repo with a 'common/' submodule" >&2
    exit 1
fi

if ! git diff --quiet --exit-code; then
    echo "Error: working tree has uncommitted changes" >&2
    exit 1
fi

echo "Running: git submodule update --remote common"
git submodule update --remote common

if git diff --quiet --exit-code; then
    echo "'common' submodule already up to date"
    exit 0
fi

OLD_COMMIT=$(git ls-tree HEAD common | awk '{print $3}')
NEW_COMMIT=$(git -C common rev-parse HEAD)
echo "Old 'common' commit: $OLD_COMMIT"
echo "New 'common' commit: $NEW_COMMIT"

SHORTLOG=$(git -C common shortlog --no-merges "${OLD_COMMIT}..${NEW_COMMIT}" | sed '/^$/!s/^/  /')
git commit -F - common <<EOF
common: update submodule

$SHORTLOG
EOF
