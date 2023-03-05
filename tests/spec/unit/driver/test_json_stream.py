from pathlib import Path
import subprocess
import json
import sys
import os
from tempfile import NamedTemporaryFile
from contextlib import contextmanager


@contextmanager
def named_temp_file(*args, **kwargs):
    # Workaround for temp files on Windows
    temp = NamedTemporaryFile(delete=False, *args, **kwargs)
    try:
        yield temp
    finally:
        temp.close()
        os.unlink(temp.name)


def test_json_stream():
    from minizinc import default_driver, Driver
    expected = [
        {"type": "trace", "section": "default", "message": "foo\n"},
        {"type": "comment", "comment": "% This is a comment\n"},
        {"type": "statistics", "statistics": {"foo": 123, "bar": "string"}},
        {
            "type": "solution",
            "output": {"dzn": "x = 1;\ny = 1;\n"},
            "sections": ["dzn"],
        },
        {
            "type": "solution",
            "output": {"dzn": "x = 1;\ny = 2;\n"},
            "sections": ["dzn"],
        },
        {
            "type": "solution",
            "output": {"dzn": "x = 1;\ny = 3;\n"},
            "sections": ["dzn"],
        },
        {
            "type": "solution",
            "output": {"dzn": "x = 2;\ny = 3;\n"},
            "sections": ["dzn"],
        },
        {
            "type": "solution",
            "output": {"dzn": "x = 3;\ny = 3;\n"},
            "sections": ["dzn"],
        },
        {"type": "status", "status": "OPTIMAL_SOLUTION"},
    ]

    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    model_file = here / "test_json_stream.mzn"
    with named_temp_file(suffix=".msc", mode="w", encoding="utf-8") as fp:
        json.dump(
            {
                "name": "Test solver",
                "version": "1.0",
                "id": "org.minizinc.test_solver",
                "executable": [
                    Path(sys.executable).resolve().as_posix(),
                    Path(__file__).resolve().as_posix(),
                ],
            },
            fp,
        )
        fp.close()
        p = subprocess.run(
            [
                default_driver._executable,
                model_file,
                "--json-stream",
                "--solver",
                fp.name,
            ],
            stdin=None,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        assert p.returncode == 0
        actual = [json.loads(l) for l in p.stdout.splitlines(False)]
        assert expected == actual


if __name__ == "__main__":
    # Dummy solver output
    print("% This is a comment")
    print("%%%mzn-stat: foo=123")
    print('%%%mzn-stat: bar="string"')
    print("%%%mzn-stat-end")
    obj = 0
    for x in range(1, 4):
        for y in range(1, 4):
            if x + y > obj:
                obj = x + y
                print(f"x = {x};")
                print(f"y = {y};")
                print("----------")
    print("==========")
