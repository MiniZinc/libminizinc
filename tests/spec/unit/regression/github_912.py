from pathlib import Path
from minizinc import default_driver, Driver
import subprocess
import json


def test_github_912():
    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    model_file = here / "github_912.mzn"
    p = subprocess.run(
        [default_driver._executable, model_file, "--json-stream"],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert p.returncode == 1
    messages = [json.loads(l) for l in p.stdout.splitlines(False)]
    errors = [m for m in messages if m["type"] == "error"]
    assert len(errors) == 1
    entries = [s for s in errors[0]["stack"] if s["isCompIter"]]
    assert len(entries) == 1
    assert entries[0]["description"] == "x = A"
