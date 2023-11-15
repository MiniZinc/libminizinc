from pathlib import Path
from minizinc import default_driver, Driver
import subprocess
import json


def _run_test(model_file):
    p = subprocess.run(
        [default_driver._executable, model_file, "--json-stream", "-a"],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    p.check_returncode()
    result = [json.loads(l) for l in p.stdout.splitlines(False)]
    messages = [m for m in result if m["type"] in ["trace", "solution"]]
    assert len(messages) == 4
    trace = messages[0]
    assert trace["type"] == "trace"
    assert trace["message"] == {"url": "foo.html", "userData": {"n": 4}}
    assert messages[1]["type"] == "solution"
    assert messages[2]["type"] == "solution"
    assert messages[3]["type"] == "solution"
    expected = [{"x": 3, "y": 2}, {"x": 2, "y": 3}, {"x": 3, "y": 3}]
    actual = [s["output"]["mzn_vis_0"] for s in messages[1:]]
    assert len(actual) == len(expected)
    for sol in actual:
        assert sol in expected
    assert actual == expected


def test_vis_ann():
    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    _run_test(here / "test_vis_ann.mzn")


def test_vis_custom():
    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    _run_test(here / "test_vis_custom.mzn")
