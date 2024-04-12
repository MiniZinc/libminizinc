import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))

from conf import *

templates_path = ["../_templates"]
html_static_path = ["../_static"]
html_logo = "../logo/MiniZn_logo_2.svg"
latex_logo = "../logo/MiniZn_logo_2_small.pdf"
latex_additional_files = ["../utils/mznstyle.sty"]
