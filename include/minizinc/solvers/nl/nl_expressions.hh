#ifndef __MINIZINC_NL_EXPRESSIONS_HH__
#define __MINIZINC_NL_EXPRESSIONS_HH__


#include <ostream>
#include <iostream>
#include <string>
#include <assert.h>

#include <minizinc/solvers/nl/nl_printable.hh>

using namespace std;

namespace MiniZinc {

    /** A token from an expression graph.
     * An expression is express in Polish Prefix Notation: operator followed by operand.
     * A token represent an operator or an operand
     */
    class NLToken: public Printable {
        public:
        
        enum Kind{
            NUMERIC,        // "n42.42"     a numeric constant, double
            VARIABLE,       // "v4"         reference to a decision variable 0<= i < nb_vars (see header) or a defined variable for i>=nb_vars
            STRING,         // "h11:some string"    Probably unused in our case.
            FUNCALL,        // "f0 3"               Call a defined function (index 0, 3 args). Probably unused in our case.
            OP,             // "o5"                 An operation defined by its operation code
            MOP             // "o7\n3"              Operator with multiple operand
        };

        enum OpCode{
            OPPLUS		    = 0,
            OPMINUS		    = 1,
            OPMULT		    = 2,
            OPDIV		    = 3,
            OPREM		    = 4,
            OPPOW		    = 5,
            OPLESS		    = 6,
            FLOOR		    = 13,
            CEIL		    = 14,
            ABS		        = 15,
            OPUMINUS	    = 16,
            OPOR		    = 20,
            OPAND		    = 21,
            LT		        = 22,
            LE		        = 23,
            EQ		        = 24,
            GE		        = 28,
            GT		        = 29,
            NE		        = 30,
            OPNOT		    = 34,
            OPIFnl		    = 35,
            OP_tanh		    = 37,
            OP_tan		    = 38,
            OP_sqrt		    = 39,
            OP_sinh		    = 40,
            OP_sin		    = 41,
            OP_log10	    = 42,
            OP_log		    = 43,
            OP_exp		    = 44,
            OP_cosh		    = 45,
            OP_cos		    = 46,
            OP_atanh	    = 47,
            OP_atan2	    = 48,
            OP_atan		    = 49,
            OP_asinh	    = 50,
            OP_asin		    = 51,
            OP_acosh	    = 52,
            OP_acos		    = 53,
            OPintDIV	    = 55,
            OPprecision	    = 56,
            OPround		    = 57,
            OPtrunc		    = 58,
            OPATLEAST	    = 62,
            OPATMOST	    = 63,
            OPPLTERM	    = 64,
            OPIFSYM		    = 65,
            OPEXACTLY	    = 66,
            OPNOTATLEAST	= 67,
            OPNOTATMOST	    = 68,
            OPNOTEXACTLY	= 69,            
            OPIMPELSE	    = 72,
            OP_IFF		    = 73,
            OPSOMESAME	    = 75,
            OP1POW		    = 76,
            OP2POW		    = 77,
            OPCPOW		    = 78,
            OPFUNCALL	    = 79,
            OPNUM		    = 80,
            OPHOL		    = 81,
            OPVARVAL	    = 82,
            N_OPS           = 83,
        };

        enum MOpCode{
            MINLIST		    = 11,
            MAXLIST		    = 12,
            OPSUMLIST	    = 54,
            OPCOUNT		    = 59,
            OPNUMBEROF	    = 60,
            OPNUMBEROFs	    = 61,
            ANDLIST		    = 70,
            ORLIST		    = 71,
            OPALLDIFF	    = 74,                        
        };

        static const char* get_name(OpCode oc);

        static const char* get_name(MOpCode moc);

        // --- --- Fields
        Kind    kind;
        double  numeric_value;      // if kind==NUMERIC
        int     variable_index;     // if kind==VARIABLE or the index of the function if kind==FUNCALL
        int     nb_args;            // if kind==FUNCALL or kind==MOP
        string  str;                // if kind==STRING or kind=VARIABLE
        OpCode  oc;                 // if kind==OP
        MOpCode moc;                // if kind==MOP

        public:
        NLToken()=default;
        ostream& print_on( ostream& o ) const override;

        // --- --- Helpers
        static NLToken n(double value);

        static NLToken v(int idx, string vname);

        static NLToken o(OpCode opc);

        static NLToken mo(MOpCode mopc, int nb);
    };

}


#endif