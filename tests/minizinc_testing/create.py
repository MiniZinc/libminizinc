from . import yaml
from . import spec
from . import helpers

from flask import Flask, request, jsonify
from glob import glob
from datetime import timedelta

import re

app = Flask(__name__)


@app.route("/")
def root():
    return app.send_static_file("index.html")


@app.route("/files.json")
def files():
    files = [x[5:].replace("\\", "/") for x in glob("spec/**/*.mzn", recursive=True)]
    return jsonify({"files": files})


@app.route("/file.json")
def file():
    path = "./spec/" + request.args["f"]
    with open(path, encoding="utf-8") as file:
        contents = file.read()
        yaml_comment = re.match(r"\/\*\*\*\n(.*?)\n\*\*\*\/", contents, flags=re.S)
        if yaml_comment is None:
            return jsonify({"cases": []})

        tests = [x for x in yaml.load_all(yaml_comment.group(1))]

        cases = [
            {
                k: v
                for k, v in test.__dict__.items()
                if v is not yaml.Undefined
                and k
                in [
                    "name",
                    "solvers",
                    "check_against",
                    "markers",
                    "type",
                ]
            }
            for test in tests
        ]

        for case, test in zip(cases, tests):
            case["extra_files"] = "\n".join(test.extra_files)
            case["expected"] = (
                [{"value": yaml.dump(expected)} for expected in test.expected]
                if isinstance(test.expected, list)
                else [{"value": yaml.dump(test.expected)}]
            )

            case["options"] = [
                {"key": k, "value": (v if isinstance(v, str) else "")}
                for k, v in test.options.items()
                if k not in ["all_solutions", "timeout"]
            ]
            case["all_solutions"] = (
                "all_solutions" in test.options and test.options["all_solutions"]
            )
            case["timeout"] = (
                test.options["timeout"].total_seconds()
                if "timeout" in test.options
                else 0
            )
        return jsonify({"cases": cases})


def load_spec(data):
    items = {
        k: v
        for k, v in data.items()
        if k in ["name", "solvers", "check_against", "markers", "type"]
    }
    items["extra_files"] = data["extra_files"].splitlines(keepends=False)
    items["expected"] = [yaml.load(x["value"]) for x in data["expected"]]
    items["options"] = {
        x["key"]: (x["value"] if len(x["value"]) > 0 else True) for x in data["options"]
    }
    items["options"]["all_solutions"] = data["all_solutions"]
    if not data["timeout"] == "" and float(data["timeout"]) > 0:
        items["options"]["timeout"] = timedelta(seconds=float(data["timeout"]))
    print(items)
    return spec.Test(**items)


@app.route("/generate.json", methods=["POST"])
def generate():
    path = "./spec/" + request.args["f"]
    variables = request.args["vars"].splitlines(keepends=False)
    mode = request.args["mode"]
    data = request.json

    def filter_args(solution, mode, variables):
        if mode == "exclude":
            for x in variables:
                delattr(solution, x)
        else:
            all_vars = [x for x in solution.__dict__.keys()]
            for x in all_vars:
                if x not in variables:
                    delattr(solution, x)

    test = load_spec(data)

    generated = []
    for solver in test.solvers:
        model, result, required, obtained = test.run(path, solver)
        if not test.passed(result):
            if isinstance(obtained, spec.Result):
                if isinstance(obtained.solution, list):
                    obtained.solution = [
                        filter_args(s, mode, variables) for s in obtained.solution
                    ]
                else:
                    filter_args(obtained.solution, mode, variables)
            test.expected.append(obtained)
            generated.append({"value": yaml.dump(obtained)})

    return jsonify({"obtained": generated})


@app.route("/save.json", methods=["POST"])
def save():
    path = "./spec/" + request.args["f"]
    data = request.json
    dumped = yaml.dump_all(load_spec(x) for x in data)

    with open(path, encoding="utf-8", mode="r+") as file:
        contents = file.read()
        yaml_comment = re.match(
            r"^(.*\/\*\*\*\n)(.*?)(\n\*\*\*\/.*)$", contents, flags=re.S
        )
        output = ""
        if yaml_comment is None:
            output = "/***\n" + dumped + "***/\n\n" + contents
        else:
            output = yaml_comment.group(1) + dumped + yaml_comment.group(3)

        file.seek(0)
        file.truncate()
        file.write(output)

    return jsonify({"status": "success"})


if __name__ == "__main__":
    app.run(debug=True)
