#if !defined(WIN32) && !defined(WIN64)
#define __cdecl
#define __stdcall
#endif
#define GRB_LESS_EQUAL    '<'
#define GRB_GREATER_EQUAL '>'
#define GRB_EQUAL         '='
#define GRB_CONTINUOUS 'C'
#define GRB_BINARY     'B'
#define GRB_INTEGER    'I'
#define GRB_MINIMIZE 1
#define GRB_MAXIMIZE -1
#define GRB_INFINITY  1e100
#define GRB_MAX_STRLEN     512
#define CB_ARGS GRBmodel *model, void *cbdata, int where, void *usrdata
#define GRB_INT_ATTR_NUMCONSTRS    "NumConstrs"    
#define GRB_INT_ATTR_NUMVARS       "NumVars"       
#define GRB_INT_ATTR_MODELSENSE    "ModelSense"    
#define GRB_DBL_ATTR_LB             "LB"              
#define GRB_DBL_ATTR_UB             "UB"              
#define GRB_DBL_ATTR_OBJ            "Obj"             
#define GRB_INT_ATTR_STATUS        "Status"      
#define GRB_DBL_ATTR_OBJVAL        "ObjVal"      
#define GRB_DBL_ATTR_OBJBOUND      "ObjBound"    
#define GRB_DBL_ATTR_OBJBOUNDC     "ObjBoundC"   
#define GRB_DBL_ATTR_NODECOUNT     "NodeCount"    
#define GRB_DBL_ATTR_X         "X"         
#define GRB_INT_ATTR_NUMOBJ       "NumObj"       
#define GRB_CB_MIP       3
#define GRB_CB_MIPSOL    4
#define GRB_CB_MIPNODE   5
#define GRB_CB_MESSAGE   6
#define GRB_CB_MIP_OBJBND  3001
#define GRB_CB_MIP_NODLFT  3005
#define GRB_CB_MIPSOL_SOL     4001
#define GRB_CB_MIPSOL_OBJ     4002
#define GRB_CB_MIPSOL_NODCNT  4005
#define GRB_CB_MIPSOL_SOLCNT  4006
#define GRB_CB_MIPNODE_STATUS  5001
#define GRB_CB_MIPNODE_REL     5002
#define GRB_CB_MSG_STRING  6001
#define GRB_CB_RUNTIME     6002
#define GRB_OPTIMAL         2
#define GRB_INFEASIBLE      3
#define GRB_INF_OR_UNBD     4
#define GRB_UNBOUNDED       5
#define GRB_INT_PAR_SOLUTIONLIMIT  "SolutionLimit"
#define GRB_DBL_PAR_TIMELIMIT      "TimeLimit"
#define GRB_DBL_PAR_FEASIBILITYTOL "FeasibilityTol"
#define GRB_DBL_PAR_INTFEASTOL     "IntFeasTol"
#define GRB_DBL_PAR_MIPGAP         "MIPGap"
#define GRB_DBL_PAR_MIPGAPABS      "MIPGapAbs"
#define GRB_INT_PAR_MIPFOCUS          "MIPFocus"
#define GRB_STR_PAR_NODEFILEDIR       "NodefileDir"
#define GRB_DBL_PAR_NODEFILESTART     "NodefileStart"
#define GRB_INT_PAR_LAZYCONSTRAINTS   "LazyConstraints"
#define GRB_INT_PAR_NONCONVEX         "NonConvex"
#define GRB_INT_PAR_PRECRUSH          "PreCrush"
#define GRB_INT_PAR_SEED              "Seed"
#define GRB_INT_PAR_THREADS           "Threads"
#define GRB_STR_PAR_DUMMY             "Dummy"
typedef struct _GRBmodel GRBmodel;
typedef struct _GRBenv GRBenv;
