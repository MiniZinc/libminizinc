#if !defined(_MSC_VER) || _MSC_VER > 1600
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
#define SCIP_Bool unsigned int               
#define TRUE  1                              
#define FALSE 0                              
#define SCIP_Longint long long                         
#define SCIP_Real double                               
#define MAX(x,y)      ((x) >= (y) ? (x) : (y))     
#define MIN(x,y)      ((x) <= (y) ? (x) : (y))     
typedef struct Scip SCIP;
enum SCIP_Retcode
{
   SCIP_OKAY               =  +1,       
   SCIP_ERROR              =   0,       
   SCIP_NOMEMORY           =  -1,       
   SCIP_READERROR          =  -2,       
   SCIP_WRITEERROR         =  -3,       
   SCIP_NOFILE             =  -4,       
   SCIP_FILECREATEERROR    =  -5,       
   SCIP_LPERROR            =  -6,       
   SCIP_NOPROBLEM          =  -7,       
   SCIP_INVALIDCALL        =  -8,       
   SCIP_INVALIDDATA        =  -9,       
   SCIP_INVALIDRESULT      = -10,       
   SCIP_PLUGINNOTFOUND     = -11,       
   SCIP_PARAMETERUNKNOWN   = -12,       
   SCIP_PARAMETERWRONGTYPE = -13,       
   SCIP_PARAMETERWRONGVAL  = -14,       
   SCIP_KEYALREADYEXISTING = -15,       
   SCIP_MAXDEPTHLEVEL      = -16,       
   SCIP_BRANCHERROR        = -17        
};
typedef enum SCIP_Retcode SCIP_RETCODE;
#define SCIP_DECL_MESSAGEWARNING(x) void x (SCIP_MESSAGEHDLR* messagehdlr, FILE* file, const char* msg)
#define SCIP_DECL_MESSAGEDIALOG(x) void x (SCIP_MESSAGEHDLR* messagehdlr, FILE* file, const char* msg)
#define SCIP_DECL_MESSAGEINFO(x) void x (SCIP_MESSAGEHDLR* messagehdlr, FILE* file, const char* msg)
#define SCIP_DECL_MESSAGEHDLRFREE(x) SCIP_RETCODE x (SCIP_MESSAGEHDLR* messagehdlr)
typedef struct SCIP_Messagehdlr SCIP_MESSAGEHDLR;
typedef struct SCIP_MessagehdlrData SCIP_MESSAGEHDLRDATA;
enum SCIP_Objsense
{
   SCIP_OBJSENSE_MAXIMIZE = -1,         
   SCIP_OBJSENSE_MINIMIZE = +1          
};
typedef enum SCIP_Objsense SCIP_OBJSENSE;
enum SCIP_Vartype
{
   SCIP_VARTYPE_BINARY     = 0,         
   SCIP_VARTYPE_INTEGER    = 1,         
   SCIP_VARTYPE_IMPLINT    = 2,         
   SCIP_VARTYPE_CONTINUOUS = 3          
};
typedef enum SCIP_Vartype SCIP_VARTYPE;
typedef struct SCIP_Var SCIP_VAR;
enum SCIP_Status
{
   SCIP_STATUS_UNKNOWN        =  0,     
   SCIP_STATUS_USERINTERRUPT  =  1,     
   SCIP_STATUS_NODELIMIT      =  2,     
   SCIP_STATUS_TOTALNODELIMIT =  3,     
   SCIP_STATUS_STALLNODELIMIT =  4,     
   SCIP_STATUS_TIMELIMIT      =  5,     
   SCIP_STATUS_MEMLIMIT       =  6,     
   SCIP_STATUS_GAPLIMIT       =  7,     
   SCIP_STATUS_SOLLIMIT       =  8,     
   SCIP_STATUS_BESTSOLLIMIT   =  9,    
   SCIP_STATUS_RESTARTLIMIT   = 10,     
   SCIP_STATUS_OPTIMAL        = 11,     
   SCIP_STATUS_INFEASIBLE     = 12,     
   SCIP_STATUS_UNBOUNDED      = 13,     
   SCIP_STATUS_INFORUNBD      = 14,     
   SCIP_STATUS_TERMINATE      = 15      
};
typedef enum SCIP_Status SCIP_STATUS;
typedef struct SCIP_Stat SCIP_STAT;
typedef struct SCIP_Sol SCIP_SOL;
#define SCIP_EVENTTYPE_BESTSOLFOUND     UINT64_C(0x01000000)  
#define SCIP_DECL_EVENTINIT(x) SCIP_RETCODE x (SCIP* scip, SCIP_EVENTHDLR* eventhdlr)
#define SCIP_DECL_EVENTEXIT(x) SCIP_RETCODE x (SCIP* scip, SCIP_EVENTHDLR* eventhdlr)
#define SCIP_DECL_EVENTEXEC(x) SCIP_RETCODE x (SCIP* scip, SCIP_EVENTHDLR* eventhdlr, SCIP_EVENT* event, SCIP_EVENTDATA* eventdata)
typedef uint64_t SCIP_EVENTTYPE;
typedef struct SCIP_Eventhdlr SCIP_EVENTHDLR;
typedef struct SCIP_EventhdlrData SCIP_EVENTHDLRDATA;
typedef struct SCIP_Event SCIP_EVENT;
typedef struct SCIP_EventData SCIP_EVENTDATA;
typedef struct SCIP_Cons SCIP_CONS;
enum SCIP_ParamType
{
   SCIP_PARAMTYPE_BOOL    = 0,               
   SCIP_PARAMTYPE_INT     = 1,               
   SCIP_PARAMTYPE_LONGINT = 2,               
   SCIP_PARAMTYPE_REAL    = 3,               
   SCIP_PARAMTYPE_CHAR    = 4,               
   SCIP_PARAMTYPE_STRING  = 5                
};
typedef enum SCIP_ParamType SCIP_PARAMTYPE;
typedef struct SCIP_Param SCIP_PARAM;
enum SCIP_BoundType
{
   SCIP_BOUNDTYPE_LOWER = 0,            
   SCIP_BOUNDTYPE_UPPER = 1             
};
typedef enum SCIP_BoundType SCIP_BOUNDTYPE;
enum SCIP_OrbitopeType
{
   SCIP_ORBITOPETYPE_FULL         = 0,       
   SCIP_ORBITOPETYPE_PARTITIONING = 1,       
   SCIP_ORBITOPETYPE_PACKING      = 2        
};
typedef enum SCIP_OrbitopeType SCIP_ORBITOPETYPE;
