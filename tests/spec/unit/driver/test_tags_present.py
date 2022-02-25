from minizinc import default_driver
from minizinc.CLI import CLIDriver
import json
import os


def test_tags_present():
    assert isinstance(default_driver, CLIDriver)
    tags = ["mzn-fzn", "mzn-nl"]
    extra = os.environ.get("MZN_TEST_TAGS_PRESENT")
    if extra is not None:
        tags.extend(i.strip() for i in extra.split(","))
    for tag in tags:
        p = default_driver.run(["--solver-json", tag])
        solver = json.loads(p.stdout)
        assert "id" in solver
