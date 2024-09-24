from pathlib import Path
import subprocess
import json


def test_mip_stats():
    from minizinc import default_driver, Driver

    assert isinstance(default_driver, Driver)
    model_file = Path(__file__).with_suffix(".mzn").resolve()

    p = subprocess.run(
        [
            default_driver._executable,
            model_file,
            "--json-stream",
            "--solver",
            "highs",
        ],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    actual = [json.loads(l) for l in p.stdout.splitlines(False)]
    if any(
        m["type"] == "error"
        and (m["what"] == "plugin loading error" or m["what"] == "configuration error")
        for m in actual
    ):
        import pytest

        pytest.skip("HiGHS not available")
        return
    assert p.returncode == 0
    stats = [
        s for m in actual if m["type"] == "statistics" for s in m["statistics"].keys()
    ]
    assert "objective" not in stats
