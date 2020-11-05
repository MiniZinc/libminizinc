from docutils import nodes

def setup(app):
    app.add_role('bugref', bugref_role)
    app.add_role('idebugref', idebugref_role)
    app.add_role('docbugref', docbugref_role)

    return {'version': '0.1'}   # identifies the version of our extension
    
def bugref_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    issue_num = int(text)
    if issue_num <= 0:
        msg = inliner.reporter.error(
            '"%s" is an invalid issue number' % text, line=lineno)
        prb = inliner.problematic(rawtext, rawtext, msg)
        return [prb], [msg]
    node = nodes.reference(rawtext, 'issue ' + str(issue_num), refuri='https://github.com/minizinc/libminizinc/issues/'+str(issue_num))
    return [node], []

def idebugref_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    issue_num = int(text)
    if issue_num <= 0:
        msg = inliner.reporter.error(
            '"%s" is an invalid issue number' % text, line=lineno)
        prb = inliner.problematic(rawtext, rawtext, msg)
        return [prb], [msg]
    node = nodes.reference(rawtext, 'issue ' + str(issue_num), refuri='https://github.com/minizinc/minizincide/issues/'+str(issue_num))
    return [node], []

def docbugref_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    issue_num = int(text)
    if issue_num <= 0:
        msg = inliner.reporter.error(
            '"%s" is an invalid issue number' % text, line=lineno)
        prb = inliner.problematic(rawtext, rawtext, msg)
        return [prb], [msg]
    node = nodes.reference(rawtext, 'issue ' + str(issue_num), refuri='https://github.com/minizinc/minizinc-doc/issues/'+str(issue_num))
    return [node], []

