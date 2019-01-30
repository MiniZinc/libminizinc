#include <minizinc/solvers/nl/nl_file.hh>

/**
 *  A NL File reprensentation.
 *  The purpose of this file is mainly to be a writer.
 *  Most of the information come from 'nlwrite.pdf'
 *  Note:
 *      * a character '#' starts a comment until the end of line.
 *      * A new line is '\n'
 */

namespace MiniZinc {

    // --- --- --- NLFile

    ostream& NLFile::print_on(ostream& os) const{
        os << header << endl << "# --- END OF HEADER" << endl << endl;
        os << NLS_BoundSeg(this) << endl << endl;
        //header.print_on(os);

        return os;
    }
    

    // --- --- --- --- --- --- VDECL

    void NLFile::add_vdecl_enum(){}

    void NLFile::add_vdecl_tystr(){}


    /** Adding a new integer variable declaration */
    void NLFile::vdecl_integer(const string& name, const IntSetVal* intSet){
        // Ensure that the variable name is available, then insert it.
        assert(variables.find(name) == variables.end());
        // Get the GLOBAL index of the variable
        int index = variables.size();
        // Check the domain
        long llb, lub;
        if(intSet->size() == 0){
            cerr << "Variable " << name << ": empty domain not implemented" << endl;
            assert(false);      
        } else if (intSet->size() == 1){
            llb = intSet->min(0).toInt();
            lub = intSet->max(0).toInt();
        } else {
            cerr << "Variable " << name << ": infinite/set domain not implemented" << endl;
            assert(false);
        }
        // Create the variable
        cerr << "Integer variable declaration " << llb << ".." << name << ".." << lub << ", index=" << index << " ";
        Var v = Var(name, index, true, NLS_BoundItem::make_bounded(llb, lub, index));
        // Update Internal structure & Header
        variables[name] = v;
        name_vars.push_back(name);
        header.nb_vars++;
    }

    /** Adding a new floating point variable declaration */
    void NLFile::vdecl_fp(const string& name, const FloatSetVal* floatSet){
        // Ensure that the variable name is available, then insert it.
        assert(variables.find(name) == variables.end());
        // Get the GLOBAL index of the variable
        int index = variables.size();
        // Check the domain
        double dlb, dub;
        if(floatSet->size() == 0){
            cerr << "Variable " << name << ": empty domain not implemented" << endl;
            assert(false);      
        } else if (floatSet->size() == 1){
            dlb = floatSet->min(0).toDouble();
            dub = floatSet->max(0).toDouble();
        } else {
            cerr << "Variable " << name << ": infinite/set domain not implemented" << endl;
            assert(false);
        }
        // Create the variable
        cerr << "Floating Point variable declaration " << dlb << ".." << name << ".." << dub << ", index=" << index << " ";
        Var v = Var(name, index, false, NLS_BoundItem::make_bounded(dlb, dub, index));
        // Update Internal structure & Header
        variables[name] = v;
        name_vars.push_back(name);
        header.nb_vars++;
    }   

}
