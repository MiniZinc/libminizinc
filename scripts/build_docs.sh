#!/usr/bin/env bash
# Build the MiniZinc handbook (English + Chinese, HTML + PDF) into docs-deploy/.
# Expects Sphinx + a LaTeX toolchain on PATH (the `documentation` CI job installs
# them via pip/apt), the repo at $ROOT (default: current dir), a built compiler at
# $ROOT/minizinc, and the vendor deps (gecode + chuffed) at $ROOT/vendor.
set -eux

ROOT="${ROOT:-$PWD}"
cd "$ROOT"

chmod +x minizinc/bin/mzn2doc

cp "$ROOT/vendor/gecode/share/minizinc/gecode/gecode.mzn" "$ROOT/share/minizinc/std/"
cp "$ROOT/vendor/chuffed/share/minizinc/chuffed/chuffed.mzn" "$ROOT/share/minizinc/std/"
echo 'include "globals.mzn"; include "gecode.mzn"; include "chuffed.mzn"; include "ide/vis.mzn"; include "experimental/all.mzn";' \
  > "$ROOT/share/minizinc/std/all.mzn"

./minizinc/bin/mzn2doc --rst-output --include-stdlib --output-base "$ROOT/docs/en/lib" "$ROOT/share/minizinc/std/all.mzn"
./minizinc/bin/mzn2doc --rst-output --include-stdlib --output-base "$ROOT/docs/chi/lib" "$ROOT/share/minizinc/std/all.mzn"

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
