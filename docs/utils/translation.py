from pathlib import Path
import re

_TL_ROOT = Path(__file__).parent.parent

_TL_REGEX = r"\.\. literalinclude::\s*(.*)\s*|:download:`.*<(.*)>`"


def translate_doc(app, docname, source):
    lang = {"zh_CN": "chi"}.get(app.config.language, app.config.language)
    if lang == "en":
        return
    translated = _TL_ROOT / lang / f"{docname}.rst"
    contents = translated.read_text() if translated.exists() else source[0]
    result = ""
    pos = 0
    for m in re.finditer(_TL_REGEX, contents):
        i = 1
        file = m.group(i)
        if file is None:
            i = 2
            file = m.group(i)
        if (_TL_ROOT / lang / file).exists():
            # Translated version of this file available
            start, end = m.span(i)
            result += contents[pos:start]
            result += f"../{lang}/{file}"
            pos = end
    result += contents[pos:]
    source[0] = result


def setup(app):
    app.connect("source-read", translate_doc)
    return {
        "version": "0.1",
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
