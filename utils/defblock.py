def setup(app):
    app.add_directive('defblock', DefBlockDirective)

    return {'version': '0.1'}   # identifies the version of our extension

from docutils import nodes

from docutils.parsers.rst.directives.admonitions import BaseAdmonition

from sphinx.locale import _

class DefBlockDirective(BaseAdmonition):

    # title of the defblock
    required_arguments = 1
    optional_arguments = 100
    # this enables content in the directive
    has_content = True
    node_class = nodes.admonition

    def run(self):
        env = self.state.document.settings.env

        self.arguments = [' '.join(self.arguments)]

        targetid = "defblock-%d" % env.new_serialno('defblock')
        targetnode = nodes.target('', '', ids=[targetid], title="bla")

        ad = super(DefBlockDirective, self).run()

        return [targetnode] + ad
