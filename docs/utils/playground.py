from docutils import nodes
from urllib.parse import quote
from pathlib import Path
import json


class playground(nodes.General, nodes.Element):
    pass


def visit_playground_node_html(self, node):
    refuri = node["refuri"]
    self.body.append('<span class="mzn-playground-button">')
    self.body.append(
        f'<a href="{refuri}" target="_blank" title="Open in Playground">&#9654;</a>'
    )
    self.body.append("</span>")


def depart_playground_node_html(self, node):
    pass


def visit_playground_node_latex(self, node):
    raise nodes.SkipNode


def playground_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    node = playground("", project=text)
    return [node], []


def process_playground_nodes(app, doctree, fromdocname):
    if app.builder.format == "html":
        for node in doctree.findall(playground):
            project = node["project"]
            node["refuri"] = app.builder.get_relative_uri(
                fromdocname, f"playground-{project}"
            )


def generate_playground_links(app):
    if app.builder.format == "html":
        src = Path(app.srcdir) / "examples"
        lang = {"zh-CN": "chi"}.get(app.config.language, app.config.language)
        localised_src = Path(app.srcdir).parent / lang / "examples"
        for project in src.iterdir():
            if project.is_dir():
                playground_project = {}
                files = []
                for mzp in project.glob("*.mzp"):
                    ide_project = json.loads(mzp.read_text())
                    for file in ide_project["openFiles"]:
                        p = project / file
                        localised = localised_src / project.name / p.name
                        if localised.exists():
                            p = localised
                        files.append({"name": p.name, "contents": p.read_text()})
                    for file in ide_project["projectFiles"]:
                        if file not in ide_project["openFiles"]:
                            p = project / file
                            localised = localised_src / project.name / p.name
                            if localised.exists():
                                p = localised
                            files.append(
                                {
                                    "name": p.name,
                                    "contents": p.read_text(),
                                    "hidden": True,
                                }
                            )
                    solver_id_map = {
                        "org.gecode.gecode": "org.minizinc.gecode_presolver",
                        "org.chuffed.chuffed": "org.minizinc.chuffed",
                    }
                    solver = ide_project.get(
                        "selectedBuiltinConfigId", "org.gecode.gecode"
                    )
                    solver = solver_id_map.get(solver, solver)
                    playground_project["solverId"] = solver
                    playground_project["tab"] = ide_project["openTab"]
                    break
                else:
                    paths = []
                    for file in project.iterdir():
                        if file.is_file():
                            localised = localised_src / project.name / file.name
                            if localised.exists():
                                paths.append(localised)
                            else:
                                paths.append(file)
                    paths.sort(
                        key=lambda x: (
                            {
                                ".mzn": 0,
                                ".mzc": 0,
                                ".dzn": 1,
                                ".json": 2,
                                ".html": 3,
                            }.get(x.suffix, 4),
                            x.name,
                        )
                    )
                    files = [{"name": p.name, "contents": p.read_text()} for p in paths]

                playground_project["files"] = files
                object = json.dumps(playground_project)
                url = f"https://play.minizinc.dev/#project={quote(object)}"
                yield f"playground-{project.name}", {
                    "redirect_url": url
                }, "redirect.html"
    return []


def setup(app):
    app.add_node(
        playground,
        html=(visit_playground_node_html, depart_playground_node_html),
        latex=(visit_playground_node_latex, None),
    )
    app.add_role("playground", playground_role)
    app.connect("doctree-resolved", process_playground_nodes)
    app.connect("html-collect-pages", generate_playground_links)
    return {
        "version": "0.1",
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
