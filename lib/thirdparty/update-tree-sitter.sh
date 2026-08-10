#!/bin/sh
# Refresh the vendored tree-sitter runtime and the generated MiniZinc parser.
#
# By default both are downloaded from their upstream repositories at the
# versions pinned below. Point either at a local checkout to vendor work that
# is not upstream yet:
#
#   lib/thirdparty/update-tree-sitter.sh
#   lib/thirdparty/update-tree-sitter.sh --grammar ~/src/shackle
#   lib/thirdparty/update-tree-sitter.sh --tree-sitter-version v0.27.0
#
# Options:
#   --tree-sitter DIR          use a local tree-sitter checkout
#   --tree-sitter-version REF  tag to download instead (default below)
#   --grammar DIR              use a local shackle checkout
#   --grammar-ref REF          branch/tag/commit to download instead
#   --generate                 run `tree-sitter generate` rather than taking the
#                              parser.c that comes with the grammar (needs the
#                              tree-sitter CLI; only required when grammar.js
#                              has been edited without regenerating)
#
# Afterwards, put the printed MD5 into lib/cached/md5_cached.cmake.
set -eu

TREE_SITTER_VERSION=v0.26.12
GRAMMAR_REPO=https://github.com/shackle-rs/shackle
GRAMMAR_REF=develop
# The MiniZinc grammar parses models; the DataZinc one parses data files, and is
# a strict subset that rejects what belongs in a model rather than in data.
GRAMMARS="minizinc datazinc"

TREE_SITTER_DIR=
GRAMMAR_DIR=
GENERATE=

while [ $# -gt 0 ]; do
	case $1 in
		--tree-sitter) TREE_SITTER_DIR=$2; shift 2 ;;
		--tree-sitter-version) TREE_SITTER_VERSION=$2; shift 2 ;;
		--grammar) GRAMMAR_DIR=$2; shift 2 ;;
		--grammar-ref) GRAMMAR_REF=$2; shift 2 ;;
		--generate) GENERATE=1; shift ;;
		*) echo "unknown option: $1" >&2; exit 2 ;;
	esac
done

ROOT=$(cd "$(dirname "$0")/../.." && pwd)
RUNTIME=$ROOT/lib/thirdparty/tree_sitter
PUBLIC_INCLUDE=$ROOT/include/minizinc/_thirdparty/tree_sitter

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

fetch() { # url, destination directory
	mkdir -p "$2"
	curl -fsSL "$1" | tar xz -C "$2" --strip-components=1
}

if [ -z "$TREE_SITTER_DIR" ]; then
	echo "downloading tree-sitter $TREE_SITTER_VERSION"
	fetch "https://github.com/tree-sitter/tree-sitter/archive/refs/tags/$TREE_SITTER_VERSION.tar.gz" \
	      "$WORK/tree-sitter"
	TREE_SITTER_DIR=$WORK/tree-sitter
fi

if [ -z "$GRAMMAR_DIR" ]; then
	echo "downloading $GRAMMAR_REPO @ $GRAMMAR_REF"
	fetch "$GRAMMAR_REPO/archive/refs/heads/$GRAMMAR_REF.tar.gz" "$WORK/shackle"
	GRAMMAR_DIR=$WORK/shackle
fi
# --- runtime ---------------------------------------------------------------
# A complete, unmodified copy of upstream's lib/src, so that any part of the
# runtime is available if it is ever needed.
rm -rf "$RUNTIME"
mkdir -p "$RUNTIME/portable" "$RUNTIME/unicode" "$RUNTIME/wasm"
cp "$TREE_SITTER_DIR"/lib/src/*.c "$TREE_SITTER_DIR"/lib/src/*.h "$RUNTIME/"
cp "$TREE_SITTER_DIR"/lib/src/portable/* "$RUNTIME/portable/"
cp "$TREE_SITTER_DIR"/lib/src/unicode/* "$RUNTIME/unicode/"
cp "$TREE_SITTER_DIR"/lib/src/wasm/* "$RUNTIME/wasm/"
cp "$TREE_SITTER_DIR/LICENSE" "$RUNTIME/LICENSE"
mkdir -p "$PUBLIC_INCLUDE"
cp "$TREE_SITTER_DIR/lib/include/tree_sitter/api.h" "$PUBLIC_INCLUDE/"

# Local patch, still needed as of v0.26.12. `repeat_depth` counts how deep a
# repetition has nested, and the parser uses it to rebalance long repetitions
# into a shallow tree. At 65536 elements it overflows, rebalancing stops, and
# the tree stays thousands of levels deep -- which makes every cursor field
# lookup walk thousands of ancestors. A .dzn holding one array of a few million
# values then takes quadratic time. Drop this once upstream widens the field.
sed -i.orig 's/^      uint16_t repeat_depth;$/      uint32_t repeat_depth;/' "$RUNTIME/subtree.h"
grep -q 'uint32_t repeat_depth;' "$RUNTIME/subtree.h" || {
	echo "repeat_depth patch no longer applies -- check whether it is still needed" >&2
	exit 1
}
rm -f "$RUNTIME/subtree.h.orig"

# --- grammars --------------------------------------------------------------
echo
for name in $GRAMMARS; do
	src=$GRAMMAR_DIR/parsers/tree-sitter-$name
	dest=$ROOT/lib/thirdparty/tree_sitter_$name

	if [ -n "$GENERATE" ]; then
		# No path argument: passing src/grammar.json makes the CLI read that file
		# and ignore grammar.js, which is the one thing this option is for.
		( cd "$src" && tree-sitter generate --abi=15 )
	fi
	rm -rf "$dest"
	mkdir -p "$dest"
	cp "$src/src/parser.c" "$ROOT/lib/thirdparty/tree_sitter_$name.c"
	cp "$src/grammar.js" "$src/src/grammar.json" "$src/src/node-types.json" "$dest/"
	cp "$GRAMMAR_DIR/LICENSE" "$dest/LICENSE"

	md5=$(md5 -q "$dest/grammar.js" 2>/dev/null ||
	      md5sum "$dest/grammar.js" | cut -d' ' -f1)
	echo "set(ts_${name}_grammar_js_md5_cached \"$md5\")"
done
echo
echo "Put those lines into lib/cached/md5_cached.cmake."
