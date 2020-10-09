from minizinc_testing import yaml
from minizinc_testing.spec import CachedResult

import pytest

# pylint: disable=import-error,no-name-in-module
from py.xml import html
from html import escape
import pytest_html
import re
import minizinc as mzn
from difflib import HtmlDiff
import sys


def pytest_configure(config):
    pytest.solver_cache = {}
    search = config.getoption("--driver")
    if search is not None:
        driver = mzn.find_driver([search])
        if driver is None:
            raise Exception("Failed to find MiniZinc driver in {}".format(search))
        driver.make_default()


def pytest_addoption(parser):
    parser.addoption(
        "--solvers",
        action="store",
        metavar="SOLVERS",
        help="only run tests with the comma separated SOLVERS.",
    )
    parser.addoption(
        "--suite",
        action="append",
        default=[],
        metavar="SUITE_NAME",
        help="Use the given YAML configuration from suites.yml",
    )
    parser.addoption(
        "--all-suites", action="store_true", dest="feature", help="Run all test suites"
    )
    parser.addoption(
        "--driver",
        action="store",
        metavar="MINIZINC",
        help="Directory containing MiniZinc executable",
    )


def pytest_collect_file(parent, path):
    if path.ext == ".mzn":
        return MznFile.from_parent(parent, fspath=path)


def pytest_html_results_table_header(cells):
    cells.insert(2, html.th("Solver", class_="sortable", col="solver"))
    cells.insert(3, html.th("Checker", class_="sortable", col="checker"))
    cells.pop()


def pytest_html_results_table_row(report, cells):
    if hasattr(report, "user_properties"):
        props = {k: v for k, v in report.user_properties}
        cells.insert(2, html.td(props["solver"]))
        cells.insert(3, html.td(props["checker"] if "checker" in props else "-"))
    cells.pop()


@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    report = outcome.get_result()
    extra = getattr(report, "extra", [])
    if report.when == "call" and report.outcome != "skipped":
        props = {k: v for k, v in report.user_properties}
        if "compare" in props:
            required, obtained = props["compare"]
            html_content = """
                <button class="copy-button" onclick="this.nextElementSibling.select();document.execCommand('copy');this.textContent = 'Copied!';">Copy obtained output to clipboard</button>
                <textarea class="hidden-textarea" readonly>{}</textarea>
            """.format(
                escape(obtained)
            )
            actual = obtained.split("\n")
            htmldiff = HtmlDiff(2)
            html_content += '<h4>Diffs</h4><div class="diffs">'
            html_content += "<hr>".join(
                htmldiff.make_table(
                    expected.split("\n"),
                    actual,
                    fromdesc="expected",
                    todesc="actual",
                    context=True,
                )
                for expected in required
            )
            html_content += "</div>"
            extra.append(pytest_html.extras.html(html_content))
        report.extra = extra


def pytest_metadata(metadata):
    # Ensure that secrets don't get shown
    # Can likely be removed after pytest-metadata is updated
    metadata.pop("CI_JOB_TOKEN", None)
    metadata.pop("CI_REPOSITORY_URL", None)
    metadata.pop("CI_REGISTRY_PASSWORD", None)


class MznFile(pytest.File):
    def collect(self):
        with open("./spec/suites.yml", encoding="utf-8") as suites_file:
            suites = yaml.load(suites_file)

            if not self.config.getoption("--all-suites"):
                enabled_suites = self.config.getoption("--suite")
                if len(enabled_suites) == 0:
                    suites = {"default": suites["default"]}
                else:
                    suites = {k: v for k, v in suites.items() if k in enabled_suites}

        with self.fspath.open(encoding="utf-8") as file:
            contents = file.read()
            yaml_comment = re.match(r"\/\*\*\*\n(.*?)\n\*\*\*\/", contents, flags=re.S)
            if yaml_comment is None:
                pytest.skip(
                    "skipping {} as no tests specified".format(str(self.fspath))
                )
            else:
                tests = [doc for doc in yaml.load_all(yaml_comment.group(1))]

                for suite_name, suite in suites.items():
                    if any(self.fspath.fnmatch(glob) for glob in suite.includes):
                        for i, spec in enumerate(tests):
                            for solver in spec.solvers:
                                base = (
                                    str(i) if spec.name is yaml.Undefined else spec.name
                                )
                                name = "{}.{}.{}".format(suite_name, base, solver)
                                cache = CachedResult()
                                yield SolveItem.from_parent(
                                    self,
                                    name=name,
                                    spec=spec,
                                    solver=solver,
                                    cache=cache,
                                    markers=spec.markers,
                                    suite=suite,
                                )
                                for checker in spec.check_against:
                                    yield CheckItem.from_parent(
                                        self,
                                        name="{}:{}".format(name, checker),
                                        cache=cache,
                                        solver=solver,
                                        checker=checker,
                                        markers=spec.markers,
                                        suite=suite,
                                    )


class MznItem(pytest.Item):
    def __init__(self, name, parent, solver, markers, suite):
        super().__init__(name, parent)
        self.user_properties.append(("solver", solver))
        for marker in markers:
            self.add_marker(marker)

        if self.config.getoption("--solvers") is not None:
            self.allowed = [
                x.strip() for x in self.config.getoption("--solvers").split(",")
            ]
            self.allowed = [x for x in self.allowed if x in suite.solvers]
        else:
            self.allowed = suite.solvers

        if not self.solver_allowed(solver):
            self.add_marker(
                pytest.mark.skip("skipping {} not in {}".format(solver, self.allowed))
            )

        if not self.solver_exists(solver):
            self.add_marker(pytest.mark.skip("Solver {} not available".format(solver)))

    def solver_allowed(self, solver):
        return self.allowed is None or solver in self.allowed

    def solver_exists(self, solver):
        solver_exists = pytest.solver_cache.get(solver, None)
        if solver_exists is None:
            try:
                s = mzn.Solver.lookup(solver)
                empty_model = mzn.Model()
                empty_model.add_string("solve satisfy;")
                instance = mzn.Instance(s, empty_model)
                instance.solve()
                solver_exists = True
            except (mzn.MiniZincError, LookupError) as error:
                solver_exists = False
            finally:
                pytest.solver_cache[solver] = solver_exists
        return solver_exists


class SolveItem(MznItem):
    def __init__(self, name, parent, spec, solver, cache, markers, suite):
        super().__init__(name, parent, solver, markers, suite)
        self.spec = spec
        self.solver = solver
        self.cache = cache
        self.default_options = suite.options
        self.strict = suite.strict

    def runtest(self):
        model, result, required, obtained = self.spec.run(
            str(self.fspath), self.solver, default_options=self.default_options
        )

        # To pass model and result to checker test item
        self.cache.model = model
        self.cache.result = result
        self.cache.obtained = obtained

        passed = self.spec.passed(result)

        if not passed:
            # Test fails if we still haven't passed
            expected = [yaml.dump(exp) for exp in required]
            actual = yaml.dump(obtained)
            self.user_properties.append(("compare", (expected, actual)))
            message = "expected one of\n\n{}\n\nbut got\n\n{}".format(
                "\n---\n".join(expected), actual
            )

            # Doesn't match, so backup by checking against another solver
            if isinstance(result, mzn.Result) and result.status.has_solution():
                checkers = [s for s in self.spec.solvers if s is not self.solver]
                if len(checkers) > 0:
                    checker = checkers[0]
                    non_strict_pass = self.cache.test(checker)
                    status = "but passed" if non_strict_pass else "and failed"
                    message += "\n\n{} check against {}.".format(status, checker)

                    if not self.strict and non_strict_pass:
                        print(message, file=sys.stderr)
                        return

            assert False, message

    def reportinfo(self):
        return self.fspath, 0, "{}::{}".format(str(self.fspath), self.name)


class CheckItem(MznItem):
    def __init__(self, name, parent, cache, solver, checker, markers, suite):
        super().__init__(name, parent, solver, markers, suite)
        if not self.solver_allowed(checker):
            self.add_marker(
                pytest.mark.skip(
                    "skipping checker {} not in {}".format(checker, self.allowed)
                )
            )
        if not self.solver_exists(checker):
            self.add_marker(
                pytest.mark.skip("skipping checker {} not available".format(checker))
            )

        self.cache = cache
        self.solver = solver
        self.checker = checker
        self.user_properties.append(("checker", checker))
        self.add_marker(pytest.mark.check)

    def runtest(self):
        if (
            not isinstance(self.cache.result, mzn.Result)
            or not self.cache.result.status.has_solution()
        ):
            pytest.skip("skipping check for no result/solution")
        else:
            passed = self.cache.test(self.checker)
            assert passed, "failed when checking against {}. Got {}".format(
                self.checker, yaml.dump(self.cache.obtained)
            )

    def reportinfo(self):
        return self.fspath, 0, "{}::{}".format(str(self.fspath), self.name)
