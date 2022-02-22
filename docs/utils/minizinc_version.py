import os
import re


def _parse_version():
    script_dir = os.path.dirname(os.path.realpath(__file__))
    path = os.path.join(script_dir, "../../CMakeLists.txt")
    if os.path.exists(path):
        with open(path, "r") as fp:
            contents = fp.read()
            match = re.search(
                r"^\s*project\(libminizinc\s+VERSION\s+((\d+\.\d+)\.\d+)",
                contents,
                re.M,
            )
            if match is not None:
                return match.group(1), match.group(2)
    return "<unknown>", "<unknown>"


mzn_release, mzn_version = _parse_version()
