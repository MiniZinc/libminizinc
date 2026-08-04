#!/usr/bin/env bash
# Download pinned vendor dependencies into ./vendor/<dep>.
#
# Usage: fetch_vendor.sh <triple> <dep>...
#
# Versions come from ./vendor.lock; assets from the per-dependency release
# `<dep>-<version>` in MiniZinc/minizinc-vendor. Replaces the old
# `download_vendor`, which always pulled vendor master.
#
# Requires: gh (via $GH_TOKEN), tar.
set -euo pipefail

TRIPLE="${1:?usage: fetch_vendor.sh <triple> <dep> [<dep> ...]}"
shift
REPO="${VENDOR_REPO:-MiniZinc/minizinc-vendor}"
here="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
mkdir -p "$here/vendor"

version_of() {
  grep -E "^$1=" "$here/vendor.lock" | head -1 | cut -d= -f2-
}

for dep in "$@"; do
  ver="$(version_of "$dep")"
  [ -n "$ver" ] || { echo "no version pinned for '$dep' in vendor.lock" >&2; exit 1; }
  asset="${dep}-${ver}-${TRIPLE}.tar.gz"
  echo "Fetching $asset from $REPO"
  gh release download "${dep}-${ver}" --repo "$REPO" --pattern "$asset" --dir "$here" --clobber
  tar -xzf "$here/$asset" -C "$here/vendor"
  rm -f "$here/$asset"
done
