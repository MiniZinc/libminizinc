from pathlib import Path
from minizinc import default_driver, Driver
import subprocess
import json


def test_model_interface_only_1():
    assert isinstance(default_driver, Driver)
    p = subprocess.Popen(
        [default_driver._executable, "-", "--model-interface-only", "-c"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    stdout, stderr = p.communicate(
        input=b'int: n; var 1..3: x; solve minimize x; output ["foo"];'
    )
    assert p.returncode == 0
    interface = json.loads(stdout)
    assert interface == {
        "type": "interface",
        "input": {"n": {"type": "int"}},
        "output": {"x": {"type": "int"}},
        "method": "min",
        "has_output_item": True,
        "included_files": [],
        "globals": [],
    }


def test_model_interface_only_2():
    assert isinstance(default_driver, Driver)
    p = subprocess.Popen(
        [default_driver._executable, "-", "--model-interface-only", "-c"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    stdout, stderr = p.communicate(input=b"int: n; var 1..3: x; solve maximize x;")
    assert p.returncode == 0
    interface = json.loads(stdout)
    assert interface == {
        "type": "interface",
        "input": {"n": {"type": "int"}},
        "output": {"x": {"type": "int"}},
        "method": "max",
        "has_output_item": False,
        "included_files": [],
        "globals": [],
    }


def test_model_interface_only_3():
    assert isinstance(default_driver, Driver)
    p = subprocess.Popen(
        [default_driver._executable, "-", "--model-interface-only", "-c"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    stdout, stderr = p.communicate(
        input=b'float: n; var set of 1..3: x; output ["foo"];'
    )
    assert p.returncode == 0
    interface = json.loads(stdout)
    assert interface == {
        "type": "interface",
        "input": {"n": {"type": "float"}},
        "output": {"x": {"type": "int", "set": True}},
        "method": "sat",
        "has_output_item": True,
        "included_files": [],
        "globals": [],
    }
