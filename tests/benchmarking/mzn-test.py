#!/usr/bin/python3

##  Author: Gleb Belov@monash.edu  2017
##  This program runs MiniZinc solvers over a set of instances,
##  checks solutions and compares to given solution logs.

##  TODO type errors etc. when checking?
##  TODO -a for optimization only
##  TODO continuous output dumping as option
##  TODO CPU/user time limit, proper memory limit (setrlimit not working)

import sys, io, re as regex, traceback
import os.path, platform
##import numpy
import math
import json, argparse
import datetime
from collections import OrderedDict

import utils, json_config, json_log, mzn_exec, cmp_result_logs
from json_config import s_CommentKey, s_AddKey

s_ProgramDescr = 'MiniZinc testing automation. (c) 2017 Monash University, gleb.belov@monash.edu'
s_ProgramDescrLong = ( "Allows checking of MiniZinc solutions by configurable checker profiles. The solutions can be input or produced by a chosen solver profile, for 1 or several instances, with result comparison and (TODO) ranking.")

###########################   GENERAL CONFIG. Could be in the JSON config actually    #########################
sDirResults = "mzn-test"
sFlnSolUnchk = sDirResults + "/sol__unchk.json"      ### Logfile to append immediate results
sFlnSolCheckBase = sDirResults + "/LOGS/sol__{}.json"        ### Logfile to append checked results. {} replaced by datetime
sFlnSolFailBase = sDirResults + "/LOGS/sol__{}__FAIL.json"      ### To save failed solutions
sFlnSolLastDzn = sDirResults + "/sol_last.dzn"     ### File to save the DZN solution for checking
sFlnStdoutBase = sDirResults + "/OUTPUTS/{}.stdout.txt"      ### File name base to dump stdout for any backend call
sFlnStderrBase = sDirResults + "/OUTPUTS/{}.stderr.txt"
sFlnStatLog = sDirResults + "/STATS/stat_log__{}.txt"              ### The instance list name(s) will be inserted, if any

sDZNOutputAgrs = "--output-mode dzn --output-objective"    ## The flattener arguments to produce DZN-compatible output facilitating solution checking
sFlatOptChecker = "--allow-multiple-assignments"

s_UsageExamples = ( 
    "\nUSAGE EXAMPLES:"
    "\n(1)  \"mzn-test.py model.mzn data.dzn [--checkDZN stdout.txt [--checkStderr stderr.txt]] [--chkPrf MINIZINC-CHK --chkPrf FZN-GECODE-CHK] [--tCheck 15]\"                  ::: check the instance's solutions, optionally reading them from a DZN-formatted file (otherwise solving first), optionally overriding default checker list etc."
    "\n(2)  \"mzn-test.py --slvPrf MZN-CPLEX -t 300 -l instList1.txt -l instList2.txt --name ChuffedTest_003 --result newLog00.json prevLog1.json prevLog2.json --failed failLog.json\""
    "             ::: solve instances using the specified solver profile and wall time limit 300 seconds. The instances are taken from the list files. The test is aliased ChuffedTest_003. Results are saved to newLog00.json and compared/ranked to those in prevLog's. (Probably) incorrect solutions are saved to failLog.json."
    "\n(3)  \"mzn-test.py [-l instList1.txt] -c prevLog1.json -c prevLog2.json [--cmpOnly]\"                  ::: only compare existing logs, optionally limited to the given instances. USE SINGLE QUOTES ONLY INSIDE ARGUMENTS PASSED TO THE BACKENDS when running backends through shell."
  )
##############################################################################################
################ Parameters of MZN-Test, including config and command-line
##############################################################################################
class MZT_Param:
    def parseCmdLine(self):
        parser = argparse.ArgumentParser(
          description=s_ProgramDescr + '\n' + s_ProgramDescrLong,
          epilog=s_UsageExamples)
        parser.add_argument('instanceFiles', nargs='*', metavar='<instanceFile>',
          help='model instance files, if no instance lists supplied, otherwise existing solution logs to compare with')
        parser.add_argument('--checkDZN', '--checkStdout', metavar='<stdout_file>',
                            help='check DZN-formatted solutions from a solver\'s <stdout_file>. The DZN format is produced, e.g., if the model is flattened with \'' + sDZNOutputAgrs + '\'')
        parser.add_argument('--checkStderr', metavar='<stderr_file>',
                            help='for checking, read a solver\'s stderr log from <stderr_file> (not essential)')
        parser.add_argument('-l', '--instanceList', dest='l_InstLists', action='append', metavar='<instanceListFile>',
            help='file with a list of instance input files, one instance per line,'
              ' instance file types specified in config')
        parser.add_argument('--cmpOnly', '--compareOnly', action='store_true',
                            help='only compare JSON logs, optionally selecting the provided instance (lists)')
        parser.add_argument('--chkPrf', '--checkerPrf', '--checkerProfile', metavar='<prf_name>', action='append',
                            help='checker profile from those defined in config section \"CHECKER_PROFILES\", can be a few')
        parser.add_argument('--tCheck',
                            type=float,
                            metavar='<sec>', help='checker backend wall-time limit, default: '+
                              str(self.cfgDefault["BACKEND_DEFS"]["__BE_CHECKER"]["EXE"]["n_TimeoutRealHard"][0]))
        parser.add_argument('--slvPrf', '--solverPrf', '--solverProfile', metavar='<prf_name>',
                            help='solver profile from those defined in config section \"SOLVER_PROFILES\"')
        parser.add_argument('--solver', '--solverCall', metavar='"<exe+flags or shell command(s) if --shellSolve 1>"',
                            help='solver backend call, should be quoted. Insert %%s where instance files need to be. Flatten with \''
                             + sDZNOutputAgrs + '\' to enable solution checking, unless the model has a suitable output definition.'
                             " Pass '--output-time' to the output filter (e.g., solns2out) to enable ranking by time")
        parser.add_argument('-t', '--tSolve',
                            type=float,
                            metavar='<sec>', help='solver backend wall-time limit, default: '+
                              str(self.cfgDefault["BACKEND_DEFS"]["__BE_SOLVER"]["EXE"]["n_TimeoutRealHard"][0]))
        parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='tee backend\'s stderr to screen, in addition to the instance\'s logfile')
        parser.add_argument('--vc', '--verbose-check', dest='vc', action='store_true', help='same for checking')
     ## parser.add_argument('--no-feature', dest='feature', action='store_false')
     ## parser.set_defaults(feature=True)
        parser.add_argument('--print-call', dest='printcall', action='store_true', help='print final command line when running through os.system()')
        parser.add_argument('--shellSolve', type=int, metavar='0/1', help='backend call through shell when using psutils`')
        parser.add_argument('--psutils', type=int, metavar='0/1', help='backend call through psutils (seems buggy in 3.4.2)')
        parser.add_argument('--result', default=sFlnSolCheckBase, metavar='<file>',
                            help='save result log to <file>, default: \''+sFlnSolCheckBase+'\'')
        parser.add_argument('--name', '--testName', metavar='<string>', help='name of this test run, defaults to result log file name')
        parser.add_argument('--failed', ## default=self.cfgDefault["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["s_FailedSaveFile"][0],
                            metavar='<file>', help='save failed check reports to <file>, default: \''+
                              self.cfgDefault["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["s_FailedSaveFile"][0]+'\'')
        parser.add_argument('--nCheckMax', '--nCheck', '--nCheckedMax', ## default=self.cfgDefault["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["s_FailedSaveFile"][0],
                            type=int,
                            metavar='<N>', help='max number of solutions checked per instance.'
                              ' Negative means checking starts from the last obtained solution. Default: '+
                              str(self.cfgDefault["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["n_CheckedMax"][0]))
        parser.add_argument('--nFailedSaveMax', ## default=self.cfgDefault["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["s_FailedSaveFile"][0],
                            type=int,
                            metavar='<N>', help='max number of failed solution reports saved per instance, default: '+
                              str(self.cfgDefault["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["n_FailedSaveMax"][0]))
        parser.add_argument('--resultUnchk', default=sFlnSolUnchk, metavar='<file>', help='save unchecked result log to <file>')
        parser.add_argument('-c', '--compare', action="append", metavar='<file>',
                            help='compare results to existing <file>. This flag can be omitted if -l is used')
        
        parser.add_argument('--mergeCfg', action="append", metavar='<file>', help='merge config from <file>')
        parser.add_argument('--saveCfg', metavar='<file>', help='save internal config to <file>. Can be useful to modify some parameters and run with --mergeCfg')
        parser.add_argument('--saveSolverCfg', metavar='<file>', help='save the final solver backend config to <file>')
        parser.add_argument('--saveCheckerCfg', metavar='<file>', help='save the final checker backend config to <file>')
        parser.add_argument('--addOption', action="append", metavar='<text>', type=str, help='add <text> to the call')
        parser.add_argument('--useJoinedName', action="append", metavar='<...%s...>', type=str, help='add this to the call, with %%s being replaced by'
                            ' a joined filename from all the input filenames, e.g., "--writeModel MODELS/%%s.mps"')
        parser.add_argument('--debug', type=int, help='bit 1: print full solver call commands, bit 2: same for checker')
        self.args = parser.parse_args()
        # print( "ARGS:\n", self.args )
        ## Solver backend and checker backend list
        self.slvBE = None
        self.chkBEs = None

    ## Get parameters from config and command line, merge values
    def obtainParams(self):
        self.parseCmdLine()
        self.mergeValues()
        
    def initCfgDefault(self):
        ddd = {
          s_CommentKey: [
            "The default config structure for mzn-test.py. ", s_ProgramDescr,
            "You can export this by --saveCfg and modify -> --mergeCfg,",
            "even having only partial JSON subtree in a merged file(s).",
            "Structure: COMMON_OPTIONS, SOLVER_/CHECKER_PROFILES, BACKEND_DEFS.",
            "Solver and checkers are selected from pre-defined profiles,",
            "which are in turn built from sequences of 'basic backend definitions'.",
            "Comments are either separate keys or added as list elements ('/// ...')",
            "in pre-selected positions; then, overriding items should keep that order."
          ],
          "COMMON_OPTIONS": {
            s_CommentKey: [ "'Solvers' and 'Checkers' select the profiles to use for solving and checking.",
                            "At the moment only the 1st solver is used from Solvers."
                            "The selected profiles must be known in SOLVER_PROFILES / CHECKER_PROFILES, resp."
                          ],
            "Solvers": [ "MINIZINC",
                        #"MZN-CPLEX",
                        "/// At the moment only the 1st element is used for solving" ],
            "SOLUTION_CHECKING": {
              "Checkers": ["FZN-GECODE-CHK", "MINIZINC-CHK", "MZN-CPLEX-CHK" ],
              "n_CheckedMax": [ -10, "/// Negative value means it's that many last solutions" ],
              "n_FailedSaveMax": [ 3, "/// After that many failed solutions, stop checking the instance" ],
              "s_FailedSaveFile": [ sFlnSolFailBase, "/// Filename to save failed solutions" ],
            },
            "Instance_List": {
              s_CommentKey: [ "Params for instance lists.",
                              "Instance list is a file containing an instance's model files,",
                              "at most 1 instance per line.",
                              "InstanceFileExt: only files with this extensions from a list file",
                              "will be taken on each line" ],
              "InstanceFileExt": [".mzn", ".dzn"]     ##   Add json?   TODO
            },
            "runCommand": {
              "windows": {
                "runSilent": "echo \"WARNING. No timeout on Windows.\" & {2} 1>{3} 2>{4}",
                "runVerbose": "echo \"WARNING. No timeout on Windows.\" & {2} 3>&1 1>{3} 2>&3 | tee {4} & echo >>{4}"
              }
              ,"non-windows": {
                "runSilent": "ulimit -v {0}; timeout -k 1 {1} bash -c \"{2}\" 1>{3} 2>{4}",
                "runVerbose": "ulimit -v {0}; timeout -k 1 {1} bash -c \"{2}\" 3>&1 1>{3} 2>&3 | tee {4}; echo >>{4}"
              }
            }
          },
          "SOLVER_PROFILES": {
            s_CommentKey: [ "Similar to CHECKER_PROFILES." ],
            "MINIZINC": [ "__BE_COMMON", "__BE_SOLVER", "BE_MINIZINC" ],
            "FZN-GUROBI": [ "__BE_COMMON", "__BE_SOLVER", "BE_FZN-GUROBI" ],
            "FZN-CPLEX": [ "__BE_COMMON", "__BE_SOLVER", "BE_FZN-CPLEX" ],
            "FZN-CBC": [ "__BE_COMMON", "__BE_SOLVER", "BE_FZN-CBC" ],
            "MZN-GUROBI": [ "__BE_COMMON", "__BE_SOLVER", "BE_MZN-GUROBI" ],
            "MZN-CPLEX": [ "__BE_COMMON", "__BE_SOLVER", "BE_MZN-CPLEX" ],
            "MZN-CBC": [ "__BE_COMMON", "__BE_SOLVER", "BE_MZN-CBC" ],
            "MZN-GECODE": [ "__BE_COMMON", "__BE_SOLVER", "BE_MZN-GECODE" ],
            "FZN-CHUFFED": [ "__BE_COMMON", "__BE_SOLVER", "BE_FZN-CHUFFED" ]
          },
          "CHECKER_PROFILES": {
            s_CommentKey: [ "Each profile gives a list of backend defs to use.",
              "Later backends in the list can override/add options, ",
              "for example for values to be read from the outputs.",
              "Adding is only possible if the key is prefixed by '"+s_AddKey+"'"
            ],
            "MINIZINC-CHK": [ "__BE_COMMON", "__BE_CHECKER_OLDMINIZINC", "BE_MINIZINC" ],
            "MZN-GECODE-CHK": [ "__BE_COMMON", "__BE_CHECKER", "BE_MZN-GECODE" ],
            "MZN-GUROBI-CHK": [ "__BE_COMMON", "__BE_CHECKER", "BE_MZN-GUROBI" ],
            "MZN-CPLEX-CHK": [ "__BE_COMMON", "__BE_CHECKER", "BE_MZN-CPLEX" ],
            "FZN-GECODE-CHK": [ "__BE_COMMON", "__BE_CHECKER_OLDMINIZINC", "BE_FZN-GECODE" ],
            "FZN-GECODE-SHELL-CHK": [ "__BE_COMMON", "__BE_CHECKER_OLDMINIZINC", "BE_FZN-GECODE_SHELL" ],
            "FZN-CHUFFED-CHK": [ "__BE_COMMON", "__BE_CHECKER", "BE_FZN-CHUFFED" ]
          },
          "BACKEND_DEFS": {
            s_CommentKey: [ "__BE_COMMON initializes a basic backend structure.",
              "Each further backend in a profile list overrides or adds options."
            ],
            "__BE_COMMON": {
              s_CommentKey: [ "THE INITIALIZING BACKEND." ],
              "EXE": {
                s_CommentKey: [ "Solver call parameters" ],
                "s_SolverCall" : ["minizinc -v -s -a " + sDZNOutputAgrs + " %s",
                  "/// The 1st element defines the call line. %s is replaced by the instance filename(s)."],
                "s_ExtraCmdline" : ["", "/// Only for __BE_SOLVER/__BE_CHECKER... subprofiles."
                                        " The 1st element gives extra cmdline arguments to the call"],
                "b_ThruShell"  : [True, "/// Set True to call solver thru shell."
                  " Then you can do shell tricks but Ctrl+C may not kill all subprocesses etc."],
                "n_TimeoutRealHard": [500, "/// Real-time timeout per instance, seconds,"
                  " for all solution steps together. Use mzn/backend options for CPU time limit."],
                "n_VMEMLIMIT_SoftHard": [16000000, 16000000, "/// 2 limits, soft/hard, in KB. Platform-dependent in Python 3.6. Default 8GB = 8388608 KB"],
              },
              "Stderr_Keylines": {
                s_CommentKey: [ "A complete line in stderr will be interpreted accordingly.",
                  " Format: <outvar> : { <line>: <value>, ... }"
                  " You can add own things here (use '"+s_AddKey+"' before new var name)",
                  " which will be transferred into results" ],
                "Problem_Sense": {
                  "This is a maximization problem.": 1,
                  "This is a minimization problem.": -1,
                  "This is a satisfiability problem.": 0,
                }
              },
              "Stderr_Keyvalues": {
                s_CommentKey: [ "Numerical values to be extracted from a line in stderr.",
                  " { <outvar>: [ <regex search pattern>, <regex to replace by spaces>, <value's pos in the line>] }."
                ], 
                "Time_Flt": [ "Flattening done,", "[s]", 3, "/// E.g., 'Flattening done, 3s' produces 3."
                                " !!! This is interpreted as successful flattening by the checker" ]
              },
              "Stdout_Keylines": {
                s_CommentKey: [ "Similar to Stderr_Keylines"],
                "Sol_Status": {
                  "----------": 1,
                  "==========": 2,
                  "=====UNSATISFIABLE=====": -1,
                  "=====UNBOUNDED=====": -2,
                  "=====UNKNOWN=====": 0,
                  "=====UNSATorUNBOUNDED=====": -3,
                  "=====ERROR=====": -4
                }
              },
              "Stdout_Keyvalues": {
                s_CommentKey: ["Similar to Stderr_Keyvalues." ],
                "ObjVal_MZN":   [ "_objective", "[():=;%]", 2,
                                    "/// The objective value as evaluated by MZN." ],
                "ObjVal_Solver":   [ "% obj, bound, CPU_time, nodes", "[,:]", 7,
                                        "/// The objval as reported by solver."],
                "DualBnd_Solver":   [ "% obj, bound, CPU_time, nodes", "[,:]", 8 ],
                "CPUTime_Solver":   [ "% obj, bound, CPU_time, nodes", "[,:]", 9 ],
                "NNodes_Solver":   [ "% obj, bound, CPU_time, nodes", "[,:]", 10 ],
                "RealTime_Solns2Out": [ "% time elapsed:", " ", 4 ],
              }
            },
            "__BE_SOLVER": {
              s_CommentKey: ["Specializations for a general solver" ],
              "EXE": {
                "s_ExtraCmdline" : ["-a"],
                "b_ThruShell"  : [True],
                "n_TimeoutRealHard": [500],
                #  "n_VMEMLIMIT_SoftHard": [8000100000, 8100000000]
              }
            },
            "__BE_CHECKER": {
              s_CommentKey: ["Specializations for a general checker" ],
              "EXE": {
                "s_ExtraCmdline" : [sFlatOptChecker],
                "b_ThruShell"  : [True],
                "n_TimeoutRealHard": [15],
                #  "n_VMEMLIMIT_SoftHard": [8100000000, 8100000000]
              }
            },
            "__BE_CHECKER_OLDMINIZINC": {
              s_CommentKey: ["Specializations for a general checker using the 1.6 MiniZinc driver" ],
              "EXE": {
                "s_ExtraCmdline" : ["--mzn2fzn-cmd 'mzn2fzn -v -s --output-mode dzn " + sFlatOptChecker + "'"],
                "b_ThruShell"  : [True],
                "n_TimeoutRealHard": [15],
                #  "n_VMEMLIMIT_SoftHard": [8100000000, 8100000000]
              }
            },
            "BE_MINIZINC": {
              s_CommentKey: [ "------------------- Specializations for pure minizinc driver" ],
              "EXE":{
                "s_SolverCall": [ "minizinc --mzn2fzn-cmd 'mzn2fzn -v -s " + sDZNOutputAgrs + "' -s %s"], # _objective fails for checking
                "b_ThruShell"  : [True],
              },
            },
            "BE_FZN-GUROBI": {
              s_CommentKey: [ "------------------- Specializations for Gurobi solver instance" ],
              "EXE":{
                "s_SolverCall" : ["mzn2fzn -v -s -G linear " + sDZNOutputAgrs + " %s --fzn tmp.fzn --ozn tmp.ozn && mzn-gurobi -v -s tmp.fzn"],  ## works without solns2out for now. Need thus when using shell call with system()  TODO?
                #"opt_writeModel": ["--writeModel"]
              },
              "Stderr_Keyvalues": {
                s_AddKey+"Preslv_Rows": [ "Presolved:", " ", 2 ],
                s_AddKey+"Preslv_Cols": [ "Presolved:", " ", 4 ],
                s_AddKey+"Preslv_Non0": [ "Presolved:", " ", 6 ]
              },
            },
            "BE_MZN-GUROBI": {
              s_CommentKey: [ "------------------- Specializations for Gurobi solver instance" ],
              "EXE":{
                "s_SolverCall" : ["mzn-gurobi -v -s -G linear --output-time " + sDZNOutputAgrs + " %s"], # _objective fails for checking TODO
                #"opt_writeModel": ["--writeModel"]
              },
              "Stderr_Keyvalues": {
                s_AddKey+"Preslv_Rows": [ "Presolved:", " ", 2 ],
                s_AddKey+"Preslv_Cols": [ "Presolved:", " ", 4 ],
                s_AddKey+"Preslv_Non0": [ "Presolved:", " ", 6 ]
              },
            },
            "BE_FZN-CPLEX": {
              s_CommentKey: [ "------------------- Specializations for IBM ILOG CPLEX solver instance" ],
              "EXE": {
                "s_SolverCall" : ["mzn2fzn -v -s -G linear " + sDZNOutputAgrs + " %s --fzn tmp.fzn --ozn tmp.ozn && mzn-cplex -v -s tmp.fzn"]
                #"s_SolverCall" : ["./run-mzn-cplex.sh %s"],
                #"b_ThruShell"  : [True],
                #"opt_writeModel": ["--writeModel"]
              },
              "Stderr_Keyvalues": {
                s_AddKey+"Preslv_Rows": [ "Reduced MIP has [0-9]+ rows,", " ", 4 ],
                s_AddKey+"Preslv_Cols": [ "Reduced MIP has [0-9]+ rows,", " ", 6 ],
                s_AddKey+"Preslv_Non0": [ "Reduced MIP has [0-9]+ rows,", " ", 9 ]
              },
            },
            "BE_MZN-CPLEX": {
              s_CommentKey: [ "------------------- Specializations for IBM ILOG CPLEX solver instance" ],
              "EXE": {
                "s_SolverCall" : ["mzn-cplex -v -s -G linear --output-time " + sDZNOutputAgrs + " %s"], # _objective fails for checking
                #"s_SolverCall" : ["./run-mzn-cplex.sh %s"],
                #"b_ThruShell"  : [True],
                #"opt_writeModel": ["--writeModel"]
              },
              "Stderr_Keyvalues": {
                s_AddKey+"Preslv_Rows": [ "Reduced MIP has [0-9]+ rows,", " ", 4 ],
                s_AddKey+"Preslv_Cols": [ "Reduced MIP has [0-9]+ rows,", " ", 6 ],
                s_AddKey+"Preslv_Non0": [ "Reduced MIP has [0-9]+ rows,", " ", 9 ]
              },
            },
            "BE_FZN-CBC": {
              s_CommentKey: [ "------------------- Specializations for COIN-OR Branch&Cut solver instance" ],
              "EXE": {
                "s_SolverCall" : ["mzn-cbc -v -s -G linear --output-time " + sDZNOutputAgrs + " %s --fzn tmp.fzn --ozn tmp.ozn && mzn-cplex -v -s tmp.fzn"], 
                #"s_SolverCall" : ["./run-mzn-cplex.sh %s"],
                #"b_ThruShell"  : [True],
              },
            },
            "BE_MZN-CBC": {
              s_CommentKey: [ "------------------- Specializations for COIN-OR Branch&Cut solver instance" ],
              "EXE": {
                "s_SolverCall" : ["mzn-cbc -v -s -G linear --output-time " + sDZNOutputAgrs + " %s"], # _objective fails for checking
                #"s_SolverCall" : ["./run-mzn-cplex.sh %s"],
                #"b_ThruShell"  : [True],
              },
            },
            "BE_MZN-GECODE": {
              s_CommentKey: [ "------------------- Specializations for Gecode FlatZinc interpreter" ],
              "EXE": {
#                "s_SolverCall" : ["mzn-fzn -s -G gecode --solver fzn-gecode " + sDZNOutputAgrs + " %s"], # _objective fails for checking TODO
                "s_SolverCall" : ["mzn-gecode -v -s -G gecode " + sDZNOutputAgrs
                     + " %s"],    #  --time 300000
                "b_ThruShell"  : [True],
              }
            },
            "BE_FZN-GECODE": {
              s_CommentKey: [ "------------------- Specializations for Gecode FlatZinc interpreter" ],
              "EXE": {
#                "s_SolverCall" : ["mzn-fzn -s -G gecode --solver fzn-gecode " + sDZNOutputAgrs + " %s"], # _objective fails for checking TODO
                "s_SolverCall" : ["minizinc -s -G gecode -f fzn-gecode --mzn2fzn-cmd 'mzn2fzn -v -s " + sDZNOutputAgrs + "' %s"],
                "b_ThruShell"  : [True],
              }
            },
            "BE_FZN-GECODE_SHELL": {
              s_CommentKey: [ "------------------- Specializations for Gecode FlatZinc interpreter" ],
              "EXE": {
#                "s_SolverCall" : ["mzn-fzn -s -G gecode --solver fzn-gecode " + sDZNOutputAgrs + " %s"], # _objective fails for checking TODO
                "s_SolverCall" : ["mzn2fzn -v -s -G gecode " + sDZNOutputAgrs
                     + sFlatOptChecker + " %s --fzn tmp.fzn --ozn tmp.ozn && fzn-gecode tmp.fzn | solns2out tmp.ozn"],
                "b_ThruShell"  : [True],
                "s_ExtraCmdline" : [""],
              }
            },
            "BE_FZN-CHUFFED": {
              s_CommentKey: [ "------------------- Specializations for Chuffed FlatZinc interpreter" ],
              "EXE": {
                "s_SolverCall" : ["mzn-fzn -v -s -G chuffed --solver fzn-chuffed --fzn-flags -f --output-time "
                                    + sDZNOutputAgrs + " %s"], # _objective fails for checking
              }  ##  --fzn-flags --time-out --fzn-flags 300
            }
          }
        }
        return ddd
    ##        self.nNoOptAndAtLeast2Feas = 0
    
    ## Read a cfg file, instead of/in addition to the default cfg
    def mergeCfg(self, fln):
        ddd1 = None
        with open( fln, 'r' ) as rf:
            ddd1 = json.load( rf )
        self.cfg = mergeJSON( self.cfg, ddd1 )
      
    ## Merge cmdline values with cfg, performing some immediate actions
    ## And compile the backends constituting solver and checker(s)
    def mergeValues(self):
        if None!=self.args.mergeCfg:               ### MERGE CFG FROM EXTRA FILES
            for eCfg in self.args.mergeCfg:
                self.mergeCfg( eCfg )
        ################ Update some explicit cmdline params -- AFTER MERGING CFG FILES.
        if None!=self.args.failed:
            self.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["s_FailedSaveFile"][0] = self.args.failed
        if None!=self.args.nCheckMax:
            self.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["n_CheckedMax"][0] = self.args.nCheckMax
        if None!=self.args.nFailedSaveMax:
            self.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["n_FailedSaveMax"][0] = self.args.nFailedSaveMax
        ################ SAVE FINAL CFG
        if None!=self.args.saveCfg:
            with utils.openFile_autoDir( self.args.saveCfg, 'w' ) as wf:
                print( "Saving final config to", self.args.saveCfg )
                json.dump( self.cfg, wf, sort_keys=True, indent=json_config.n_JSON_Indent )
        ### COMPILE THE SOLVER BACKEND
        if None!=self.args.slvPrf:
            self.cfg["COMMON_OPTIONS"]["Solvers"][0] = self.args.slvPrf
        slvPrfName = self.cfg["COMMON_OPTIONS"]["Solvers"][0]
        slvPrf = self.cfg["SOLVER_PROFILES"][slvPrfName]
        assert len(slvPrf)>0, "Solver profile '%s' should use at least a basic backend" % slvPrfName
        self.slvBE = self.cfg["BACKEND_DEFS"][slvPrf[0]]
        for i in range( 1, len( slvPrf ) ):
            self.slvBE = json_config.mergeJSON( self.slvBE, self.cfg["BACKEND_DEFS"][slvPrf[i]] )
        if None!=self.args.tSolve:
            self.slvBE["EXE"]["n_TimeoutRealHard"][0] = self.args.tSolve
        assert None==self.args.solver or None==self.args.slvPrf, "ERROR: both solver call and a solver profile specified."
        if None!=self.args.solver:    ## After the compilation
            self.slvBE["EXE"]["s_SolverCall"][0] = self.args.solver
        if None!=self.args.shellSolve:
            self.slvBE["EXE"]["b_ThruShell"][0] = self.args.shellSolve!=0
        print ( "\nSolver/checker configurations:\n     SLV_CFG: ", json.dumps( self.slvBE["EXE"] ) )
        ### COMPILE THE CHECKER BACKENDS
        if None!=self.args.chkPrf and 0<len( self.args.chkPrf ):
            self.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["Checkers"] = self.args.chkPrf
        chkPrfList = self.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["Checkers"]
        self.chkBEs = []
        for chkPrfName in chkPrfList:
            chkPrf = self.cfg["CHECKER_PROFILES"][chkPrfName]
            assert len(chkPrf)>0, "Checker profile '%s' should use at least a basic backend" % chkPrfName
            self.chkBEs.append( self.cfg["BACKEND_DEFS"][chkPrf[0]] )
            for i in range( 1, len( chkPrf ) ):
                self.chkBEs[-1] = json_config.mergeJSON( self.chkBEs[-1], self.cfg["BACKEND_DEFS"][chkPrf[i]] )
            if None!=self.args.tCheck:
                self.chkBEs[-1]["EXE"]["n_TimeoutRealHard"][0] = self.args.tCheck
            print ( "     CHK_CFG: ", json.dumps( self.chkBEs[-1]["EXE"] ) )
        ### SAVE THE SOLVER BACKEND
        if None!=self.args.saveSolverCfg:
            with utils.openFile_autoDir( self.args.saveSolverCfg, 'w' ) as wf:
                print( "Saving solver config to", self.args.saveSolverCfg )
                json.dump( self.slvBE, wf, sort_keys=True, indent=json_config.n_JSON_Indent )
        ### SAVE THE CHECKER BACKENDS
        if None!=self.args.saveCheckerCfg:
            with utils.openFile_autoDir( self.args.saveCheckerCfg, 'w' ) as wf:
                print( "Saving checker config to", self.args.saveCheckerCfg )
                json.dump( self.chkBE, wf, sort_keys=True, indent=json_config.n_JSON_Indent )
        self.sThisName = self.args.result
        if None!=self.args.name:
            self.sThisName = self.args.name
    
    def __init__(self):
        self.cfgDefault = self.initCfgDefault()
        self.cfg = self.cfgDefault
        ### Further parameters
        self.args = {}
        
    def __str__(self):
        s_Out = json.dumps( self.cfg, sort_keys=True, indent=json_config.n_JSON_Indent ) + '\n'
        s_Out += str(self.args) + '\n'
        s_Out += "\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SOLVER BACKEND:\n"
        s_Out += json.dumps( self.slvBE, sort_keys=True, indent=json_config.n_JSON_Indent ) + '\n'
        s_Out += "\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> CHECKER BACKENDS:\n"
        for chkBE in self.chkBEs:
            s_Out += json.dumps( chkBE, sort_keys=True, indent=json_config.n_JSON_Indent ) + '\n'
        return s_Out
                

##############################################################################################
################### The MZNTest class
##############################################################################################
class MznTest:
    def obtainParams( self ):
        self.params.obtainParams()
        ## TRUNCATING files first, then "a" - better on Win??
        sNow = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        self.sStartTime = sNow
        self.fileSol00 = utils.openFile_autoDir( self.params.args.resultUnchk, "w" )
        self.fileSol = utils.openFile_autoDir(self.params.args.result.format(sNow + 
          ("" if self.params.args.name is None else "__"+utils.flnfy(self.params.args.name))), "w" )
        self.fileFailName = self.params.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["s_FailedSaveFile"][0].format(sNow)
        self.fileFail = None      ## Not opening yet

    ## If an instance was specified in cmdline, or model list(s) supplied
    def compileExplicitModelLists(self):
        ## Get cmdline filenames or from the --instList arguments
        ## Can compile the list from log files, see below
        self.params.instList = []
        ## If -l not used, take the pos args
        if not self.params.args.cmpOnly and 0<len( self.params.args.instanceFiles ) and \
          ( None==self.params.args.l_InstLists or 0==len( self.params.args.l_InstLists ) ):
            self.params.instList.append( " ".join( self.params.args.instanceFiles ) )     ## Also if l_InstLists?  TODO
        ## If -l used, compile the inst list files
        if None!=self.params.args.l_InstLists and 0<len( self.params.args.l_InstLists ):
            for sFln in self.params.args.l_InstLists:
                with open( sFln, 'r' ) as rf:
                    for line in rf:
                        s_ModelFiles = ""
                        for wrd in line.split():
                            fIsMF = False
                            for ext in self.params.cfg["COMMON_OPTIONS"]["Instance_List"]["InstanceFileExt"]:
                                if wrd.endswith( ext ):
                                    fIsMF = True
                                    break
                            if fIsMF:
                                s_ModelFiles += ' ' + wrd
                        if 0<len( s_ModelFiles ):
                          self.params.instList.append( s_ModelFiles )
        
    ## Result logs can be used to extract the instance list, if wished
    def compileResultLogs(self):
        self.cmpRes = cmp_result_logs.CompareLogs()
        cmpFileList = []
        if None!=self.params.args.compare:     ### If -c used
            cmpFileList += self.params.args.compare
        ## Mode "compare only" if explicit or no instances
        self.bCmpOnly = True if self.params.args.cmpOnly or \
              0==len( self.params.instList ) else False
        ### If -l used, interpret pos arguments as comparison logs
        if None!=self.params.args.l_InstLists and 0<len( self.params.args.l_InstLists ):
            cmpFileList += self.params.args.instanceFiles
        for sFlnLog in cmpFileList:
            nEntries = 0
            logCurrent, lLogNames = self.cmpRes.addLog( sFlnLog )
            print( "Reading result log '", sFlnLog, "'...  ", sep='', end='', flush=True )
            with open( sFlnLog, 'r' ) as rf:
                while True:
                    chJ = json_log.readLogJSONChunk( rf )
                    if None==chJ:
                        break
                    try:
                        logCurrent[
                          frozenset( chJ["Inst_Files"].split() ) ] = chJ
                        if "TEST_NAME" in chJ:
                            lLogNames[1] = chJ[ "TEST_NAME" ]
                            print( "  TEST NAME: '", chJ[ "TEST_NAME" ],
                                  "'... ", sep='', end='', flush=True )
                    except:
                        print( "\n   WARNING: unrecognized result chunk in file '", sFlnLog,
                              "' before position", rf.tell(), ", doesn't contain all keys")
                    else:
                        nEntries += 1
            print( len(logCurrent), "different instances among", nEntries, "recognized entries."  )
        
    def runTheInstances(self):
        logCurrent, lLogNames = self.cmpRes.addLog( self.params.args.result )
        if self.params.sThisName!=lLogNames[0]:
            lLogNames[1] = self.params.sThisName
        self.nCheckedInstances = 0
        self.nChecksFailed = 0
        self.cmpRes.initListComparison()
        for i_Inst in range( len(self.params.instList) ):
            s_Inst = self.params.instList[ i_Inst ]
            self.initInstance( i_Inst, s_Inst )
            self.solveOriginal( s_Inst )
            try:
                if self.ifShouldCheck():
                    self.checkOriginal( s_Inst )
                else:
                    print( "NO CHECK,  total check-failed instances:", self.nChecksFailed,
                           "from", self.nCheckedInstances )
            except:
                print( "  WARNING: failed to check instance solution. ", sys.exc_info() )
            ## Saving to the main log:
            self.saveSolution( self.fileSol )
            ## Ranking:
            sSet_Inst = frozenset( self.result["Inst_Files"].split() )
            logCurrent[ sSet_Inst ] = self.result
            try:
                self.cmpRes.compareInstance( sSet_Inst )
                if i_Inst < len(self.params.instList)-1:
                    print( "STATS:  ", end='' )
                    self.cmpRes.summarizeCurrent( lLogNames )
                    print( "" )
            except:
                print( "  WARNING: failed to compare/rank instance. ", sys.exc_info() )

    def compareLogs(self):
        self.cmpRes.initListComparison()
        theList = [ frozenset( lfn.split() ) for lfn in self.params.instList ]
        if 0==len( theList ):
            theList = self.cmpRes.getInstanceUnion()
        for sInst in theList:
            try:
                self.cmpRes.compareInstance( sInst )
            except:
                print( "  ------  WARNING: failed to compare/rank instance. ",  )
                traceback.print_exc()
        self.cmpRes.summarizeCmp()

    def summarize(self):
        try:
            ### Printing summary
            print ('')
            print ('='*50)
            print('            SUMMARY')
            print ('='*50)
            stats = self.cmpRes.summarize()
            print (stats)
            print( "Printing stats log to: ", end="" )
            sFlnSL = sFlnStatLog.format( utils.flnfy(
                " ".join(self.params.args.l_InstLists if self.params.args.l_InstLists is not None else []) ) )
            print( sFlnSL )
            with utils.openFile_autoDir( sFlnSL, "a" ) as wf:
                wf.write( "\nRUN  " + self.sStartTime + "--" + datetime.datetime.now().__str__() + ":  " )
                wf.write( sys.argv.__str__() )
                wf.write( "\n" )
                wf.write( stats )
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            print(exc_type, exc_obj, fname, exc_tb.tb_lineno)
        if not self.bCmpOnly:
            print( "\nResult logs saved to '",  self.params.args.result,
               "', with the unchecked log in '", self.params.args.resultUnchk, "'; failed solutions saved to '",
               self.params.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["s_FailedSaveFile"][0],
               "' in an \"appendable JSON\" format, cf. https://github.com/pvorb/jsml."
               "\nSolver/checker stdout(err) outputs saved to '" + sDirResults + "/last_stdout(err)_...txt'.", sep='')

##############################################################################################
######################### MZNTest average-level #############################

    ## init
    def initInstance( self, i_Inst, s_Inst ):
        print( "\n--------------------- INSTANCE %d of %d:  '" % (i_Inst+1, len( self.params.instList )),
              s_Inst, "' ----------------------", sep='' )
        self.result = OrderedDict()
        if 0==i_Inst and None!=self.params.args.name:
            self.result["TEST_NAME"] = self.params.args.name
        self.result[ "Inst_Index_Of" ] = [ (i_Inst+1), len( self.params.instList ) ]
        self.result["Inst_Files"] = s_Inst
        self.solList = []                   ## The solution list
        ## TODO: instance name source?

    ## Solve the original instance
    def solveOriginal( self, s_Inst ):
        self.result["__SOLVE__"] = self.solveInstance( s_Inst, self.params.slvBE, '_SOLVING', self.solList )
        # print( "   RESULT:\n", self.result )
        self.saveSolution( self.fileSol00 )
        
    ## Solve a given MZN instance and return the result map
    ## Arguments: instance files in a string, backend parameter dictionary
    ## slvName is used for screen output and last_std(out/err)_... naming, so better no spaces
    ## solList provided <=> this is solving (not checking) and will use --checkDZN if opted

    ##  TODO
    ## Because we cannot limit memory of the subprocesses directly AND there seem to be bugs in the Python 3.4 impl,
    ## could replace the subprocess call by a call to an external which would run under given memory/time limits
    ## and save output to given files.
    ## OR: update from Python 3.4.2?
    ## NAming output / model files: sort input filenames, replace spaces/punctuation
    ### USE smth like
    ## keepcharacters = (' ','.','_')
    ## "".join(c for c in filename if c.isalnum() or c in keepcharacters else 'I').strip()
    def solveInstance(self, s_Inst, slvBE, slvName, solList=None):
        resSlv = OrderedDict()
        bChkDZN = True if None!=solList and None!=self.params.args.checkDZN else False
        if bChkDZN:
#        print( "Running '", slvBE["EXE"]["s_SolverCall"][0] \
#          + ' ' + slvBE["EXE"]["s_ExtraCmdline"][0], "'... ", sep='', end='', flush=True )
            resSlv["Sol_Status"] = [-50, "   ????? NO STATUS LINE PARSED."]
            print( "_PARSING '", self.params.args.checkDZN, sep='', end="'... " )
            with open( self.params.args.checkDZN, 'r' ) as ro:
                mzn_exec.parseStdout( ro, resSlv, slvBE["Stdout_Keylines"], slvBE["Stdout_Keyvalues"], solList )
                print( ro.tell(), "bytes", end='' )
            if None!=self.params.args.checkStderr:
                print( "  and '", self.params.args.checkStderr, sep='', end="'... " )
                with open( self.params.args.checkStderr, 'r' ) as re:
                    mzn_exec.parseStderr( re, resSlv, slvBE["Stderr_Keylines"], slvBE["Stderr_Keyvalues"] )
                    print( re.tell(), "bytes", end='' )
        else:      #### Solving oneself
            print( slvName, "... ", sep='', end='', flush=True )
            s_Call = slvBE["EXE"]["s_SolverCall"][0] % s_Inst \
              + ' ' + slvBE["EXE"]["s_ExtraCmdline"][0]
            if solList is not None and self.params.args.addOption is not None:
                for sOpt in self.params.args.addOption:
                    s_Call += ' ' + sOpt
            s_InstMerged = s_Inst.strip()
            ## s_InstMerged = regex.sub( r"[.\\/:~]", "", s_InstMerged );
            ## s_InstMerged = regex.sub( r"[ ]", "-", s_InstMerged );
            keepcharacters = ('-','_')
            s_InstMerged = "".join(c if c.isalnum() or c in keepcharacters else 'I' for c in s_InstMerged).strip()
            
            if solList is not None and self.params.args.useJoinedName is not None:
                for sUseJN in self.params.args.useJoinedName:
                    s_UsingOpt = sUseJN % s_InstMerged
                    s_Call += ' ' + s_UsingOpt
            if solList is not None:      ## solving the original instance
                sFlnStdout = sFlnStdoutBase.format( s_InstMerged )
                sFlnStderr = sFlnStderrBase.format( s_InstMerged )
                if self.params.args.debug is not None and ( self.params.args.debug & 1 ):
                    print( "  CALL: \"", s_Call, "\"", sep='', flush=True )
            else:
                sFlnStdout = 'last_stdout' + slvName + '.txt'
                sFlnStderr = 'last_stderr' + slvName + '.txt'
                if self.params.args.debug is not None and ( self.params.args.debug & 2 ):
                    print( "  CALL: \"", s_Call, "\"", sep='', flush=True )
            resSlv["Solver_Call"] = s_Call
            resSlv["DateTime_Start"] = datetime.datetime.now().__str__()
            if 1==self.params.args.psutils:
                completed, tmAll = \
                    mzn_exec.runCmd(
                      s_Call,
                      slvBE["EXE"]["b_ThruShell"][0],
                      slvBE["EXE"]["n_TimeoutRealHard"][0],
                      slvBE["EXE"]["n_VMEMLIMIT_SoftHard"]
                    )
                with utils.openFile_autoDir( sFlnStdout, "w" ) as tf:
                    tf.write( completed.stdout )
                with utils.openFile_autoDir( sFlnStderr, "w" ) as tf:
                    tf.write( completed.stderr )
                print( "STDOUT/ERR: ", len(completed.stdout), '/',
                      len(completed.stderr), " bytes", sep='', end=', ' )
                mzn_exec.parseStderr( io.StringIO( completed.stderr ), resSlv, slvBE["Stderr_Keylines"], slvBE["Stderr_Keyvalues"] )
                if "Time_Flt" in resSlv:
                    resSlv["Sol_Status"] = [-50, "   ????? NO STATUS LINE PARSED."]
                else:
                    resSlv["Sol_Status"] = [-51, "   !!!!! NOFZN"]     ## This can mean a check failed.
                mzn_exec.parseStdout( io.StringIO( completed.stdout ), resSlv, slvBE["Stdout_Keylines"], slvBE["Stdout_Keyvalues"], solList )
                ## Adding the outputs to the log
                ## resSlv["StdErr"] = completed.stderr
                ## resSlv["StdOut"] = completed.stdout
            else:          ## use the 'system' call
                with utils.openFile_autoDir( sFlnStdout, "w" ) as tf:
                    tf.write( "% EMPTY\n" )
                with utils.openFile_autoDir( sFlnStderr, "w" ) as tf:
                    tf.write( "% EMPTY" )
                tmAll = mzn_exec.runCmdCmdline(
                      s_Call,
                      sFlnStdout, sFlnStderr,
                      self.params.cfg["COMMON_OPTIONS"]["runCommand"],
                      slvBE["EXE"]["n_TimeoutRealHard"][0],
                      (self.params.args.verbose) if solList is not None else (self.params.args.vc),
                      slvBE["EXE"]["n_VMEMLIMIT_SoftHard"]
                )
                with open( sFlnStderr, "r" ) as rf:
                    mzn_exec.parseStderr( rf, resSlv, slvBE["Stderr_Keylines"], slvBE["Stderr_Keyvalues"] )
                if "Time_Flt" in resSlv:
                    resSlv["Sol_Status"] = [-50, "   ????? NO STATUS LINE PARSED."]
                else:
                    resSlv["Sol_Status"] = [-51, "   !!!!! NOFZN"]     ## This can mean a check failed.
                with open( sFlnStdout, "r" ) as rf:
                    mzn_exec.parseStdout( rf, resSlv, slvBE["Stdout_Keylines"], slvBE["Stdout_Keyvalues"], solList )

            print( " t: {:.3f}".format( tmAll ), end=' s, ' )
            resSlv["DateTime_Finish"] = datetime.datetime.now().__str__()
            resSlv["TimeReal_All"] = tmAll
            resSlv["TimeReal_LastStatus"] = 0
            resSlv["Hostname"] = platform.uname()[1]
        dTmLast = utils.try_float( resSlv.get( "RealTime_Solns2Out" ) )
        if None!=dTmLast:
            resSlv["TimeReal_LastStatus"] = dTmLast / 1000.0
            resSlv.pop( "RealTime_Solns2Out" )
        ## if "SolutionLast" in resSlv:
        ##     print( "   SOLUTION_LAST:\n", resSlv["SolutionLast"], sep='' )
        if None!=solList:
            print( "  Nsol:", len(solList), end=',' )
        print( "   STATUS:", resSlv["Sol_Status"] )
        return resSlv
      
    ## Append the unchecked solution of the instance to a temp. file
    def saveSolution(self, wf):
        json_log.writeLogChunk( wf, json.dumps( self.result, indent=json_config.n_JSON_Indent ) )
    
    ## Return the necessity to check solutions.
    ## Can be false if we only want to compare different solvers.
    def ifShouldCheck(self):
        return \
          0<self.params.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["n_FailedSaveMax"][0] and \
          0!=self.params.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["n_CheckedMax"][0] and \
          0<len( self.params.chkBEs ) and \
          self.ifCheckableStatus( self.result["__SOLVE__"]["Sol_Status"][0] )
    
    ##
    def ifCheckableStatus( self, status ):
        return status>0
    
    ## Check selected solutions of the instance
    def checkOriginal( self, s_Inst ):
        nCM = self.params.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["n_CheckedMax"][0]
        nFSM = self.params.cfg["COMMON_OPTIONS"]["SOLUTION_CHECKING"]["n_FailedSaveMax"][0]
        assert 0!=nCM
        assert 0<len(self.solList)
        rng = None
        if 0<nCM:
            rng = range( 0, min( nCM, len(self.solList ) ) )
        else:
            rng = range( -1, max( nCM-1, -len(self.solList)-1 ), -1 )
        self.result["SOLUTION_CHECKS_DONE"] = 0
        self.result["SOLUTION_CHECKS_FAILED"] = 0
        fFailed = 0
        ## Iterate over the solution range
        for iSol in rng:
            ## Try modify
            # self.solList[ iSol ] = self.solList[iSol][:20] + '1' + self.solList[iSol][21:]
            print ( "CHK SOL ", iSol if iSol<0 else (iSol+1), '/', len(rng), sep='', end='... ' )
            with utils.openFile_autoDir( sFlnSolLastDzn, "w" ) as wf:          ## Save the selected solution
                wf.write( self.solList[ iSol ] )
            s_IC = s_Inst + ' ' + sFlnSolLastDzn
            bCheckOK = True
            chkFlSt = []
            # self.result["__CHECKS__"] = []
            for iChk in range ( len ( self.params.chkBEs ) ):
                chkBE = self.params.chkBEs[ iChk ]
                chkRes = self.solveInstance( s_IC, chkBE, '__Checker_'+str(iChk+1) )
                # self.result["__CHECKS__"].append( chkRes )
                if ( ## -51==chkRes["Sol_Status"][0] or               ## NOFZN? No, flattener should report INFEAS.
                    ( 0>chkRes["Sol_Status"][0] and -3<=chkRes["Sol_Status"][0] ) ):   ## INFEAS
                    bCheckOK = False
                    chkFlSt = chkRes["Sol_Status"]
            self.result["SOLUTION_CHECKS_DONE"] += 1
            if not bCheckOK:
                fFailed = 1
                self.result["SOLUTION_CHECKS_FAILED"] += 1
                self.result["SOLUTION_FAILED_LAST"] = self.solList[ iSol ]
                # self.result["SOLUTION_FAILED_LAST__CHKSTATUS"] = chkRes["Sol_Status"]
                if self.fileFail is None:
                    self.fileFail = utils.openFile_autoDir( self.fileFailName, "w" )
                self.saveSolution( self.fileFail )
                if nFSM<=self.result["SOLUTION_CHECKS_FAILED"]:
                    print ( self.result["SOLUTION_CHECKS_FAILED"], "failed solution(s) saved, go on" )
                    break
              
        self.nCheckedInstances += 1
        self.nChecksFailed += fFailed
        print( "   CHECK FAILS on this instance: ", self.result["SOLUTION_CHECKS_FAILED"],
                ",  total check-failed instances: ", self.nChecksFailed,
                " from ", self.nCheckedInstances, sep='' )
            
    def __init__(self):
        ## Default params
        self.params = MZT_Param()

##############################################################################################
######################### MZNTest public #############################
##############################################################################################
    def run(self):
        self.obtainParams()
        self.compileExplicitModelLists()
        self.compileResultLogs()
        if not self.bCmpOnly:
            self.runTheInstances()
        else:
            self.compareLogs()
        self.summarize()
        
#  def main
mznTst = MznTest()
mznTst.run()

