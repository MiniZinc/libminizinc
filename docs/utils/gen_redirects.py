from __future__ import print_function, unicode_literals

import io
import os
import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: " + sys.argv[0] + " <DOC_DIR> <OUT_DIR>", file=sys.stderr)
        exit(1)
    in_dir = os.path.realpath(sys.argv[1])
    out_dir = os.path.realpath(sys.argv[2])

    in_base = os.path.basename(in_dir)
    out_base = os.path.basename(out_dir)

    redirects = []
    for root, dirs, files in os.walk(in_dir):
        for file in files:
            path = os.path.join(root, file)
            if file.endswith(".html"):
                source = os.path.join(out_base, os.path.relpath(path, in_dir))
                destination = os.path.relpath(path, os.path.dirname(in_dir))
                redirects.append((source, destination))
            elif file == "MiniZinc Handbook.pdf":
                source = os.path.join(
                    out_base,
                    os.path.relpath(os.path.join(root, "handbook.html"), in_dir),
                )
                destination = os.path.relpath(path, os.path.dirname(in_dir))
                redirects.append((source, destination))
    redirects.append(
        (
            os.path.join(out_base, "index.html"),
            os.path.join(in_base, "en/index.html"),
        )
    )
    redirects.append(
        (
            os.path.join(out_base, "index-en.html"),
            os.path.join(in_base, "en/index.html"),
        )
    )

    for src, dst in redirects:
        source = src.replace("\\", "/")
        destination = dst.replace("\\", "/")
        print("Redirecting " + source + " to " + destination + ".", file=sys.stderr)
        out_file = os.path.join(os.path.dirname(out_dir), source)
        out_path = os.path.dirname(out_file)
        if not os.path.exists(out_path):
            os.makedirs(out_path)
        with io.open(out_file, "w", encoding="utf-8", newline="\n") as f:
            f.write("---\n")
            f.write('redirect_to: "/' + destination + '"\n')
            f.write("---\n")

    print("Done.", file=sys.stderr)
