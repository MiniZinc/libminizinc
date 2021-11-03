#!/usr/bin/env python3

# This script will generate interfacing headers from the official headers of solvers.
# Note that since this is not an actual parser for the headers this is not a foolproof
# method. Please make sure that the generated file do not contain multiple definitions
# of the same identifier. This generally suggest a ifdef statement was read ignored.
#
# Generally this script goes over the header files given as "source_names" and looks
# for declarations of macros, enumerated types, and typedefinitions. It then compares
# each of these definitions to see if they are used in the files declared in "usage".
# Additional control is added by the "ignore" and "include" fields to correct
# the generated header file.

import os
import re
import sys
from itertools import chain
from pathlib import Path
from typing import List, Optional

STDCALL_PREPEND = """#if !defined(WIN32) && !defined(WIN64)
#define __cdecl
#define __stdcall
#endif
"""


def find_file(file_name: str, search_path: List[Path]) -> Optional[Path]:
    for p in search_path:
        path = p / file_name
        if path.exists():
            return path
    return None


def remove_comments(code: str):
    code = re.sub(r"/\*(.|\n)*?\*/", "", code)
    code = re.sub(r"//.*", "", code)
    return code


# Generator for the definitions in the header of the given Path.
def definitions(path: Path):

    with path.open() as file:
        end = True
        buffer = ""
        for line in file.readlines():
            buffer += line

            if line.endswith("\\\n"):
                buffer = buffer[:-2]
                continue

            # macro definitions
            match = re.search(r"#define\s+(\w+).*[ \t]+\S+[^\n]*\n", buffer)
            if match:
                text = remove_comments(match[0])
                yield (match[1], text)
                buffer = ""
                continue
            buffer = ""

    txt = path.read_text()
    # macro definitions
    for match in re.findall(r"^\s*(enum\s+(\w+)([^;]|\n)*;)", txt, flags=re.MULTILINE):
        text = remove_comments(match[0]) + "\n"
        yield (match[1], text)
    # type definitions
    for match in re.findall(r"(typedef\s+.+\s+\*?\s*(\w+)\s*;)", txt):
        text = remove_comments(match[0]) + "\n"
        yield (match[1], text)


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print(
            "Header generator usage:\n\tgen_header.py <solver> [search_location...]",
            file=sys.stderr,
        )
        exit(1)
    solver = sys.argv[1].upper()
    root = Path(os.path.realpath(__file__)).parent.parent.parent

    # Set addition search paths provided from command line
    search_path = []
    for i in range(2, len(sys.argv)):
        search_path.append(Path(sys.argv[i]).resolve())
        search_path.append(Path(sys.argv[i]).resolve() / "include")

    # Set all solver specific settings
    output = root / "include/minizinc/_thirdparty"
    prepend = ""
    ignore = set()  # identifiers to be ignore (e.g., platform dependent definitions)
    include = (
        set()
    )  # identifiers that must be included (e.g., identifiers used by included macros)

    if solver == "CPLEX":
        output /= "cplex_interface.h"
        source_names = ["ilcplex/cpxconst.h"]
        prepend = """#ifdef _MSC_VER
   typedef __int64 CPXLONG;
#else
   typedef long long CPXLONG;
#endif
#ifdef _WIN32
#define CPXPUBLIC      __stdcall
#else
#define CPXPUBLIC
#endif
"""
        ignore = {"CPXLONG", "CPXPUBLIC"}
        usage = [
            root / "solvers/MIP/MIP_cplex_solverfactory.cpp",
            root / "solvers/MIP/MIP_cplex_wrap.cpp",
            root / "include/minizinc/solvers/MIP/MIP_cplex_solverfactory.hh",
            root / "include/minizinc/solvers/MIP/MIP_cplex_wrap.hh",
        ]

        for ver in [
            "1210",
            "129",
            "128",
            "1271",
            "127",
            "1263",
            "1262",
            "1261",
            "126",
            "201",
        ]:
            paths = [
                f"/opt/ibm/ILOG/CPLEX_Studio{ver}",
                f"/opt/IBM/ILOG/CPLEX_Studio{ver}",
                f"C:\\Program Files\\IBM\\ILOG\\CPLEX_Studio{ver}",
                f"C:\\Program Files (x86)\\IBM\\ILOG\\CPLEX_Studio{ver}",
                f"~/Applications/IBM/ILOG/CPLEX_Studio{ver}",
                f"/Applications/IBM/ILOG/CPLEX_Studio{ver}",
                f"/Applications/CPLEX_Studio{ver}",
            ]
            search_path.extend([Path(p) / "include" for p in paths if Path(p).exists()])
            search_path.extend(
                [Path(p) / "cplex/include" for p in paths if Path(p).exists()]
            )

    elif solver == "GUROBI":
        output /= "gurobi_interface.h"
        source_names = ["gurobi_c.h"]
        prepend = STDCALL_PREPEND
        usage = [
            root / "solvers/MIP/MIP_gurobi_solverfactory.cpp",
            root / "solvers/MIP/MIP_gurobi_wrap.cpp",
            root / "include/minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh",
            root / "include/minizinc/solvers/MIP/MIP_gurobi_wrap.hh",
        ]
        for ver in [
            "913",
            "912",
            "911",
            "910",
            "903",
            "902",
            "901",
            "900",
            "811",
            "810",
            "801",
            "752",
            "702",
        ]:
            paths = [
                f"/opt/gurobi{ver}/linux64",
                f"C:\\gurobi{ver}\\win64",
                f"C:\\gurobi{ver}\\win32",
                f"/Library/gurobi{ver}/mac64",
                f"/Library/gurobi{ver}/macos_universal2",
            ]
            search_path.extend([Path(p) / "include" for p in paths if Path(p).exists()])
    elif solver == "SCIP":
        output /= "scip_interface.h"
        source_names = [
            "scip/def.h",
            "scip/type_scip.h",
            "scip/type_retcode.h",
            "scip/type_message.h",
            "scip/type_prob.h",
            "scip/type_var.h",
            "scip/type_stat.h",
            "scip/type_sol.h",
            "scip/type_event.h",
            "scip/type_cons.h",
            "scip/type_paramset.h",
            "scip/type_lp.h",
            "scip/cons_orbitope.h",
        ]
        include = {
            "SCIP_Retcode",
            "SCIP_Vartype",
            "SCIP_Status",
            "SCIP_Objsense",
            "SCIP_BoundType",
            "SCIP_OrbitopeType",
        }
        prepend = """#if !defined(_MSC_VER) || _MSC_VER > 1600
#ifdef __cplusplus
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#else
#define PRIx64 "llx"
#endif
#ifdef SCIP_DEBUG
#define SCIPdebugMessage                printf("[%s:%d] debug: ", __FILE__, __LINE__), printf
#else
#define SCIPdebugMessage                while( FALSE ) printf
#endif
"""
        usage = [
            root / "solvers/MIP/MIP_scip_solverfactory.cpp",
            root / "solvers/MIP/MIP_scip_wrap.cpp",
            root / "include/minizinc/solvers/MIP/MIP_scip_solverfactory.hh",
            root / "include/minizinc/solvers/MIP/MIP_scip_wrap.hh",
        ]
    elif solver == "XPRESS":
        output /= "xpress_interface.h"
        source_names = ["xprb.h", "xprs.h"]
        prepend = """#ifdef _WIN32
#define XPRS_CC __stdcall
#else
#define XPRS_CC
#endif
#if defined(_WIN32) || defined(_WIN64)
#define XB_CC __stdcall
#else
#define XB_CC
#endif
#if defined(_WIN32)
#define XPRSint64 __int64
#elif defined(__LP64__) || defined(_LP64) || defined(__ILP64__) || defined(_ILP64)
#define XPRSint64 long
#else
#define XPRSint64 long long
#endif
"""
        ignore = {"XB_CC", "XPRS_CC", "XPRSint64"}
        include = {
            "XB_INFINITY",
            "XB_PL",
            "XB_BV",
            "XB_UI",
            "XB_L",
            "XB_G",
            "XB_E",
            "XB_MAXIM",
            "XB_MINIM",
            "XB_LP",
            "XB_MPS",
            "XB_XO_SOL",
            "XB_MIP_NOT_LOADED",
            "XB_MIP_NO_SOL_FOUND",
            "XB_MIP_INFEAS",
            "XB_MIP_OPTIMAL",
            "XB_MIP_UNBOUNDED",
        }
        usage = [
            root / "solvers/MIP/MIP_xpress_solverfactory.cpp",
            root / "solvers/MIP/MIP_xpress_wrap.cpp",
            root / "include/minizinc/solvers/MIP/MIP_xpress_solverfactory.hh",
            root / "include/minizinc/solvers/MIP/MIP_xpress_wrap.hh",
        ]
        search_path.extend(
            [
                "/opt/xpressmp/include",
                "C:\\xpressmp\\Applications\\FICO Xpress\\xpressmp\\include",
            ]
        )
    else:
        print(
            f"ERROR: solver `{sys.argv[1]}' unknown. Unable to generate headers",
            file=sys.stderr,
        )
        exit(1)

    # Find Source Files
    source_files = []
    for name in source_names:
        f = find_file(name, search_path)
        if f is None:
            print(
                f"ERROR: Unable to locate source file `{name}'. Did you forget to specify a search location?",
                file=sys.stderr,
            )
            exit(1)
        source_files.append(f)

    # Filter definitions on usage and place in generated header file
    combined_usage = "\n".join([f.read_text() for f in usage])
    with output.open("w") as out:
        out.write(prepend)
        for (ident, define) in chain(*[definitions(f) for f in source_files]):
            if ident in include or (ident not in ignore and ident in combined_usage):
                out.write(define)
