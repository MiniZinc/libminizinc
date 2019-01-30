#include <minizinc/solvers/nl/nl_expressions.hh>

namespace MiniZinc {

    int getNLOpCode(BinOpType bot){
        switch(bot){
            // Arithmetic
            case BOT_PLUS:  return 0;
            case BOT_MINUS: return 1;
            case BOT_MULT:  return 2;
            case BOT_DIV:   return 3;
            case BOT_IDIV:  return 55;
            case BOT_MOD:   return 4;
            case BOT_POW:   return 5;
            // Comparison
            case BOT_LE:    return 22;
            case BOT_LQ:    return 23;
            case BOT_GR:    return 29;
            case BOT_GQ:    return 28;
            case BOT_EQ:    return 24;
            case BOT_NQ:    return 30;
            // Set operators
            case BOT_IN:        cerr << "BOT_IN not implemented"; assert(false);
            case BOT_SUBSET:    cerr << "BOT_SUBSET not implemented"; assert(false);
            case BOT_SUPERSET:  cerr << "BOT_SUPERSET not implemented"; assert(false);
            case BOT_UNION:     cerr << "BOT_UNION not implemented"; assert(false);
            case BOT_DIFF:      cerr << "BOT_DIFF not implemented"; assert(false);
            case BOT_SYMDIFF:   cerr << "BOT_SYMDIFF not implemented"; assert(false);
            // Others...
            case BOT_INTERSECT: cerr << "BOT_INTERSECT not implemented"; assert(false);
            case BOT_PLUSPLUS:  cerr << "BOT_PLUSPLUS not implemented"; assert(false);
            // Logic
            case BOT_EQUIV:     cerr << "BOT_EQUIV not implemented"; assert(false);
            case BOT_IMPL:      cerr << "BOT_IMPL not implemented"; assert(false);
            case BOT_RIMPL:     cerr << "BOT_RIMPL not implemented"; assert(false);
            case BOT_OR:        return 20;
            case BOT_AND:       return 21;
            case BOT_XOR:       cerr << "BOT_XOR not implemented"; assert(false);
            case BOT_DOTDOT:    cerr << "BOT_DOTDOT not implemented"; assert(false);
            default:            assert(false);
        }
    }

    


}
