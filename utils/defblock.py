def setup(app):
    app.add_node(defblock,
                 html=(visit_defblock_node, depart_defblock_node),
                 latex=(visit_defblock_node, depart_defblock_node),
                 text=(visit_defblock_node, depart_defblock_node))

    app.add_directive('defblock', DefBlockDirective)
    app.connect('env-purge-doc', purge_defblocks)

    return {'version': '0.1'}   # identifies the version of our extension

from docutils import nodes

class defblock(nodes.Admonition, nodes.Element):
    pass

def visit_defblock_node(self, node):
    self.visit_admonition(node)

def depart_defblock_node(self, node):
    self.depart_admonition(node)

from docutils.parsers.rst import Directive

from sphinx.util.compat import make_admonition
from sphinx.locale import _

class DefBlockDirective(Directive):

    # title of the defblock
    required_arguments = 1
    optional_arguments = 100
    # this enables content in the directive
    has_content = True

    def run(self):
        env = self.state.document.settings.env

        targetid = "defblock-%d" % env.new_serialno('defblock')
        targetnode = nodes.target('', '', ids=[targetid])

        title = ' '.join(self.arguments)

        ad = make_admonition(defblock, self.name, [title], self.options,
                             self.content, self.lineno, self.content_offset,
                             self.block_text, self.state, self.state_machine)

        if not hasattr(env, 'defblock_all_defblocks'):
            env.defblock_all_defblocks = []
        env.defblock_all_defblocks.append({
            'docname': env.docname,
            'lineno': self.lineno,
            'defblock': ad[0].deepcopy(),
            'target': targetnode,
        })

        return [targetnode] + ad

def purge_defblocks(app, env, docname):
    if not hasattr(env, 'defblock_all_defblocks'):
        return
    env.defblock_all_defblocks = [defblock for defblock in env.defblock_all_defblocks if defblock['docname'] != docname]
