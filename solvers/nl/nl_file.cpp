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

  /** *** *** *** Helpers *** *** *** **/
  
  /** Create a string representing the name (and unique identifier) of a variable from a variable declaration. */
  string NLFile::get_vname(const VarDecl &vd){
      stringstream os;
      if (vd.id()->idn() != -1) {  os << "X_INTRODUCED_" << vd.id()->idn() << "_"; }
      else if (vd.id()->v().size() != 0){ os << vd.id()->v(); }
      string name = os.str();
      return name;
  }

  /** Create a string representing the name (and unique identifier) of a constraint from a specific call expression. */
  string NLFile::get_cname(const Call& c){
      stringstream os;
      os << c.id() << "_" << static_cast<const void*>(&c); // use the memory address as unique ID.
      string name = os.str();
      return name;
  }

  /** Obtain the vector of an array, either from an identifier or an array litteral */
  const ArrayLit& NLFile::get_arraylit(const Expression* e){
      switch(e->eid()){
          case Expression::E_ID: {
              return get_arraylit( e->cast<Id>()->decl()->e() ); // Follow the pointer to the expression of the declaration
          }

          case Expression::E_ARRAYLIT: {
              const ArrayLit& al = *e->cast<ArrayLit>();
              return al;
          }
      }
      cerr << "Could not read array from expression." << endl;
      assert(false);
  }

  /** Create a vector of double from a vector containing Expression being IntLit. */
  vector<double> NLFile::from_vec_int(const ArrayLit& v_int){
    vector<double> v = {};
    for(unsigned int i=0; i<v_int.size(); ++i){
      double d  = v_int[i]->cast<IntLit>()->v().toInt();
      v.push_back(d);
    }
    return v;
  }

  /** Create a vector of double from a vector containing Expression being FloatLit. */
  vector<double> NLFile::from_vec_fp(const ArrayLit& v_fp){
    vector<double> v = {};
    for(unsigned int i=0; i<v_fp.size(); ++i){
      double d  = v_fp[i]->cast<FloatLit>()->v().toDouble();
      v.push_back(d);
    }
    return v;
  }

  /** Create a vector of variable names from a vector containing Expression being identifier Id. */
  vector<string> NLFile::from_vec_id(const ArrayLit& v_id){
    vector<string> v = {};
    for(unsigned int i=0; i<v_id.size(); ++i){
      string s  = get_vname(*(v_id[i]->cast<Id>()->decl()));
      v.push_back(s);
    }
    return v;
  }

  /** Create a token from an expression representing either a variable or an integer numeric value.
   * ONLY USE FOR CONSTRAINT, NOT OBJECTIVE!
   */
  NLToken NLFile::get_tok_var_int(const Expression* e, NLFile* nl_file){
    if(e->type().ispar()){
      // Constant
      long long value = e->cast<IntLit>()->v().toInt();
      return NLToken::n(value);
    } else {
      // Variable
      VarDecl& vd = *(e->cast<Id>()->decl());
      string n = get_vname(vd);
      return NLToken::vc(n, nl_file);
    }
  }




  /** *** *** *** Phase 1: collecting data from MZN *** *** *** **/

  /** Add a solve goal in the NL File. In our case, we can only have one and only one solve goal. */
  void NLFile::add_solve(SolveI::SolveType st, const Expression* e){

      // We can only have one objective. Prevent override.
      assert(!objective.is_defined());
      
      switch(st){
          case SolveI::SolveType::ST_SAT:{
              // Satisfy: implemented by minimizing 0 (print n0 for an empty expression graph)
              objective.minmax = objective.SATISFY;
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

      // Ensure that the obejctive is now defined.
      assert(objective.is_defined());
  }


  /** Add a variable declaration in the NL File.
   *  This function pre-analyse the declaration VarDecl, then delegate to add_vdecl_integer or add_vdecl_fp.
   *  In flatzinc, arrays always have a rhs: the can always be replaced by their definition (following the pointer starting at the ID)
   *  Hence, we do not reproduce arrays in the NL file.
   *  However, we have to take into account the output variables (TODO) */
  void NLFile::add_vdecl(const VarDecl &vd, const TypeInst &ti, const Expression &rhs){      
    // Get the name
    string name = get_vname(vd);
    // Discriminate according to the type:
    if(ti.isEnum()){
      cerr << "Should not happen" << endl;
      assert(false);
    }
    else if(ti.isarray()){
      cerr << "Definition of array " << name << " is not reproduced in nl.";
      // Gather output variable
      if(vd.ann().containsCall(constants().ann.output_array)){
        // TODO : For now, just x = [ list... ]
      }
    } else {
      // Check if the variable needs to be reported
      bool to_report = vd.ann().contains(constants().ann.output_var);
      cerr << "'" << name << "' to be reported? " << to_report << " ";

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
          add_vdecl_integer(name, isv, to_report);
      } else {
          // Floating point
          FloatSetVal* fsv = NULL;
          if(domain!=NULL){fsv = domain->cast<SetLit>()->fsv();}
          add_vdecl_fp(name, fsv, to_report);
      }
    }
  }

  /** Add an integer variable declaration to the NL File. */
  void NLFile::add_vdecl_integer(const string& name, const IntSetVal* isv, bool to_report){
    // Check that we do not have naming conflict
    assert(variables.find(name) == variables.end());
    // Check the domain.
    NLBound bound;
    if(isv == NULL){
      bound = NLBound::make_nobound();
    } else if (isv->size()==1){
      long long lb = isv->min(0).toInt();
      long long ub = isv->max(0).toInt();
      bound = NLBound::make_bounded(lb, ub);
    } else {
      cerr << "Should not happen: switch on mzn_opt_only_range_domains" << endl; assert(false);
    }
    // Create the variable and update the NLFile
    NLVar v = NLVar(name, true, to_report, bound);
    variables[name] = v;
  }

  /** Add a floating point variable declaration to the NL File. */
  void NLFile::add_vdecl_fp(const string& name, const FloatSetVal* fsv, bool to_report) {
    // Check that we do not have naming conflict
    assert(variables.find(name) == variables.end());
    // Check the domain.
    NLBound bound;
    if(fsv == NULL){
      bound = NLBound::make_nobound();
    } else if (fsv->size()==1){
      double lb = fsv->min(0).toDouble();
      double ub = fsv->max(0).toDouble();
      bound = NLBound::make_bounded(lb, ub);
    } else {
      cerr << "Should not happen: switch on mzn_opt_only_range_domains" << endl; assert(false);
    }
    // Create the variable and update the NLFile
    NLVar v = NLVar(name, false, to_report, bound);
    variables[name] = v;
  }

  /** Dispatcher for constraint analysis. */
  void NLFile::analyse_constraint(const Call& c){

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


  /* *** *** *** Integer Constraint methods *** *** *** */
  // WARGNING: when building expression graph in the constraints below, create the variable with vc and not vo!
  // 'vc' will update the internal flag of variable for non linear constraints, while 'vo' is doing the same for the objective.


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
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_int(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    long long       value   = c.arg(2)->cast<IntLit>()->v().toInt();

    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;
    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;
    // Create the bound of the constraint
    NLBound bound = NLBound::make_equal(value);
    cons.range = bound;
    // No non linear part: leave the expression graph empty.
    // Linear part: set the jacobian
    cons.set_jacobian(vars, coeffs, this);

    // Add the constraint in our mapping
    constraints[cname] = cons;
  }


  /** Linar constraint: [array0] *+ [array1] =< value **/
  void NLFile::consint_lin_le(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_int(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    long long       value   = c.arg(2)->cast<IntLit>()->v().toInt();

    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;
    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;
    // Create the bound of the constraint
    NLBound bound = NLBound::make_ub_bounded(value);
    cons.range = bound;
    // No non linear part: leave the expression graph empty.
    // Linear part: set the jacobian
    cons.set_jacobian(vars, coeffs, this);

    // Add the constraint in our mapping
    constraints[cname] = cons;
  }

  /** Linar constraint: [array0] *+ [array1] != value
   *  Translated into a logical constraint, not an algebraic one!
   */
  void NLFile::consint_lin_ne(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_int(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    long long       value   = c.arg(2)->cast<IntLit>()->v().toInt();

    // Create the Logical Constraint and set the data
    NLLogicalCons cons(logical_constraints.size());
    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;
    // Create the expression graph
    // 1) Push the comparison  "!= operand1 operand2"
    cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::NE));
    // 2) Operand1 := sum of product
    // WARNING: OPSUMLIST needs 3 operands and more!
    // 2.1) With two operands
    if(coeffs.size()==2){
      cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::OPPLUS));
    } else {
      // 2.2) With more operands
      cons.expression_graph.push_back(NLToken::mo(NLToken::MOpCode::OPSUMLIST, coeffs.size()));
    }
    // 2.3) All the sums in one go
    for(unsigned int i=0; i<coeffs.size(); ++i){
      // Product if coeff !=1
      if(coeffs[i]!=1){
        cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::OPMULT));
        cons.expression_graph.push_back(NLToken::n(coeffs[i]));
      }
      cons.expression_graph.push_back(NLToken::vc(vars[i], this));  // Use 'vc' here !
    }
    // 3) Operand 2 := value
    cons.expression_graph.push_back(NLToken::n(value));

    // Store the constraint
    logical_constraints.push_back(cons);
  }

  // --- --- --- Non Linear Constraints: operations
  // Flatzinc:
  // For a call c, we have 3 arguments:
  // c.arg(0) and c.arg(1) are the operands
  // c.arg(2) is the value to which it is compared.
  // The arguments are either variable or parameter

  /** Non linear constraint: x * y = z **/
  void NLFile::consint_times(const Call& c){
    NLToken t0     = get_tok_var_int(c.arg(0), this);
    NLToken t1     = get_tok_var_int(c.arg(1), this);
    NLToken tvalue = get_tok_var_int(c.arg(2), this);
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;
    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;
    // Create the bound of the constraint
    NLBound bound = NLBound::make_equal(0);
    cons.range = bound;
    // Create the expression graph: multiply the tokens t0 and t1 and substract tvalue
    cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::OPMINUS));
    cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::OPMULT));
    cons.expression_graph.push_back(t0);
    cons.expression_graph.push_back(t1);
    cons.expression_graph.push_back(tvalue);
    // Store the constraint
    constraints[cname] = cons;
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

  /** Non linear constraint x =< y translated as x-y =< 0 **/
  void NLFile::consint_le(const Call& c){
    NLToken t0     = get_tok_var_int(c.arg(0), this);
    NLToken t1     = get_tok_var_int(c.arg(1), this);
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;
    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;
    // Create the bound of the constraint: equal 0
    NLBound bound = NLBound::make_ub_bounded(0);
    cons.range = bound;
    // Create the expression graph: substract the two variables
    cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::OPMINUS));
    cons.expression_graph.push_back(t0);
    cons.expression_graph.push_back(t1);
    // Store the constraint
    constraints[cname] = cons;
  }


  /** Non linear constraint x = y
   *  arg(0) should be variable
   *  arg(1) could be a var or a constant
   ***/
  void NLFile::consint_eq(const Call& c){    
    Expression* arg0 = c.arg(0);
    Expression* arg1 = c.arg(1);

    // Get first arg
    if(arg0->type().ispar()){
      cerr << "Comparison: first argument shoud be a variable"; assert(false);
    }
    VarDecl& vd0 = *(arg0->cast<Id>()->decl());
    string n0 = get_vname(vd0);

    // Get second arg. If constant, update the bound on the variable, else create a constraint.
    if(arg1->type().ispar()){
      // Update the bound of the variable represented by arg0  
      long long value = arg1->cast<IntLit>()->v().toInt();
      NLVar& v = variables.at(n0);
      v.bound = NLBound::make_equal(value);
    } else {
      // Create a constraint between variables: soustraction of the two members=0
      VarDecl& vd1 = *(arg1->cast<Id>()->decl());
      string n1 = get_vname(vd1);
      // Create the Algebraic Constraint and set the data
      NLAlgCons cons;
      // Get the name of the constraint
      string cname = get_cname(c);
      cons.name = cname;
      // Create the bound of the constraint: equal 0
      NLBound bound = NLBound::make_equal(0);
      cons.range = bound;
      // Create the expression graph: substract the two variables
      cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::OPMINUS));
      cons.expression_graph.push_back(NLToken::vc(n0, this));
      cons.expression_graph.push_back(NLToken::vc(n1, this));
      // Store the constraint
      constraints[cname] = cons;
    }
  }

  /** Non linear constraint x != y **/
  void NLFile::consint_ne(const Call& c){
    NLToken t0     = get_tok_var_int(c.arg(0), this);
    NLToken t1     = get_tok_var_int(c.arg(1), this);

    // Create the Logical Constraint and set the data
    NLLogicalCons cons(logical_constraints.size());
    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;
    // Create the expression graph
    cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::NE));
    cons.expression_graph.push_back(t0);
    cons.expression_graph.push_back(t1);

    // Store the constraint
    logical_constraints.push_back(cons);
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



  /* *** *** *** Phase 2: processing *** *** *** */

  void NLFile::phase_2(){

    // --- --- --- Variables ordering and indexing
    for (auto const& name_var : variables){
      const NLVar &v = name_var.second;

      // Accumulate jacobian count
      _jacobian_count += v.jacobian_count;



      // First check non linear variables in BOTH objective and constraint.
      if(v.is_in_nl_objective && v.is_in_nl_constraint){
        if(v.is_integer){
          vname_nliv_both.push_back(v.name);
        } else {
          vname_nlcv_both.push_back(v.name);
        }
      }
      // Variables in non linear constraint ONLY
      else if(!v.is_in_nl_objective && v.is_in_nl_constraint){
        if(v.is_integer){
          vname_nliv_cons.push_back(v.name);
        } else {
          vname_nlcv_cons.push_back(v.name);
        }
      }
      // Variables in non linear objective ONLY
      else if(v.is_in_nl_objective && !v.is_in_nl_constraint){
        if(v.is_integer){
          vname_nliv_obj.push_back(v.name);
        } else {
          vname_nlcv_obj.push_back(v.name);
        }
      }
      // Variables not appearing nonlinearly
      else if(!v.is_in_nl_objective && !v.is_in_nl_constraint){
        if(v.is_integer){
          vname_liv_all.push_back(v.name);
        } else {
          vname_lcv_all.push_back(v.name);
        }
      }
      // Should not happen
      else { cerr << "Should not happen" << endl; assert(false);}

    }

    // Note:  In the above, we dealt with all 'vname_*' vectors BUT 'vname_larc_all' and 'vname_bv_all'
    //        networks and boolean are not implemented. Nevertheless, we keep the vectors and deal with
    //        them below to ease further implementations.

    vnames.reserve(variables.size());

    vnames.insert(vnames.end(), vname_nlcv_both.begin(),  vname_nlcv_both.end());
    vnames.insert(vnames.end(), vname_nliv_both.begin(),  vname_nliv_both.end());
    vnames.insert(vnames.end(), vname_nlcv_cons.begin(),  vname_nlcv_cons.end());
    vnames.insert(vnames.end(), vname_nliv_cons.begin(),  vname_nliv_cons.end());
    vnames.insert(vnames.end(), vname_nlcv_obj.begin(),   vname_nlcv_obj.end());
    vnames.insert(vnames.end(), vname_nliv_obj.begin(),   vname_nliv_obj.end());
    vnames.insert(vnames.end(), vname_larc_all.begin(),   vname_larc_all.end());
    vnames.insert(vnames.end(), vname_lcv_all.begin(),    vname_lcv_all.end());
    vnames.insert(vnames.end(), vname_bv_all.begin(),     vname_bv_all.end());
    vnames.insert(vnames.end(), vname_liv_all.begin(),    vname_liv_all.end());

    // Create the mapping name->index
    for(int i=0; i<vnames.size(); ++i){
      variable_indexes[vnames[i]] = i;
    }

    // --- --- --- Constraint ordering, couting, and indexing
    for (auto const& name_cons : constraints){
      const NLAlgCons &c = name_cons.second;

      // Sort by linearity. We do not have network constraint.
      if(c.is_linear()){
        cnames_lin_general.push_back(c.name);
      } else {
        cnames_nl_general.push_back(c.name);
      }

      // Count the number of ranges and eqns constraints
      switch(c.range.tag){
        case NLBound::LB_UB:{ ++nb_alg_cons_range; }
        case NLBound::EQ:{ ++nb_alg_cons_eq; }
      }
    }

    cnames.reserve(constraints.size());
    cnames.insert(cnames.end(), cnames_nl_general.begin(), cnames_nl_general.end());
    cnames.insert(cnames.end(), cnames_nl_network.begin(), cnames_nl_network.end());
    cnames.insert(cnames.end(), cnames_lin_network.begin(), cnames_lin_network.end());
    cnames.insert(cnames.end(), cnames_lin_general.begin(), cnames_lin_general.end());

    // Create the mapping name->index
    for(int i=0; i<cnames.size(); ++i){
      constraint_indexes[cnames[i]] = i;
    }


  }



  // --- --- --- Counts

  /** Jacobian count. */
  int NLFile::jacobian_count() const {
    return _jacobian_count;
  }

  /** Total number of variables. */
  int NLFile::n_var() const {
    return variables.size();
  }

  /** Number of variables appearing nonlinearly in constraints. */
  int NLFile::nlvc() const {
    // Variables in both + variables in constraint only (integer+continuous)
    return nlvb()+vname_nliv_cons.size()+vname_nlcv_cons.size();
  }

  /** Number of variables appearing nonlinearly in objectives. */
  int NLFile::nlvo() const {
    // Variables in both + variables in objective only (integer+continuous)
    return nlvb()+vname_nliv_obj.size()+vname_nlcv_obj.size();
  }

  /** Number of variables appearing nonlinearly in both constraints and objectives.*/
  int NLFile::nlvb() const { return vname_nlcv_both.size() + vname_nliv_both.size();  }

  /** Number of integer variables appearing nonlinearly in both constraints and objectives.*/        
  int NLFile::nlvbi() const { return vname_nliv_both.size(); }

  /** Number of integer variables appearing nonlinearly in constraints **only**.*/        
  int NLFile::nlvci() const { return vname_nliv_cons.size(); }

  /** Number of integer variables appearing nonlinearly in objectives **only**.*/        
  int NLFile::nlvoi() const { return vname_nliv_obj.size(); }

  /** Number of linear arcs. Network nor implemented, so always 0.*/
  int NLFile::nwv() const { return vname_larc_all.size(); }

  /** Number of "other" integer variables.*/
  int NLFile::niv() const { return vname_liv_all.size();}

  /** Number of binary variables.*/        
  int NLFile::nbv() const { return vname_bv_all.size(); }
































    /** *** *** *** Printable Interface *** *** *** **/

    // Note:  * empty line not allowed
    //        * comment only not allowed
    ostream& NLFile::print_on(ostream& os) const{
      // Print the header
      {NLHeader header; header.print_on(os, *this);}
      os << endl;
      
      // Print the unique segments about the variables
      if(n_var()>1){

        // Print the 'k' segment Maybe to adjust with the presence of 'J' segments
        os << "k" << (n_var()-1) << "   # Cumulative Sum of non-zero in the jacobian matrix's (nbvar-1) columns." << endl;
        int acc = 0;
        // Note stop before the last var. Total jacobian count is in the header.
        for(int i=0; i<n_var()-1; ++i){
          string name = vnames[i];
          acc += variables.at(name).jacobian_count;
          os << acc << "   # " << name << endl;
        }

        // Print the 'b' segment
        os << "b   # Bounds on variables (" << n_var() << ")" << endl;
        for(auto n: vnames){
          NLVar v = variables.at(n);
          v.bound.print_on(os, n);
          os << endl;
        }
      }

      // Print the unique segments about the constraints
      if(!cnames.empty()){
        // Create the 'r' range segment per constraint
        // For now, it is NOT clear if the network constraint should appear in the range constraint or not.
        // To be determine if later implemented.
        os << "r   # Bounds on algebraic constraint bodies (" << cnames.size() << ")" << endl;
        for(auto n: cnames){
          NLAlgCons c = constraints.at(n);
          c.range.print_on(os, n);
          os << endl;
        }
      }

      // Print the Algebraic Constraints
      for(auto n:cnames){
        NLAlgCons c = constraints.at(n);
        c.print_on(os, *this);
      }

      // Print the Logical constraint
      for(auto& lc: logical_constraints){
        lc.print_on(os, *this);
      }

      // Print the objective
      objective.print_on(os, *this);

      return os;
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