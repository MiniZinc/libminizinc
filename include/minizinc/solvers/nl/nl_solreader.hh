#ifndef __MINIZINC_NL_READ_SOL_HH__
#define __MINIZINC_NL_READ_SOL_HH__

#include <string>
#include <cstring>
#include <vector>
#include <fstream>

#include <minizinc/solns2out.hh>
#include <minizinc/solvers/nl/nl_file.hh>

using namespace std;

// Reading a .sol file.
// Note: I did not find any description of the format, so I did a bit of reverse engineering.
// A sol file looks like this (first line is the 'message...' line)
/*
    message from the solver

    Options
    3
    1
    1
    0
    1
    0
    2
    2
    1
    2
    objno 0 0
*/
// A sol file does not contains comments, and the blank line between the message and 'Options' is meaningfull.
// Now, with our own comments.
/*
    message from the solver # Starts on first line. Can span several line as long as their is no blank line
    <blank line>            # the blank line marks the end of the message
    Options                 # Inform about option used by the solver (?). May be absent.
    3                       # Number of options.
    1                       # opt 1         Note:   I did not fully understand this, but reading "hooking your solver"
    1                       # opt 2                 suggests that we can (probably) ignore that.
    0                       # opt 3  
    1                       # Number A of dual before "suf_sos()" (before solving ?)
    0                       # Number of dual after. Seems to be either A or 0. Will probably always be 0 (???)
    2                       # Number B of primal before "suf_sos()" (before solving ?)
    2                       # Number of primal after. Seems to be either B or 0
    1                       # No dual. So this is the result for the first primal (variable indice 0)
    2                       # result for variable indice 1
    objno 0 0               # Objectif 0 <result code>
*/
// Result code can be :
// (from https://github.com/ampl/mp/blob/master/include/mp/common.h)
// UNKNOWN     = -1,
// SOLVED      = 0,     // Optimal sol found for an optim problem or a feasible solution found for a satisfaction problem.
// UNCERTAIN   = 100,   // Solution returned but it can be non-optimal or even infeasible.
// INFEASIBLE  = 200,
// UNBOUNDED   = 300,   // Problem is unbounded.
// LIMIT       = 400,   // Stopped by a limit, e.g. on iterations or time.
// FAILURE     = 500,   // A solver error.
// INTERRUPTED = 600    // Interrupted by the user.


namespace MiniZinc {

    /** Declaration of the exit codes. */
    enum NL_Solver_Status {
        PARSE_ERROR = -2,    // We are adding our own error code for parsing
        UNKNOWN     = -1,
        SOLVED      = 0,     // Optimal sol found for an optim problem or a feasible solution found for a satisfaction problem.
        UNCERTAIN   = 100,   // Solution returned but it can be non-optimal or even infeasible.
        INFEASIBLE  = 200,
        UNBOUNDED   = 300,   // Problem is unbounded.
        LIMIT       = 400,   // Stopped by a limit, e.g. on iterations or time.
        FAILURE     = 500,   // A solver error.
        INTERRUPTED = 600    // Interrupted by the user.
    };

    /** Represent a solution read from a file '.sol'. */
    class NLSol {
        public:
        // --- --- --- Fields
        string              message;
        NL_Solver_Status    status;
        vector<double>      values;

        // --- --- --- Constructors
        
        NLSol() = default;

        NLSol(string mes, NL_Solver_Status st, vector<double> res):
            message(mes), status(st), values(res) {}

        // --- --- --- Static functions

        static NLSol parse_sol(std::istream &in);
    };

    /** Our version of Solns2Out **/

    class NLSolns2Out {
        private:
        Solns2Out*      out;
        stringstream    ss;
        NLFile&         nl_file;
        bool            done;

        public:
        NLSolns2Out(Solns2Out* out, NLFile& nl_file): out(out), nl_file(nl_file), done(false){}

        void  parse_sol(const string& filename);

        bool feedRawDataChunk(const char* data);
        std::ostream& getLog(void);
    };
}

#endif