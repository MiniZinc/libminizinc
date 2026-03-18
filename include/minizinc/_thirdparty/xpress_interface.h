#ifdef _WIN32
#define XPRS_CC __stdcall
#else
#define XPRS_CC
#endif
#if defined(_WIN32) || defined(_WIN64)
#define XB_CC __stdcall
#else
#define XB_CC
#endif
#ifdef _WIN32
#define XPRSint64 __int64
#elif defined(__LP64__) || defined(_LP64) || defined(__ILP64__) || defined(_ILP64)
#define XPRSint64 long
#else
#define XPRSint64 long long
#endif
// XPRS control parameters
#define XPRS_MIPABSSTOP                                              7019
#define XPRS_MIPRELSTOP                                              7020
#define XPRS_TIMELIMIT                                               7158
#define XPRS_MAXMIPSOL                                               8021
#define XPRS_OUTPUTLOG                                               8035
#define XPRS_THREADS                                                 8278
#define XPRS_RANDOMSEED                                              8328
#define XPRS_OBJVAL                                                  2118
#define XPRS_BESTBOUND                                               2004
#define XPRS_SOLSTATUS                                               1053
#define XPRS_NODES                                                   1013
#define XPRS_ACTIVENODES                                             1015

// XPRS_SOLSTATUS values
#define XPRS_SOLSTATUS_NOTFOUND                                      0
#define XPRS_SOLSTATUS_OPTIMAL                                       1
#define XPRS_SOLSTATUS_FEASIBLE                                      2
#define XPRS_SOLSTATUS_INFEASIBLE                                    3
#define XPRS_SOLSTATUS_UNBOUNDED                                     4
#define XPRS_TYPE_INT                      1
#define XPRS_TYPE_INT64                    2
#define XPRS_TYPE_DOUBLE                   3
#define XPRS_TYPE_STRING                   4

// XPRS problem handle
typedef struct xo_prob_struct* XPRSprob;

// XPRS constants
#define XPRS_PLUSINFINITY              1.0e+20
#define XPRS_OBJ_MINIMIZE                 1
#define XPRS_OBJ_MAXIMIZE                -1

