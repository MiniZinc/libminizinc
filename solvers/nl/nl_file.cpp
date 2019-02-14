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

    /** *** *** *** Printable Interface *** *** *** **/

    // Note:  * empty line not allowed
    //        * comment only not allowed
    ostream& NLFile::print_on(ostream& os) const{
        os << header << endl;

        os << b_segment;

        os << r_segment;

        for(auto &cseg: c_segments){
            os << cseg;
        }

        // K segment here if we have some j_segments
        if(!j_segments.empty()){

          if(!name_vars.empty()){
            os << "k" << (header.nb_vars-1) << " # Cumulative Sum of non-zero in the jacobian matrix's (nbvar-1) columns." << endl;
            int acc=0;
            // Note: stop before the last var. Total count will be in the header
            for(int i=0; i<name_vars.size()-1; ++i){
              string name = name_vars[i];
              acc += variables.at(name).jacobian_count;
              os << acc << " # " << name << endl;
            }
            /*  Line "comment only" not allowed (tested with gecode)!!!! For real...
            // Last variable in comments. NL format use the header to complete the information (non zero in jacobian)
            string name = name_vars[name_vars.size()-1];
            acc += variables.at(name).jacobian_count;
            os << "# " << acc << " # " << name << " Equivalent to the last column. NL format use the header to determine this value." << endl;
            */
          }

          // J segments here
          for(auto &jseg: j_segments){
            os << jseg;
          }
        }

        for(auto &lseg: l_segments){
            os << lseg;
        }

        os << o_segment;

        return os;
    }
    
    /** *** *** *** Helpers *** *** *** **/
    
    /** Obtain the name of the variable */
    string NLFile::get_vname(const VarDecl &vd){
        stringstream os;
        if (vd.id()->idn() != -1) {  os << " X_INTRODUCED_" << vd.id()->idn() << "_"; }
        else if (vd.id()->v().size() != 0){ os << " " << vd.id()->v(); }
        string name = os.str();
        return name;
    }

    /** Obtain the vector of an array, either from an identifier or an array litteral */
    ASTExprVec<Expression> NLFile::get_vec(const Expression* e){
        switch(e->eid()){
            case Expression::E_ID: {
                // Follow the pointer to the expression of the declaration
                return get_vec( e->cast<Id>()->decl()->e() );
            }

            case Expression::E_ARRAYLIT: {
                const ArrayLit& al = *e->cast<ArrayLit>();
                return al.getVec();
            }
        }
        cerr << "Could not read array from expression." << endl;
        assert(false);
        return {}; // never reached
    }

    /** Create the J segment for integer values **/
    NLS_JSeg NLFile::make_jseg_int(int considx, const ASTExprVec<Expression> &coeffs, const ASTExprVec<Expression> &vars){
        NLS_JSeg jseg(this, considx);
        for(unsigned int i=0; i<coeffs.size(); ++i){
              double co      = coeffs[i]->cast<IntLit>()->v().toInt();
              Var* v         = &variables[get_vname(*(vars[i]->cast<Id>()->decl()))];
              int vidx       = v->index;
              v->jacobian_count++;
              header.nb_nonzero_jacobian++;
              jseg.var_coeff.push_back(pair<int, double>(vidx, co));
        }
        return jseg;
    }

    /** Create the J segment for floating point values **/
    NLS_JSeg NLFile::make_jseg_fp(int considx, const ASTExprVec<Expression> &coeffs, const ASTExprVec<Expression> &vars){
        NLS_JSeg jseg(this, considx);
        for(unsigned int i=0; i<coeffs.size(); ++i){
              double co      = coeffs[i]->cast<FloatLit>()->v().toDouble();
              Var* v         = &variables[get_vname(*(vars[i]->cast<Id>()->decl()))];
              int vidx       = v->index;
              v->jacobian_count++;
              header.nb_nonzero_jacobian++;
              jseg.var_coeff.push_back(pair<int, double>(vidx, co));
        }
        return jseg;
    }        


    /** *** *** *** Solve analysis *** *** *** **/
    void NLFile::analyse_solve(SolveI::SolveType st, const Expression* e){
        header.nb_objectives++;
        
        switch(st){
            case SolveI::SolveType::ST_SAT:{
                // Satisfy: implemented by minimizing 0
                o_segment.minmax = o_segment.MINIMIZE;
                o_segment.expression_graph.push_back(NLToken::n(0));
                break;
            }
            case SolveI::SolveType::ST_MIN:{
                cerr << "solve min not implemented (todo: checking expression)" << endl;
                assert(false);
                break;
            }
            case SolveI::SolveType::ST_MAX:{
                cerr << "solve max not implemented (todo: checking expression)" << endl;
                assert(false);                
                break;
            }
        }
    }

    



    /** *** *** *** Variable declaration methods *** *** *** **/

    /** Analyse a variable declaration vd of type ti with an ths
     * The variable declaration gives us access to the variable name while the type
     * allows us to discriminate between integer, floating point value and arrays.
     * Array are ignored (not declared): if we encouter an array in a constraint,
     * we can find the array through the variable (ot it is a litteral).
     * Note that we use -Glinear, so we do not have boolean.
     * 
     * RHS is for arrays: contain the definition of the array
     * 
     * The type also gives us the domain, which can be:
     *  NULL:       no restriction over the variable
     *  SetLit:     Gives us a lower and upper bound
     *  If a variable is bounded only on one side, then the domain is NULL and the bound is expressed through a constraint.
     * 
     */
    void NLFile::analyse_vdecl(const VarDecl &vd, const TypeInst &ti, const Expression &rhs){
        // Get the name
        string name = get_vname(vd);


        if(ti.isEnum()){ cerr << "Should not happen" << endl; assert(false); }
        else if(ti.isarray()){
            // In flatzinc, array always have a rhs: they can alway be replaced by their definition.
            // Follows the pointer starting at the ID to do so.
            cerr << "Definition of array " << name << " is not reproduced in nl.";
                    // Gather output variable
        if(vd.ann().containsCall(constants().ann.output_array)){
            // For now, just x = [ list... ]
            // todo
        }
        } else {
                    // Gather output variable
        if(vd.ann().contains(constants().ann.add_to_output)){
            // todo
        }
            // variable declaration
            const Type& type          = ti.type();
            const Expression* domain  = ti.domain();
            // Check the type: integer or floatin point
            assert(type.isvarint()||type.isvarfloat());
            bool isvarint = type.isvarint();
            // Call the declaration function according to the type
            // Check the domain and convert if not null.
            // Note: we directly jump to the specialized Int/Float set, going through the set literal
            if(isvarint){
                // Integer
                IntSetVal* isv = NULL;
                if(domain!=NULL){isv = domain->cast<SetLit>()->isv();}
                vdecl_integer(name, isv);
            } else {
                // Floating point
                FloatSetVal* fsv = NULL;
                if(domain!=NULL){fsv = domain->cast<SetLit>()->fsv();}
                vdecl_fp(name, fsv);
            }
        }
    }

    /** Declare a new integer variable, maybe bounded if intSet!=NULL **/
    void NLFile::vdecl_integer(const string& name, const IntSetVal* intSet){
        // Check that the name is available. Get variable GLOBAL index (0-base => number of vars before insertion).
        assert(variables.find(name) == variables.end());
        int index = variables.size();
        // Check the domain
        NLS_BoundItem bound;
        if(intSet == NULL){
            bound = NLS_BoundItem::make_nobound(index);
        } else if(intSet->size()==1){
            long long lb = intSet->min(0).toInt();
            long long ub = intSet->max(0).toInt();
            bound = NLS_BoundItem::make_bounded(lb, ub, index);
        } else {
            cerr << "Should not happen: switch on mzn_opt_only_range_domains" << endl;
            assert(false);
        }
        // Create the variable and update the internal structure & header
        Var v = Var(name, index, true, bound);  // true == integer
        variables[name] = v;
        name_vars.push_back(name);
        header.nb_vars++;
        header.nb_linear_integer_vars++;        // Also update that one!
    }

    /** Declare a new floating point variable, maybe bounded if floatSet!=NULL **/
    void NLFile::vdecl_fp(const string& name, const FloatSetVal* intSet){
        // Check that the name is available. Get variable GLOBAL index (0-base => number of vars before insertion).
        assert(variables.find(name) == variables.end());
        int index = variables.size();
        // Check the domain
        NLS_BoundItem bound;
        if(intSet == NULL){
            bound = NLS_BoundItem::make_nobound(index);
        } else if(intSet->size()==1){
            double lb = intSet->min(0).toDouble();
            double ub = intSet->max(0).toDouble();
            bound = NLS_BoundItem::make_bounded(lb, ub, index);
        } else {
            cerr << "Should not happen: switch on mzn_opt_only_range_domains" << endl;
            assert(false);
        }
        // Create the variable and update the internal structure & header
        Var v = Var(name, index, false, bound);  // false == floating point
        variables[name] = v;
        name_vars.push_back(name);
        header.nb_vars++;
    }   



    /** *** *** *** Constraint methods *** *** *** **/

    void NLFile::analyse_constraint(const Call& c){
        // Guard
        if(c.decl() == NULL){ cerr << "Undeclared function " << c.id(); assert(false); }

        // ID of the call
        auto id = c.id();
        // Constants for integer builtins
        auto consint = constants().ids.int_;
        // Constants for floating point builtins
        auto consfp = constants().ids.float_;

        // Dispatch among integer builtins
        if(id == consint.lin_eq){       consint_lin_eq(c); }
        else if(id == consint.lin_le){  consint_lin_le(c); }
        else if(id == consint.lin_ne){  consint_lin_ne(c); }
        else if(id == consint.times){   consint_times(c); }
        else if(id == consint.div){     consint_div(c); }
        else if(id == consint.mod){     consint_mod(c); }
        else if(id == consint.plus){    cerr << "Should not happen 'int plus'"; assert(false); }
        else if(id == consint.minus){   cerr << "Should not happen 'int minus'"; assert(false); }
        else if(id == consint.lt){      cerr << "Should not happen 'int lt'"; assert(false); }
        else if(id == consint.le){      consint_le(c); }
        else if(id == consint.gt){      cerr << "Should not happen 'int gt'"; assert(false); }
        else if(id == consint.ge){      cerr << "Should not happen 'int ge'"; assert(false); }
        else if(id == consint.eq){      consint_eq(c); }
        else if(id == consint.ne){      consint_ne(c); }

        // Dispatch among floating point builtins
        else if(id == consfp.lin_eq){   consfp_lin_eq(c); }
        else if(id == consfp.lin_le){   consfp_lin_le(c); }
        else if(id == consfp.lin_lt){   consfp_lin_lt(c); }
        else if(id == consfp.lin_ne){   consfp_lin_ne(c); }
        else if(id == consfp.plus){     consfp_plus(c); }
        else if(id == consfp.minus){    consfp_minus(c); }
        else if(id == consfp.times){    consfp_times(c); }
        else if(id == consfp.div){      consfp_div(c); }
        else if(id == consfp.mod){      consfp_mod(c); }
        else if(id == consfp.lt){       consfp_lt(c); }
        else if(id == consfp.le){       consfp_le(c); }
        else if(id == consfp.gt){       cerr << "Should not happen 'float gt'"; assert(false); }
        else if(id == consfp.ge){       cerr << "Should not happen 'float ge'"; assert(false); }
        else if(id == consfp.eq){       consfp_eq(c); }
        else if(id == consfp.ne){       consfp_ne(c); }
        else if(id == consfp.in){       cerr << "Ignore for now: constraint 'float in    ' not implemented"; assert(false); }
        else if(id == consfp.dom){      cerr << "Ignore for now: constraint 'float dom   ' not implemented"; assert(false); }
        
        else {
            cerr << "Unrecognized builtins " << c.id() << " not implemented";
            assert(false);
        }
    }



    /** *** *** *** Integer Constraint methods *** *** *** **/

    // --- --- --- Linear Constraints
    // flatzinc:
    // For a call c, we have 3 arguments:
    // c.arg(0) and c.arg(1) are always arrays: array0 = coefficients and array1 = variables.
    // c.arg(2) is the 'value'.
    //
    // nl:
    // Update the header, taking specific cases (range/equality/logical) into account
    //
    // Algebraic Cases: = and =<
    // C segment: non linear part of the contraint. Will be 0. Update the number of algebraic contraints
    // J segment: linear part of the contraint: list of couple (variable index, coefficient)
    // update of the r segment: the "range' constraint over the body (constraint number 'i' matches the ith line of the segment)
    //
    // Logical Case: !=
    // L segment: logical constraint. Update the number of logical constraints
    // The constraint is built with an expression graph, just as a non linear constraint.

    /** Linar constraint: [array0] *+ [array1] = value **/
    void NLFile::consint_lin_eq(const Call& c){
        // Get the arg
        ASTExprVec<Expression> coeffs = get_vec(c.arg(0));
        ASTExprVec<Expression> vars   = get_vec(c.arg(1));
        long long value               =  c.arg(2)->cast<IntLit>()->v().toInt();

        // Get the constraint index and increase the number of algebraic constraint.
        // Note: Also include the "equality constraint" counter;
        int considx = header.nb_algebraic_constraints;
        header.nb_algebraic_constraints++;
        header.nb_equality_constraints++;

        // C segment: only contains 'n0' (all the constraint in the J seg)
        NLS_CSeg cseg(this, considx);
        cseg.expression_graph.push_back(NLToken::n(0));
        c_segments.push_back(cseg);

        // J segment: the linear part of the constraint.
        NLS_JSeg jseg = make_jseg_int(considx, coeffs, vars);
        j_segments.push_back(jseg);
        
        // r segment: equal constraint
        AlgebraicCons ac = AlgebraicCons(considx, NLS_BoundItem::make_equal(value, considx));
        r_segment.addConstraint(ac);
    }

    /** Linar constraint: [array0] *+ [array1] =< value **/
    void NLFile::consint_lin_le(const Call& c){
        // Get the arg
        ASTExprVec<Expression> coeffs = get_vec(c.arg(0));
        ASTExprVec<Expression> vars   = get_vec(c.arg(1));
        long long value               =  c.arg(2)->cast<IntLit>()->v().toInt();

        // Get the constraint index and increase the number of algebraic constraint.
        // Note: this is not a range constraint, even if it has a 'range segment' line.
        int considx = header.nb_algebraic_constraints;
        header.nb_algebraic_constraints++;

        // C segment: only contains 'n0' (all the constraint in the J seg)
        NLS_CSeg cseg(this, considx);
        cseg.expression_graph.push_back(NLToken::n(0));
        c_segments.push_back(cseg);

        // J segment: the linear part of the constraint.
        NLS_JSeg jseg = make_jseg_int(considx, coeffs, vars);
        j_segments.push_back(jseg);
        
        // r segment: range constraint
        AlgebraicCons ac = AlgebraicCons(considx, NLS_BoundItem::make_ub_bounded(value, considx));
        r_segment.addConstraint(ac);
    }

    /** Linar constraint: [array0] *+ [array1] != value **/
    void NLFile::consint_lin_ne(const Call& c){
        // Get the arg
        ASTExprVec<Expression> coeffs = get_vec(c.arg(0));
        ASTExprVec<Expression> vars   = get_vec(c.arg(1));
        long long value               =  c.arg(2)->cast<IntLit>()->v().toInt();

        // This is a logical constraint: get the constraint index and increase the number of logical constraint.
        int considx = header.nb_logical_constraints;
        header.nb_logical_constraints++;

        // L segment:
        NLS_LSeg lseg(this, considx);
        // 1) Push the comparison  "!= operand1 operand2"
        lseg.expression_graph.push_back(NLToken::o(NLToken::OpCode::NE));
        // 2) Operand1 = sum of product
        // All the sums in one go
        lseg.expression_graph.push_back(NLToken::mo(NLToken::MOpCode::OPSUMLIST, coeffs.size()));
        for(unsigned int i=0; i<coeffs.size(); ++i){
            double co       = coeffs[i]->cast<IntLit>()->v().toInt();
            string vname    = get_vname(*(vars[i]->cast<Id>()->decl()));
            int    vidx     = variables[vname].index;
            // Product
            lseg.expression_graph.push_back(NLToken::o(NLToken::OpCode::OPMULT));
            lseg.expression_graph.push_back(NLToken::n(co));
            lseg.expression_graph.push_back(NLToken::v(vidx, vname));
        }
        // 3) Operand 2 = value
        lseg.expression_graph.push_back(NLToken::n(value));
        l_segments.push_back(lseg);
    }

    // --- --- --- Non Linear Constraints: operations
    // Flatzinc:
    // For a call c, we have 3 arguments:
    // c.arg(0) and c.arg(1) are the operands
    // c.arg(2) is the value to which it is compared.
    // The arguments are either variable or parameter

    /** Non linear constraint: x * y = z **/
    void NLFile::consint_times(const Call& c){
      cerr << "Non linear to be implementeed constraint 'int times'   not implemented"; assert(false);
    }

    /** Non linear constraint: x / y = z **/
    void NLFile::consint_div(const Call& c){
      cerr << "Non linear to be implementeed constraint 'int div'   not implemented"; assert(false);
    }

    /** Non linear constraint: x mod y = z **/
    void NLFile::consint_mod(const Call& c){
      cerr << "Non linear to be implementeed constraint 'int mod'   not implemented"; assert(false);
    }



    // --- --- --- Non Linear Constraints: comparisons
    // Flatzinc:
    // For a call c, we have 2 arguments, c.arg(0) and c.arg(1), being then operands of the comparison.
    // The arguments are either variable or parameter

    /** Non linear constraint x =< y **/
    void NLFile::consint_le(const Call& c){
        Expression* arg0 = c.arg(0);
        Expression* arg1 = c.arg(1);

        // Bound checking: if arg0 is a parameter, its value is fixed.
        // We have a constraint that create a partial a partial bound (or update a bound)
        if(arg0->type().ispar()){
            // Get the lower bound, the variable declaration and the associated name
            long long lb = arg0->cast<IntLit>()->v().toInt();
            VarDecl& vd = *(arg1->cast<Id>()->decl());
            // Get the variable itself and its bound
            string name = get_vname(vd);
            Var v1 = variables[name];
            // New bound as a copy. Update the copy, create an updated var and put it in the map
            NLS_BoundItem newBound = v1.bound;
            newBound.update_lb(lb);         // param <= var: lower bound
            Var new_v = Var(name, v1.index, v1.is_integer, newBound);
            variables[name] = new_v;
        } else if(arg1->type().ispar()){
            // Symmetric, with an upper bound var <= param
            VarDecl& vd = *(arg0->cast<Id>()->decl());
            long long ub = arg1->cast<IntLit>()->v().toInt();            
            string name = get_vname(vd);
            Var v0 = variables[name];
            NLS_BoundItem newBound = v0.bound;
            newBound.update_ub(ub);         // var <= param: upper bound
            Var new_v = Var(name, v0.index, v0.is_integer, newBound);
            variables[name] = new_v;
        } else {
            // --- --- --- Actual constraint
            cerr << "not implemented" << endl;
            assert(false);
        }
    }


    /** Non linear constraint x = y
     *  arg(0) should be variable
     *  arg(1) could be a var or a constant
     ***/
    void NLFile::consint_eq(const Call& c){
        Expression* arg0 = c.arg(0);
        Expression* arg1 = c.arg(1);

        // Get fist arg
        if(arg0->type().ispar()){
          cerr << "Comparison: first argument shoud be a variable"; assert(false);
        }
        
        // Get second arg.
        // If constant, update the bound on the variable, else create a constraint.
        if(arg1->type().ispar()){
          VarDecl& vd = *(arg0->cast<Id>()->decl());
          long long value = arg1->cast<IntLit>()->v().toInt();
          string name = get_vname(vd);
          Var v1 = variables[name];
          // New bound as a copy. Update the copy, create an updated var and put it in the map
          NLS_BoundItem newBound = NLS_BoundItem::make_equal(value, v1.index);
          Var new_v = Var(name, v1.index, v1.is_integer, newBound);
          variables[name] = new_v;
        }else{
          // Create the constraint:
          VarDecl& v0 = *(arg0->cast<Id>()->decl());
          VarDecl& v1 = *(arg0->cast<Id>()->decl());


        }
    }


    /** Non linear constraint x != y **/
    void NLFile::consint_ne(const Call& c){
      cerr << "constraint 'int ne' not implemented"; assert(false);
    }




    /** *** *** *** Floating Point Constraint methods *** *** *** **/

    void NLFile::consfp_lin_eq(const Call& c){
      cerr << "constraint 'float lin_eq' not implemented"; assert(false);
    }

    void NLFile::consfp_lin_le(const Call& c){
      cerr << "constraint 'float lin_le' not implemented"; assert(false);
    }

    void NLFile::consfp_lin_lt(const Call& c){
      cerr << "constraint 'float lin_lt' not implemented"; assert(false);
    }

    void NLFile::consfp_lin_ne(const Call& c){
      cerr << "constraint 'float lin_ne' not implemented"; assert(false);
    }

    void NLFile::consfp_plus(const Call& c){
      cerr << "constraint 'float plus  ' not implemented"; assert(false);
    }

    void NLFile::consfp_minus(const Call& c){
      cerr << "constraint 'float minus ' not implemented"; assert(false);
    }

    void NLFile::consfp_times(const Call& c){
      cerr << "constraint 'float times ' not implemented"; assert(false);
    }

    void NLFile::consfp_div(const Call& c){
      cerr << "constraint 'float div   ' not implemented"; assert(false);
    }

    void NLFile::consfp_mod(const Call& c){
      cerr << "constraint 'float mod   ' not implemented"; assert(false);
    }

    void NLFile::consfp_lt(const Call& c){
      cerr << "constraint 'float lt    ' not implemented"; assert(false);
    }

    void NLFile::consfp_le(const Call& c){
      cerr << "constraint 'float le    ' not implemented"; assert(false);
    }

    void NLFile::consfp_eq(const Call& c){
      cerr << "constraint 'float eq    ' not implemented"; assert(false);
    }

    void NLFile::consfp_ne(const Call& c){
      cerr << "constraint 'float ne    ' not implemented"; assert(false);
    }














  /** Check expression within a flatzinc call.
   * Can only contain:
   *    Expression::E_INTLIT
   *    Expression::E_FLOATLIT
   *    Expression::E_ID
   *    Expression::E_ARRAYLIT
   */
  const Expression* check_expression(const Expression* e) {
    // Guard
    if (e==NULL){assert(false);}

    // Dispatch on expression type
    switch (e->eid()) {

      // --- --- --- Literals
      case Expression::E_INTLIT: {
        // cerr << "case " << e->eid() << " not implemented." << endl;
        // assert(false);
        return e;
      } break;

      case Expression::E_FLOATLIT: {
        // cerr << "case " << e->eid() << " not implemented." << endl;
        // assert(false);
        return e;
      } break;

      case Expression::E_SETLIT: {
        cerr << "Set literal should not happen (use -Glinear)." << endl;
        assert(false);
      } break;

      case Expression::E_BOOLLIT: {
        cerr << "Bool literal should not happen (use -Glinear)." << endl;
        assert(false);
      } break;

      case Expression::E_STRINGLIT: {
        cerr << "String literal should not happen." << endl;
        assert(false);
      } break;

      /// --- --- --- Expressions

      case Expression::E_ID: { // Identifier
        // cerr << "case " << e->eid() << " not implemented." << endl;
        // assert(false);
        return e;
      } break;

      case Expression::E_TIID: { // Type-inst identifier
        cerr << "Type identifier should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_ANON: {
        cerr << "Anonymous variable should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_ARRAYLIT: {
        // cerr << "case " << e->eid() << " not implemented." << endl;
        // assert(false);
        return e;
      } break;

      case Expression::E_ARRAYACCESS: {
        cerr << "Array access should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_COMP:{ // Comprehension
        cerr << "Comprehension should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_ITE:{ // If-then-else expression
        cerr << "If then else should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_BINOP: {
        cerr << "Binary Op should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_UNOP: {
        cerr << "Unary Op should not happen in flatzinc call." << endl;
        assert(false);
      } break;


      case Expression::E_CALL: {
        cerr << "Call should not happen in flatzinc call." << endl;
        assert(false);
      } break;


      case Expression::E_VARDECL: {
        cerr << "Var Decl should not happen in flatzinc call." << endl;
        assert(false);
      } break;


      case Expression::E_LET: {
        cerr << "Let should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_TI: {
        cerr << "TI should not happen in flatzinc call." << endl;
        assert(false);
      } break;
    } // END OF SWITCH
    return NULL;
  } // END OF FUN








}
