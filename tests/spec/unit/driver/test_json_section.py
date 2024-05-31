from pathlib import Path
from minizinc import default_driver, Driver
import subprocess
import json


def test_json_section():
    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    model_file = here / "test_json_section.mzn"
    p = subprocess.run(
        [default_driver._executable, model_file, "--json-stream"],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert p.returncode == 0
    messages = [json.loads(l) for l in p.stdout.splitlines(False)]
    solution = next(m for m in messages if m["type"] == "solution")
    expected = {
        "type": "solution",
        "output": {
            "foo": {"a": 1, "b": [2, 3], "c": None},
            "raw": '{"a": 1, "b": [2, 3], "c": null}\n',
            "dzn": "",
        },
        "sections": ["foo", "raw", "dzn"],
    }
    assert solution == expected
