#!/usr/bin/env bash
# Build the MiniZinc handbook (English + Chinese, HTML + PDF) into docs-deploy/.
#
# Expects Sphinx + a LaTeX toolchain on PATH and mzn2doc already built at
# $ROOT/build (see .github/workflows/docs.yml).
#
# The gecode/chuffed libraries are pulled from upstream rather than from a vendor
# build: only their .mzn declarations are needed to document them, and this keeps
# the step identical to .readthedocs.yaml's.
set -eux

ROOT="${ROOT:-$PWD}"
cd "$ROOT"

curl -fsSL "https://raw.githubusercontent.com/Gecode/gecode/main/gecode/flatzinc/mznlib/gecode.mzn" \
  -o share/minizinc/std/gecode.mzn
curl -fsSL "https://raw.githubusercontent.com/chuffed/chuffed/stable/chuffed/flatzinc/mznlib/chuffed.mzn" \
  -o share/minizinc/std/chuffed.mzn

./build/mzn2doc --rst-output --include-stdlib --output-base "$ROOT/docs/en/lib" "$ROOT/docs/all.mzn"
./build/mzn2doc --rst-output --include-stdlib --output-base "$ROOT/docs/chi/lib" "$ROOT/docs/all.mzn"

cd "$ROOT/docs"
make BUILDDIR="en/_build" html latexpdf
make SPHINXOPTS="-D language=zh_CN" BUILDDIR="chi/_build" html latexpdf

MZNVERSION=$(python3 "$ROOT/docs/utils/minizinc_version.py")
mkdir -p "$ROOT/docs-deploy/doc-${MZNVERSION}/en" "$ROOT/docs-deploy/doc-${MZNVERSION}/chi"
cp -r "$ROOT/docs/en/_build/html/"* "$ROOT/docs-deploy/doc-${MZNVERSION}/en/"
cp "$ROOT/docs/en/_build/latex/MiniZinc.pdf" "$ROOT/docs-deploy/doc-${MZNVERSION}/en/MiniZinc Handbook.pdf"
cp -r "$ROOT/docs/chi/_build/html/"* "$ROOT/docs-deploy/doc-${MZNVERSION}/chi/"
cp "$ROOT/docs/chi/_build/latex/MiniZinc.pdf" "$ROOT/docs-deploy/doc-${MZNVERSION}/chi/MiniZinc Handbook.pdf"
python3 "$ROOT/docs/utils/gen_redirects.py" "$ROOT/docs-deploy/doc-${MZNVERSION}" "$ROOT/docs-deploy/doc-latest"
