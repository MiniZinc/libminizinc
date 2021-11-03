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
#if defined(_WIN32)
#define XPRSint64 __int64
#elif defined(__LP64__) || defined(_LP64) || defined(__ILP64__) || defined(_ILP64)
#define XPRSint64 long
#else
#define XPRSint64 long long
#endif
#define XB_INFINITY 1.0e+20
#define XB_PL  0
#define XB_BV  1
#define XB_UI  2
#define XB_N   0
#define XB_L   1
#define XB_G   2
#define XB_E   3
#define XB_MAXIM 1
#define XB_MINIM 0
#define XB_LP   1
#define XB_MPS  2
#define XB_XO_SOL    1
#define XB_MIP_NOT_LOADED     0
#define XB_MIP_NO_SOL_FOUND   3
#define XB_MIP_INFEAS         5
#define XB_MIP_OPTIMAL        6
#define XB_MIP_UNBOUNDED      7
#define XPRB_INFINITY XB_INFINITY
#define XPRB_PL XB_PL
#define XPRB_BV XB_BV
#define XPRB_UI XB_UI
#define XPRB_L XB_L
#define XPRB_G XB_G
#define XPRB_E XB_E
#define XPRB_MAXIM XB_MAXIM
#define XPRB_MINIM XB_MINIM
#define XPRB_LP  XB_LP
#define XPRB_MPS XB_MPS
#define XPRB_XPRS_SOL    XB_XO_SOL
#define XPRB_MIP_NOT_LOADED XB_MIP_NOT_LOADED
#define XPRB_MIP_NO_SOL_FOUND XB_MIP_NO_SOL_FOUND
#define XPRB_MIP_INFEAS XB_MIP_INFEAS
#define XPRB_MIP_OPTIMAL XB_MIP_OPTIMAL
#define XPRB_MIP_UNBOUNDED XB_MIP_UNBOUNDED
typedef struct Xbprob *XPRBprob;
typedef struct Xbvar *XPRBvar;
typedef struct Xbctr *XPRBctr;
typedef struct Xbsol *XPRBsol;
#define XPRS_MIPABSSTOP                                              7019
#define XPRS_MIPRELSTOP                                              7020
#define XPRS_MAXTIME                                                 8020
#define XPRS_MAXMIPSOL                                               8021
#define XPRS_THREADS                                                 8278
#define XPRS_RANDOMSEED                                              8328
#define XPRS_MIPOBJVAL                                               2003
#define XPRS_BESTBOUND                                               2004
#define XPRS_MIPSTATUS                                               1011
#define XPRS_NODES                                                   1013
#define XPRS_ACTIVENODES                                             1015
#define XPRS_TYPE_INT                      1
#define XPRS_TYPE_INT64                    2
#define XPRS_TYPE_DOUBLE                   3
#define XPRS_TYPE_STRING                   4
typedef struct xo_prob_struct* XPRSprob;
