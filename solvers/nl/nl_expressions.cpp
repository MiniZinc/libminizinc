#include <minizinc/solvers/nl/nl_expressions.hh>

namespace MiniZinc {

    /** *** *** *** Construction helpers *** *** *** **/
    NLToken NLToken::n(double value){
        NLToken tok;
        tok.kind = Kind::NUMERIC;
        tok.numeric_value = value;
        return tok;
    }

    NLToken NLToken::v(int idx, string vname){
        NLToken tok;
        tok.kind = Kind::VARIABLE;
        tok.str = vname;
        tok.variable_index = idx;
        return tok;
    }

    NLToken NLToken::o(OpCode opc){
        NLToken tok;
        tok.kind = Kind::OP;
        tok.oc = opc;
        return tok;
    }

    NLToken NLToken::mo(MOpCode mopc, int nb){
        NLToken tok;
        tok.kind = Kind::MOP;
        tok.moc = mopc;
        tok.nb_args = nb;
        return tok;
    }

    // Print a NLtoken
    ostream& NLToken::print_on(ostream& os) const {

        switch (kind) {

            case Kind::NUMERIC:{
                os << "n" << numeric_value;
                break;
            }

            case Kind::VARIABLE:{
                os << "v" << variable_index << " # " << str;
                break;
            }

            case Kind::STRING:{
                cerr << "Kind::STRING not implemented" << endl;
                assert(false);
                break;
            }

            case Kind::FUNCALL:{
                cerr << "Kind::FUNCALL not implemented" << endl;
                assert(false);
                break;
            }

            case Kind::OP:{
                os << "o" << oc << " # " << get_name(oc);
                break;
            }

            case Kind::MOP:{
                os << "o" << moc << " # " << get_name(moc) << " " << nb_args << endl;
                os << nb_args;
                break;
            }

            default: assert(false);
        }


        return os;
    }

    // Get the name of an operation:
    const char* NLToken::get_name(OpCode oc){
        switch(oc){
            case OpCode::OPPLUS: return "OPPLUS";
            case OpCode::OPMINUS: return "OPMINUS";
            case OpCode::OPMULT: return "OPMULT";
            case OpCode::OPDIV: return "OPDIV";
            case OpCode::OPREM: return "OPREM";
            case OpCode::OPPOW: return "OPPOW";
            case OpCode::OPLESS: return "OPLESS";
            case OpCode::FLOOR: return "FLOOR";
            case OpCode::CEIL: return "CEIL";
            case OpCode::ABS: return "ABS";
            case OpCode::OPUMINUS: return "OPUMINUS";
            case OpCode::OPOR: return "OPOR";
            case OpCode::OPAND: return "OPAND";
            case OpCode::LT: return "LT";
            case OpCode::LE: return "LE";
            case OpCode::EQ: return "EQ";
            case OpCode::GE: return "GE";
            case OpCode::GT: return "GT";
            case OpCode::NE: return "NE";
            case OpCode::OPNOT: return "OPNOT";
            case OpCode::OPIFnl: return "OPIFnl";
            case OpCode::OP_tanh: return "OP_tanh";
            case OpCode::OP_tan: return "OP_tan";
            case OpCode::OP_sqrt: return "OP_sqrt";
            case OpCode::OP_sinh: return "OP_sinh";
            case OpCode::OP_sin: return "OP_sin";
            case OpCode::OP_log10: return "OP_log10";
            case OpCode::OP_log: return "OP_log";
            case OpCode::OP_exp: return "OP_exp";
            case OpCode::OP_cosh: return "OP_cosh";
            case OpCode::OP_cos: return "OP_cos";
            case OpCode::OP_atanh: return "OP_atanh";
            case OpCode::OP_atan2: return "OP_atan2";
            case OpCode::OP_atan: return "OP_atan";
            case OpCode::OP_asinh: return "OP_asinh";
            case OpCode::OP_asin: return "OP_asin";
            case OpCode::OP_acosh: return "OP_acosh";
            case OpCode::OP_acos: return "OP_acos";
            case OpCode::OPintDIV: return "OPintDIV";
            case OpCode::OPprecision: return "OPprecision";
            case OpCode::OPround: return "OPround";
            case OpCode::OPtrunc: return "OPtrunc";
            case OpCode::OPATLEAST: return "OPATLEAST";
            case OpCode::OPATMOST: return "OPATMOST";
            case OpCode::OPPLTERM: return "OPPLTERM";
            case OpCode::OPIFSYM: return "OPIFSYM";
            case OpCode::OPEXACTLY: return "OPEXACTLY";
            case OpCode::OPNOTATLEAST: return "OPNOTATLEAST";
            case OpCode::OPNOTATMOST: return "OPNOTATMOST";
            case OpCode::OPNOTEXACTLY: return "OPNOTEXACTLY";
            case OpCode::OPIMPELSE: return "OPIMPELSE";
            case OpCode::OP_IFF: return "OP_IFF";
            case OpCode::OPSOMESAME: return "OPSOMESAME";
            case OpCode::OP1POW: return "OP1POW";
            case OpCode::OP2POW: return "OP2POW";
            case OpCode::OPCPOW: return "OPCPOW";
            case OpCode::OPFUNCALL: return "OPFUNCALL";
            case OpCode::OPNUM: return "OPNUM";
            case OpCode::OPHOL: return "OPHOL";
            case OpCode::OPVARVAL: return "OPVARVAL";
            case OpCode::N_OPS: return "N_OPS";
            default: assert(false); return NULL;
        }
    };

    const char* NLToken::get_name(MOpCode moc){
        switch(moc){        
            case MOpCode::MINLIST: return "MINLIST";
            case MOpCode::MAXLIST: return "MAXLIST";
            case MOpCode::OPSUMLIST: return "OPSUMLIST";
            case MOpCode::OPCOUNT: return "OPCOUNT";
            case MOpCode::OPNUMBEROF: return "OPNUMBEROF";
            case MOpCode::OPNUMBEROFs: return "OPNUMBEROFs";
            case MOpCode::ANDLIST: return "ANDLIST";
            case MOpCode::ORLIST: return "ORLIST";
            case MOpCode::OPALLDIFF: return "OPALLDIFF";
            default: assert(false); return NULL;
        }            
    }

}
