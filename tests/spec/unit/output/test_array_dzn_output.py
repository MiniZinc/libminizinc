from pathlib import Path
import subprocess
import json


def test_array_dzn_output():
    from minizinc import default_driver, Driver

    expected = {
        "type": "solution",
        "output": {"dzn": "y = array3d('a b'..'a b', 'a b'..'a b', c..c, [1]);\n"},
        "sections": ["dzn"],
    }

    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    model_file = here / "test_array_dzn_output.mzn"

    p = subprocess.run(
        [
            default_driver._executable,
            model_file,
            "--json-stream",
            "--solver",
            "gecode",
        ],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert p.returncode == 0
    actual = [json.loads(l) for l in p.stdout.splitlines(False)]
    assert expected in actual
