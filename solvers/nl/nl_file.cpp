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



  /** *** *** *** Phase 1: collecting data from MZN *** *** *** **/

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



  // --- --- --- Constraints analysis

  /** Dispatcher for constraint analysis. */
  void NLFile::analyse_constraint(const Call& c){

      // ID of the call
      auto id = c.id();
      // Constants for integer builtins
      auto consint = constants().ids.int_;
      // Constants for floating point builtins
      auto consfp = constants().ids.float_;

      // Integer linear predicates
      if(id == consint.lin_eq){       consint_lin_eq(c);  }
      else if(id == consint.lin_le){  consint_lin_le(c);  }
      else if(id == consint.lin_ne){  consint_lin_ne(c);  }
      
      // Integer predicates
      else if(id == consint.le){      consint_le(c);      }
      else if(id == consint.eq){      consint_eq(c);      }
      else if(id == consint.ne){      consint_ne(c);      }

      // Integer binary operators
      else if(id == consint.times){   consint_times(c);   }
      else if(id == consint.div){     consint_div(c);     }
      else if(id == consint.mod){     consint_mod(c);     }
      else if(id == consint.plus){    consint_plus(c);    }
      else if(id == "int_pow"){       int_pow(c);         }
      else if(id == "int_max"){       int_max(c);         }
      else if(id == "int_min"){       int_min(c);         }

      // Integer unary operators
      else if(id == "int_abs"){       int_abs(c);         }

      // Floating point linear predicates
      else if(id == consfp.lin_eq){   consfp_lin_eq(c);   }
      else if(id == consfp.lin_le){   consfp_lin_le(c);   }
      else if(id == consfp.lin_lt){   consfp_lin_lt(c);   }
      else if(id == consfp.lin_ne){   consfp_lin_ne(c);   }

      // Floating point predicates
      else if(id == consfp.lt){       consfp_lt(c);       }
      else if(id == consfp.le){       consfp_le(c);       }
      else if(id == consfp.eq){       consfp_eq(c);       }
      else if(id == consfp.ne){       consfp_ne(c);       }

      // Floating point binary operators
      else if(id == consfp.plus){     consfp_plus(c);     }
      else if(id == consfp.minus){    consfp_minus(c);    }
      else if(id == consfp.div){      consfp_div(c);      }
      else if (id =="float_pow"){     float_pow(c);       }
      else if (id =="float_max"){     float_max(c);       }
      else if (id =="float_min"){     float_min(c);       }

      // Floating point unary operators
      else if (id =="float_abs"){     float_abs(c);       }
      else if (id =="float_acos"){    float_acos(c);      }
      else if (id =="float_acosh"){   float_acosh(c);     }
      else if (id =="float_asin"){    float_asin(c);      }
      else if (id =="float_asinh"){   float_asinh(c);     }
      else if (id =="float_atan"){    float_atan(c);      }
      else if (id =="float_atanh"){   float_atanh(c);     }
      else if (id =="float_cos"){     float_cos(c);       }
      else if (id =="float_cosh"){    float_cosh(c);      }
      else if (id =="float_exp"){     float_exp(c);       }
      else if (id =="float_ln"){      float_ln(c);        }
      else if (id =="float_log10"){   float_log10(c);     }
      else if (id =="float_log2"){    float_log2(c);      }
      else if (id =="float_sqrt"){    float_sqrt(c);      }
      else if (id =="float_sin"){     float_sin(c);       }
      else if (id =="float_sinh"){    float_sinh(c);      }
      else if (id =="float_tan"){     float_tan(c);       }
      else if (id =="float_tanh"){    float_tanh(c);      }

      // Other
      else if(id == "int2float"){     int2float(c);       }

      // Domain
      else if(id == consfp.in){       cerr << "Ignore for now: constraint 'float in    ' not implemented"; assert(false); }
      else if(id == consfp.dom){      cerr << "Ignore for now: constraint 'float dom   ' not implemented"; assert(false); }

      // Grey area
      else if(id == consint.lt){      cerr << "Should not happen 'int lt'";     assert(false); }
      else if(id == consint.gt){      cerr << "Should not happen 'int gt'";     assert(false); }
      else if(id == consint.ge){      cerr << "Should not happen 'int ge'";     assert(false); }
      else if(id == consint.minus){   cerr << "Should not happen 'int minus'";  assert(false); }

      else if(id == consfp.gt){       cerr << "Should not happen 'float gt'";   assert(false); }
      else if(id == consfp.ge){       cerr << "Should not happen 'float ge'";   assert(false); }
      else if(id == consfp.times){    consfp_times(c);    } // Should not happen?
      else if(id == consfp.mod){      consfp_mod(c);      } // Should not happen?

      // Not implemented
      else {
          cerr << "Builtins " << c.id() << " not implemented";
          assert(false);
      }
  }



  // --- --- --- Helpers

  /** Create a token from an expression representing a variable */
  NLToken NLFile::get_tok_var_int(const Expression* e){
    if(e->type().ispar()){
      // Constant
      long long value = e->cast<IntLit>()->v().toInt();
      return NLToken::n(value);
    } else {
      // Variable
      VarDecl& vd = *(e->cast<Id>()->decl());
      string n = get_vname(vd);
      return NLToken::v(n);
    }
  }

  /** Create a token from an expression representing either a variable or a floating point numeric value. */ 
  NLToken NLFile::get_tok_var_fp(const Expression* e){
    if(e->type().ispar()){
      // Constant
      double value = e->cast<FloatLit>()->v().toDouble();
      return NLToken::n(value);
    } else {
      // Variable
      VarDecl& vd = *(e->cast<Id>()->decl());
      string n = get_vname(vd);
      return NLToken::v(n);
    }
  }

  /** Create a token from an expression representing either a variable. */
  NLToken NLFile::get_tok_var(const Expression* e){
      assert(!e->type().ispar());
      // Variable
      VarDecl& vd = *(e->cast<Id>()->decl());
      string n = get_vname(vd);
      return NLToken::v(n);
  }

  /** Update an expression graph (only by appending token) with a linear combination
   *  of coefficients and variables. Count as "non linear" for the variables occuring here.
   */
  void NLFile::make_SigmaMult(vector<NLToken>& expression_graph, const vector<double>& coeffs, const vector<string>& vars){
    assert(coeffs.size()==vars.size());
    assert(coeffs.size()>=2);
    
    // Create a sum of products.
    // WARNING: OPSUMLIST needs at least 3 operands! We need a special case for the sum of 2.
    if(coeffs.size()==2){
      expression_graph.push_back(NLToken::o(NLToken::OpCode::OPPLUS));
    } else {
      expression_graph.push_back(NLToken::mo(NLToken::MOpCode::OPSUMLIST, coeffs.size()));
    }

    // Component of the sum. Simplify multiplication by one.
    for(unsigned int i=0; i<coeffs.size(); ++i){
      // Product if coeff !=1
      if(coeffs[i]!=1){
        expression_graph.push_back(NLToken::o(NLToken::OpCode::OPMULT));
        expression_graph.push_back(NLToken::n(coeffs[i]));
      }
      // Set the variable as "non linear"
      expression_graph.push_back(NLToken::v(vars[i]));
    }

  }



  // --- --- --- Linear Builders

  /** Create a linear constraint [coeffs] *+ [vars] = value. */
  void NLFile::lincons_eq(const Call& c, const vector<double>& coeffs, const vector<string>& vars, NLToken value){
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;

    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;

    if(value.is_constant()){
      // Create the bound of the constraint
      NLBound bound = NLBound::make_equal(value.numeric_value);
      cons.range = bound;
      // No non linear part: leave the expression graph empty.
      // Linear part: set the jacobian
      cons.set_jacobian(vars, coeffs, this);
    } else {
      // Create the bound of the constraint = 0 and change the Jacobian to incorporate a -1 on the variable in 'value'
      NLBound bound = NLBound::make_equal(0);
      cons.range = bound;
      // No non linear part: leave the expression graph empty.
      // Linear part: set the jacobian
      vector<double>  coeffs_(coeffs);
      coeffs_.push_back(-1);
      vector<string>  vars_(vars);
      vars_.push_back(value.str);
      cons.set_jacobian(vars_, coeffs_, this);
    }

    // Add the constraint in our mapping
    constraints[cname] = cons;
  }

  /** Create a linear constraint [coeffs] *+ [vars] <= value. */
  void NLFile::lincons_le(const Call& c, const vector<double>& coeffs, const vector<string>& vars, NLToken value){
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;

    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;

    if(value.is_constant()){
      // Create the bound of the constraint
      NLBound bound = NLBound::make_ub_bounded(value.numeric_value);
      cons.range = bound;
      // No non linear part: leave the expression graph empty.
      // Linear part: set the jacobian
      cons.set_jacobian(vars, coeffs, this);
    } else {
      // Create the bound of the constraint = 0 and change the Jacobian to incorporate a -1 on the variable in 'value'
      NLBound bound = NLBound::make_ub_bounded(0);
      cons.range = bound;
      // No non linear part: leave the expression graph empty.
      // Linear part: set the jacobian
      vector<double>  coeffs_(coeffs);
      coeffs_.push_back(-1);
      vector<string>  vars_(vars);
      vars_.push_back(value.str);
      cons.set_jacobian(vars_, coeffs_, this);
    }

    // Add the constraint in our mapping
    constraints[cname] = cons;
  }

  /** Create a linear logical constraint [coeffs] *+ [vars] PREDICATE value.
   *  Use a generic comparison operator.
   *  Warnings:   - Creates a logical constraint
   *              - Only use for conmparisons that cannot be expressed with '=' xor '<='.
   */
  void NLFile::lincons_predicate(const Call& c, NLToken::OpCode oc,
    const vector<double>& coeffs, const vector<string>& vars, NLToken value){

    // Create the Logical Constraint and set the data
    NLLogicalCons cons(logical_constraints.size());

    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;

    // Create the expression graph (Note: no Jacobian associated with a logical constraint).
    // 1) Push the comparison operator, e.g. "!= operand1 operand2"
    cons.expression_graph.push_back(NLToken::o(oc));

    // 2) Operand1 := sum of product
    make_SigmaMult(cons.expression_graph, coeffs, vars);

    // 3) Operand 2 := value.
    cons.expression_graph.push_back(value);

    // Store the constraint
    logical_constraints.push_back(cons);
  }



  // --- --- --- Non Linear Builders
  // For predicates, uses 2 variables or literals: x := c.arg(0), y := c.arg(1)
  // x PREDICATE y

  // For operations, uses 3 variables or literals: x := c.arg(0), y := c.arg(1), and z := c.arg(2).
  // x OPERATOR y = z

  /** Create a non linear constraint x = y
   *  Use the jacobian and the bound on constraint to translate into x - y = 0
   *  Simply update the bound if one is a constant.
   */
  void NLFile::nlcons_eq(const Call& c, NLToken x, NLToken y){
    if(x.kind != y.kind){
      if(x.is_constant()){
        // Update bound on y
        double value = x.numeric_value;
        NLVar& v = variables.at(y.str);
        v.bound.update_eq(value);
      } else {
        // Update bound on x
        double value = y.numeric_value;
        NLVar& v = variables.at(x.str);
        v.bound.update_eq(value);
      }
    } else if(x.str != y.str){ // both must be variables anyway.
      assert(x.is_variable());
      // Create the Algebraic Constraint and set the data
      NLAlgCons cons;

      // Get the name of the constraint
      string cname = get_cname(c);
      cons.name = cname;

      // Create the bound of the constraint: equal 0
      NLBound bound = NLBound::make_equal(0);
      cons.range = bound;

      // Create the jacobian 
      vector<double>  coeffs  = {1, -1};
      vector<string>  vars    = {x.str, y.str};
      cons.set_jacobian(vars, coeffs, this);

      // Store the constraint
      constraints[cname] = cons;
    }
  }

  /** Create a non linear constraint x <= y
   *  Use the jacobian and the bound on constraint to translate into x - y <= 0
   *  Simply update the bound if one is a constant.
   */
  void NLFile::nlcons_le(const Call& c, NLToken x, NLToken y){
    if(x.kind != y.kind){
      if(x.is_constant()){
        // Update lower bound on y
        double value = x.numeric_value;
        NLVar& v = variables.at(y.str);
        v.bound.update_lb(value);
      } else {
        // Update upper bound on x
        double value = y.numeric_value;
        NLVar& v = variables.at(x.str);
        v.bound.update_ub(value);
      }
    } else if(x.str != y.str){ // both must be variables anyway.
      assert(x.is_variable());

      // Create the Algebraic Constraint and set the data
      NLAlgCons cons;

      // Get the name of the constraint
      string cname = get_cname(c);
      cons.name = cname;

      // Create the bound of the constraint: <= 0
      NLBound bound = NLBound::make_ub_bounded(0);
      cons.range = bound;

      // Create the jacobian 
      vector<double>  coeffs  = {1, -1};
      vector<string>  vars    = {x.str, y.str};
      cons.set_jacobian(vars, coeffs, this);

      // Store the constraint
      constraints[cname] = cons;
    }
  }

  /** Create a non linear constraint with a predicate: x PREDICATE y
   *  Use a generic comparison operator.
   *  Warnings:   - Creates a logical constraint
   *              - Only use for conmparisons that cannot be expressed with '=' xor '<='.
   */
  void NLFile::nlcons_predicate(const Call& c, NLToken::OpCode oc, NLToken x, NLToken y){

    // Create the Logical Constraint and set the data
    NLLogicalCons cons(logical_constraints.size());

    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;

    // Create the expression graph
    cons.expression_graph.push_back(NLToken::o(oc));
    cons.expression_graph.push_back(x);
    cons.expression_graph.push_back(y);

    // Store the constraint
    logical_constraints.push_back(cons);
  }

  /** Create a non linear constraint with a binary operator: x OPERATOR y = z */
  void NLFile::nlcons_operator_binary(const Call& c, NLToken::OpCode oc, NLToken x, NLToken y, NLToken z){
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;

    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;

    // If z is a constant, use the bound, else use the jacobian
    if(z.is_constant()){
      // Create the bound of the constraint with the numeric value of z
      NLBound bound = NLBound::make_equal(z.numeric_value);
      cons.range = bound;
    } else {
      // Else, use a constraint bound = 0 and use the jacobian to substract vres from the result.
      // Create the bound of the constraint to 0
      NLBound bound = NLBound::make_equal(0);
      cons.range = bound;

      vector<double>  coeffs  = {};
      vector<string>  vars    = {};

      // Check that x is not y or z
      if(x.str!=y.str && x.str!=y.str){
        coeffs.push_back(0);
        vars.push_back(x.str);
      }
      // Check that y is not z
      if(y.str!=z.str){
        coeffs.push_back(0);
        vars.push_back(y.str);
      }
      // z is a variable whose value is substracted from the result
      coeffs.push_back(-1);
      vars.push_back(z.str);
      
      // Finish jacobian
      cons.set_jacobian(vars, coeffs, this);
    }

    // Create the expression graph using the operand code 'oc'
    cons.expression_graph.push_back(NLToken::o(oc));
    cons.expression_graph.push_back(x);
    cons.expression_graph.push_back(y);

    // Store the constraint
    constraints[cname] = cons;
  }

  /** Create a non linear constraint with a binary operator: x OPERATOR y = z.
   *  OPERATOR is now a Multiop, with a count of 2 (so the choice of the method to use depends on the LN implementation) */
  void NLFile::nlcons_operator_binary(const Call& c, NLToken::MOpCode moc, NLToken x, NLToken y, NLToken z){
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;

    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;

    // If z is a constant, use the bound, else use the jacobian
    if(z.is_constant()){
      // Create the bound of the constraint with the numeric value of z
      NLBound bound = NLBound::make_equal(z.numeric_value);
      cons.range = bound;
    } else {
      // Else, use a constraint bound = 0 and use the jacobian to substract vres from the result.
      // Create the bound of the constraint to 0
      NLBound bound = NLBound::make_equal(0);
      cons.range = bound;

      vector<double>  coeffs  = {};
      vector<string>  vars    = {};

      // Check that x is not y or z
      if(x.str!=y.str && x.str!=y.str){
        coeffs.push_back(0);
        vars.push_back(x.str);
      }
      // Check that y is not z
      if(y.str!=z.str){
        coeffs.push_back(0);
        vars.push_back(y.str);
      }
      // z is a variable whose value is substracted from the result
      coeffs.push_back(-1);
      vars.push_back(z.str);
      
      // Finish jacobian
      cons.set_jacobian(vars, coeffs, this);
    }

    // Create the expression graph using the operand code 'oc'
    cons.expression_graph.push_back(NLToken::mo(moc, 2));
    cons.expression_graph.push_back(x);
    cons.expression_graph.push_back(y);

    // Store the constraint
    constraints[cname] = cons;
  }


  /** Create a non linear constraint with an unary operator: OPERATOR x = y */
  void NLFile::nlcons_operator_unary(const Call& c, NLToken::OpCode oc, NLToken x, NLToken y){
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;

    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;

    // If y is a constant, use the bound, else use the jacobian
    if(y.is_constant()){
      // Create the bound of the constraint with the numeric value of y
      NLBound bound = NLBound::make_equal(y.numeric_value);
      cons.range = bound;
    } else {
      // Else, use a constraint bound = 0 and use the jacobian to substract vres from the result.
      // Create the bound of the constraint to 0
      NLBound bound = NLBound::make_equal(0);
      cons.range = bound;

      vector<double>  coeffs  = {};
      vector<string>  vars    = {};

      // Check that x is not y
      if(x.str!=y.str){
        coeffs.push_back(0);
        vars.push_back(x.str);
      }
      // z is a variable whose value is substracted from the result
      coeffs.push_back(-1);
      vars.push_back(y.str);
      
      // Finish jacobian
      cons.set_jacobian(vars, coeffs, this);
    }

    // Create the expression graph using the operand code 'oc'
    cons.expression_graph.push_back(NLToken::o(oc));
    cons.expression_graph.push_back(x);

    // Store the constraint
    constraints[cname] = cons;
  }

  /** Create a non linear constraint, specialized for log2 unary operator: Log2(x) = y */
  void NLFile::nlcons_operator_unary_log2(const Call& c, NLToken x, NLToken y){
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;

    // Get the name of the constraint
    string cname = get_cname(c);
    cons.name = cname;

    // If y is a constant, use the bound, else use the jacobian
    if(y.is_constant()){
      // Create the bound of the constraint with the numeric value of y
      NLBound bound = NLBound::make_equal(y.numeric_value);
      cons.range = bound;
    } else {
      // Else, use a constraint bound = 0 and use the jacobian to substract vres from the result.
      // Create the bound of the constraint to 0
      NLBound bound = NLBound::make_equal(0);
      cons.range = bound;

      vector<double>  coeffs  = {};
      vector<string>  vars    = {};

      // Check that x is not y
      if(x.str!=y.str){
        coeffs.push_back(0);
        vars.push_back(x.str);
      }
      // z is a variable whose value is substracted from the result
      coeffs.push_back(-1);
      vars.push_back(y.str);
      
      // Finish jacobian
      cons.set_jacobian(vars, coeffs, this);
    }

    // Create the expression graph with log2(x) = ln(x)/ln(2) with ln(2) = 0.693147180559945309417232121458176568075
    // For a double, the number of significant number is between 15 and 17, so we use 17...
    cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::OPDIV));
    cons.expression_graph.push_back(NLToken::o(NLToken::OpCode::OP_log));
    cons.expression_graph.push_back(x);
    cons.expression_graph.push_back(NLToken::n(0.69314718055994530));


    // Store the constraint
    constraints[cname] = cons;
  }





  // --- --- --- Integer Linear Constraints

  /** Linar constraint: [coeffs] *+ [vars] = value */
  void NLFile::consint_lin_eq(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_int(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    NLToken         value   = get_tok_var_int(c.arg(2));
    // Create the constraint
    lincons_eq(c, coeffs, vars, value);
  }

  /** Linar constraint: [coeffs] *+ [vars] =< value */
  void NLFile::consint_lin_le(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_int(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    NLToken         value   = get_tok_var_int(c.arg(2));
    // Create the constraint
    lincons_le(c, coeffs, vars, value);
  }

  /** Linar constraint: [coeffs] *+ [vars] != value */
  void NLFile::consint_lin_ne(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_int(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    NLToken         value   = get_tok_var_int(c.arg(2));
    // Create the constraint
    lincons_predicate(c, NLToken::OpCode::NE, coeffs, vars, value);
  }



  // --- --- --- Integer Non Linear Predicate Constraints

  /** Non linear constraint x = y */
  void NLFile::consint_eq(const Call& c){    
    nlcons_eq(c, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)));
  }

  /** Non linear constraint x <= y */
  void NLFile::consint_le(const Call& c){    
    nlcons_le(c, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)));
  }

  /** Non linear constraint x != y */
  void NLFile::consint_ne(const Call& c){
    nlcons_predicate(c, NLToken::OpCode::NE, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)));
  }



  // --- --- --- Integer Non Linear Binary Operator Constraints

  /** Non linear constraint x + y = z */
  void NLFile::consint_plus(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPPLUS, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)), get_tok_var_int(c.arg(2)));
  }

  /** Non linear constraint x * y = z */
  void NLFile::consint_times(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPMULT, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)), get_tok_var_int(c.arg(2)));
  }

  /** Non linear constraint x / y = z */
  void NLFile::consint_div(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPDIV, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)), get_tok_var_int(c.arg(2)));
  }

  /** Non linear constraint x mod y = z */
  void NLFile::consint_mod(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPREM, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)), get_tok_var_int(c.arg(2)));
  }

  /** Non linear constraint x pow y = z */
  void NLFile::int_pow(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPPOW, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)), get_tok_var_int(c.arg(2)));
  }

  /** Non linear constraint max(x, y) = z */
  void NLFile::int_max(const Call& c){
    nlcons_operator_binary(c, NLToken::MOpCode::MAXLIST, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)), get_tok_var_int(c.arg(2)));
  }

  /** Non linear constraint min(x, y) = z */
  void NLFile::int_min(const Call& c){
    nlcons_operator_binary(c, NLToken::MOpCode::MINLIST, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)), get_tok_var_int(c.arg(2)));
  }




  // --- --- --- Integer Non Linear Unary Operator Constraints

  /** Non linear constraint abs x = y */
  void NLFile::int_abs(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::ABS, get_tok_var_int(c.arg(0)), get_tok_var_int(c.arg(1)));
  }



  // --- --- --- Floating Point Linear Constraints

  /** Linar constraint: [coeffs] *+ [vars] = value */
  void NLFile::consfp_lin_eq(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_fp(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    NLToken         value   = get_tok_var_fp(c.arg(2));
    // Create the constraint
    lincons_eq(c, coeffs, vars, value);
  }

  /** Linar constraint: [coeffs] *+ [vars] = value */
  void NLFile::consfp_lin_le(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_fp(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    NLToken         value   = get_tok_var_fp(c.arg(2));
    // Create the constraint
    lincons_le(c, coeffs, vars, value);
  }

  /** Linar constraint: [coeffs] *+ [vars] != value */
  void NLFile::consfp_lin_ne(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_fp(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    NLToken         value   = get_tok_var_fp(c.arg(2));
    // Create the constraint
    lincons_predicate(c, NLToken::OpCode::NE, coeffs, vars, value);
  }

  /** Linar constraint: [coeffs] *+ [vars] < value */
  void NLFile::consfp_lin_lt(const Call& c){
    // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
    vector<double>  coeffs  = from_vec_fp(get_arraylit(c.arg(0)));
    vector<string>  vars    = from_vec_id(get_arraylit(c.arg(1)));
    NLToken         value   = get_tok_var_fp(c.arg(2));
    // Create the constraint
    lincons_predicate(c, NLToken::OpCode::LT, coeffs, vars, value);
  }  



  // --- --- --- Floating Point Non Linear Predicate Constraints

  /** Non linear constraint x = y */
  void NLFile::consfp_eq(const Call& c){    
    nlcons_eq(c, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint x <= y */
  void NLFile::consfp_le(const Call& c){    
    nlcons_le(c, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint x != y */
  void NLFile::consfp_ne(const Call& c){
    nlcons_predicate(c, NLToken::OpCode::NE, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint x < y */
  void NLFile::consfp_lt(const Call& c){
    nlcons_predicate(c, NLToken::OpCode::LT, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }



  // --- --- --- Floating Point Non Linear Binary Operator Constraints

  /** Non linear constraint x + y = z */
  void NLFile::consfp_plus(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPPLUS, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)), get_tok_var_fp(c.arg(2)));
  }

  /** Non linear constraint x - y = z */
  void NLFile::consfp_minus(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPMINUS, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)), get_tok_var_fp(c.arg(2)));
  }

  /** Non linear constraint x * y = z */
  void NLFile::consfp_times(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPMULT, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)), get_tok_var_fp(c.arg(2)));
  }

  /** Non linear constraint x / y = z */
  void NLFile::consfp_div(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPDIV, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)), get_tok_var_fp(c.arg(2)));
  }

  /** Non linear constraint x mod y = z */
  void NLFile::consfp_mod(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPREM, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)), get_tok_var_fp(c.arg(2)));
  }

  /** Non linear constraint x pow y = z */
  void NLFile::float_pow(const Call& c){
    nlcons_operator_binary(c, NLToken::OpCode::OPPOW, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)), get_tok_var_fp(c.arg(2)));
  }

  /** Non linear constraint max(x, y) = z */
  void NLFile::float_max(const Call& c){
    nlcons_operator_binary(c, NLToken::MOpCode::MAXLIST, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)), get_tok_var_fp(c.arg(2)));
  }

  /** Non linear constraint min(x, y) = z */
  void NLFile::float_min(const Call& c){
    nlcons_operator_binary(c, NLToken::MOpCode::MINLIST, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)), get_tok_var_fp(c.arg(2)));
  }




  // --- --- --- Floating Point Non Linear Unary operator Constraints

  /** Non linear constraint abs x = y */
  void NLFile::float_abs(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::ABS, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint acos x = y */
  void NLFile::float_acos(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_acos, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint acosh x = y */
  void NLFile::float_acosh(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_acosh, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint asin x = y */
  void NLFile::float_asin(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_asin, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint asinh x = y */
  void NLFile::float_asinh(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_asinh, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint atan x = y */
  void NLFile::float_atan(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_atan, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint atanh x = y */
  void NLFile::float_atanh(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_atanh, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint cos x = y */
  void NLFile::float_cos(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_cos, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint cosh x = y */
  void NLFile::float_cosh(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_cosh, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint exp x = y */
  void NLFile::float_exp(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_exp, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint ln x = y */
  void NLFile::float_ln(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_log, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint log10 x = y */
  void NLFile::float_log10(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_log10, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint log2 x = y */
  void NLFile::float_log2(const Call& c){
    nlcons_operator_unary_log2(c, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint sqrt x = y */
  void NLFile::float_sqrt(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_sqrt, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint sin x = y */
  void NLFile::float_sin(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_sin, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint sinh x = y */
  void NLFile::float_sinh(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_sinh, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint tan x = y */
  void NLFile::float_tan(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_tan, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }

  /** Non linear constraint tanh x = y */
  void NLFile::float_tanh(const Call& c){
    nlcons_operator_unary(c, NLToken::OpCode::OP_tanh, get_tok_var_fp(c.arg(0)), get_tok_var_fp(c.arg(1)));
  }



  // --- --- --- Other

  /** Integer x to floating point y. Constraint x = y translated into x - y = 0.
   *  Simulate a linear constraint [1, -1]+*[x, y] = 0
   */
  void NLFile::int2float(const Call& c){
    vector<double> coeffs = {1, -1};
    vector<string> vars = {};
    vars.push_back(get_tok_var(c.arg(0)).str);
    vars.push_back(get_tok_var(c.arg(1)).str);
    // Create the constraint
    lincons_eq(c, coeffs, vars, NLToken::n(0));
  }






  // --- --- --- Objective


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
              // Maximize an objective represented by a variable
              objective.minmax = objective.MINIMIZE;
              string v = get_tok_var(e).str;
              // Use the gradient
              vector<double>  coeffs  = {1};
              vector<string>  vars    = {v};
              objective.set_gradient(vars, coeffs);
              break;
              break;
          }
          case SolveI::SolveType::ST_MAX:{
              // Maximize an objective represented by a variable
              objective.minmax = objective.MAXIMIZE;
              string v = get_tok_var(e).str;
              // Use the gradient
              vector<double>  coeffs  = {1};
              vector<string>  vars    = {v};
              objective.set_gradient(vars, coeffs);
              break;
          }
      }


      // Ensure that the obejctive is now defined.
      assert(objective.is_defined());
  }




  /* *** *** *** Phase 2: processing *** *** *** */

  void NLFile::phase_2(){

    // --- --- --- Go over all constraint (algebraic AND logical) and mark non linear variables
    for(auto const& n_c: constraints){
      for(auto const& tok: n_c.second.expression_graph){
        if(tok.is_variable()){
          variables.at(tok.str).is_in_nl_constraint = true;
        }
      }
    }

    for(auto const& c: logical_constraints){
      for(auto const& tok: c.expression_graph){
        if(tok.is_variable()){
          variables.at(tok.str).is_in_nl_constraint = true;
        }
      }
    }

    // --- --- --- Go over the objective abd mark non linear variables
    for(auto const& tok: objective.expression_graph){
        if(tok.is_variable()){
          variables.at(tok.str).is_in_nl_objective = true;
        }
    }


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

