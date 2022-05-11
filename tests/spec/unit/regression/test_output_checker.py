from pathlib import Path
from minizinc import Instance, Model, Solver, Status, default_driver, Driver
import json
from minizinc_testing.utils import temporary_file


def test_output_checker():
    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    model_file = here / "test_output_checker.mzn"
    checker_file = here / "test_output_checker.mzc.mzn"
    p = default_driver._run([model_file, checker_file, "--output-mode", "checker"])
    messages = [json.loads(l) for l in p.stdout.splitlines(False)]
    solutions = [m for m in messages if m["type"] == "solution"]
    assert len(solutions) > 0
    for s in solutions:
        model = Model(checker_file)
        instance = Instance(Solver.lookup("gecode"), model)
        with temporary_file(
            s["output"]["dzn"], suffix=".dzn"
        ) as dzn, instance.branch() as child:
            child.add_file(dzn)
            solution = child.solve()
            assert solution.status in [
                Status.ALL_SOLUTIONS,
                Status.OPTIMAL_SOLUTION,
                Status.SATISFIED,
            ]
