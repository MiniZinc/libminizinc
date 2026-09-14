#!/bin/sh
# Refresh the generated MiniZinc and DataZinc tree-sitter parsers.
#
# CMake downloads the pinned runtime. Generate ABI 15 parser tables to match it.
#
# By default, download the pinned grammar branch. To use a local checkout:
#
#   lib/thirdparty/update-tree-sitter.sh
#   lib/thirdparty/update-tree-sitter.sh --grammar ~/src/shackle
#
# Options:
#   --grammar DIR    use a local shackle checkout
#   --grammar-ref REF  branch/tag/commit to download instead
#   --generate       run `tree-sitter generate` rather than taking the parser.c
#                    that comes with the grammar (needs the tree-sitter CLI;
#                    only required when grammar.js has been edited without
#                    regenerating)
#
# Afterwards, put the printed MD5 into lib/cached/md5_cached.cmake.
set -eu

GRAMMAR_REPO=https://github.com/shackle-rs/shackle
GRAMMAR_REF=develop
GRAMMARS="minizinc datazinc"

GRAMMAR_DIR=
GENERATE=

while [ $# -gt 0 ]; do
	case $1 in
		--grammar) GRAMMAR_DIR=$2; shift 2 ;;
		--grammar-ref) GRAMMAR_REF=$2; shift 2 ;;
		--generate) GENERATE=1; shift ;;
		*) echo "unknown option: $1" >&2; exit 2 ;;
	esac
done

ROOT=$(cd "$(dirname "$0")/../.." && pwd)

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

fetch() { # url, destination directory
	mkdir -p "$2"
	curl -fsSL "$1" | tar xz -C "$2" --strip-components=1
}

if [ -z "$GRAMMAR_DIR" ]; then
	echo "downloading $GRAMMAR_REPO @ $GRAMMAR_REF"
	fetch "$GRAMMAR_REPO/archive/refs/heads/$GRAMMAR_REF.tar.gz" "$WORK/shackle"
	GRAMMAR_DIR=$WORK/shackle
fi

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
