/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flatten.hh>
#include <minizinc/MIPdomains.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/copy.hh>
#include <minizinc/hash.hh>
#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>

#include <minizinc/stl_map_set.hh>

#include <minizinc/flatten_internal.hh>

// temporary
#include <minizinc/prettyprinter.hh>
//#include <ostream>

//#include <map>

/// TO CONSIDER and TODO
/// set_in etc. are not propagated between views
/// CLEANUP after work: ~destructor
/// Also check initexpr of all vars?  TODO

namespace MiniZinc {
  class MIPD {  
  public:
    MIPD(Env* env) : __env(env) { getEnv(); }
    void MIPdomains() {
      registerLinearConstraints();
      register__POSTconstraints();
      register__POSTvariables();
      constructVarViewCliques();
      printStats(std::cerr);
    }
    
  private:
    
    Env* __env=0;
    Env* getEnv() { assert(__env); return __env; }
    
    FunctionI *int_lin_eq;
    FunctionI *int_lin_le;
    FunctionI *float_lin_eq;
    FunctionI *float_lin_le;
    FunctionI *int2float;
    FunctionI *lin_exp_int;
    FunctionI *lin_exp_float;
    std::vector<Type> int_lin_eq_t = {Type::parint(1), Type::varint(1), Type::parint()};
    std::vector<Type> float_lin_eq_t = {Type::parfloat(1), Type::varfloat(1), Type::parfloat()};
    std::vector<Type> t_VIVF = { Type::varint(), Type::varfloat() };
      
    void registerLinearConstraints()
    {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      
      int_lin_eq = env.orig->matchFn(env, constants().ids.int_.lin_eq, int_lin_eq_t);
      std::cerr << "  int_lin_eq = " << int_lin_eq << std::endl;
//       assert(fi);
//       int_lin_eq = (fi && fi->e()) ? fi : NULL;
      int_lin_le = env.orig->matchFn(env, constants().ids.int_.lin_le, int_lin_eq_t);
      float_lin_eq = env.orig->matchFn(env, constants().ids.float_.lin_eq, float_lin_eq_t);
      float_lin_le = env.orig->matchFn(env, constants().ids.float_.lin_le, float_lin_eq_t);
      int2float = env.orig->matchFn(env, constants().ids.int2float, t_VIVF);

      lin_exp_int = env.orig->matchFn(env, constants().ids.lin_exp, int_lin_eq_t);
      lin_exp_float = env.orig->matchFn(env, constants().ids.lin_exp, float_lin_eq_t);

//       std::cerr << "  lin_exp_int=" << lin_exp_int << std::endl;
//       std::cerr << "  lin_exp_float=" << lin_exp_float << std::endl;
    }
  //   bool matchAndMarkFunction();
  //   std::set<FunctionI*> funcs;

    /// Possible function param sets
    std::vector<Type> t_VII = { Type::varint(), Type::parint() };
    std::vector<Type> t_VIVI = { Type::varint(), Type::varint() };
    std::vector<Type> t_VIIVI = { Type::varint(), Type::parint(), Type::varint() };
    std::vector<Type> t_VFVI = { Type::varfloat(), Type::varint() };
    std::vector<Type> t_VFFVI = { Type::varfloat(), Type::parfloat(), Type::varint() } ;
    std::vector<Type> t_VFVIF = { Type::varfloat(), Type::varint(), Type::parfloat() };
    std::vector<Type> t_VFFF = { Type::varfloat(), Type::parfloat(), Type::parfloat() };
  //     std::vector<Type> t_VFVFVIF({ Type::varfloat(), Type::varfloat(), Type::varint(), Type::parfloat() });

    std::vector<Type> t_VIAVI = { Type::varint(), Type::varint(1) };
    std::vector<Type> t_VISI = { Type::varint(), Type::parsetint() };
    std::vector<Type> t_VISIVI = { Type::varint(), Type::parsetint(), Type::varint() };
    
  //     std::vector<Type> t_intarray(1);
  //     t_intarray[0] = Type::parint(-1);
    enum EnumReifType { RIT_None, RIT_Static, RIT_Reif, RIT_Halfreif1, RIT_Halfreif0 };
    enum EnumConstrType { CT_None, CT_Comparison, CT_SetIn, CT_Encode };
    enum EnumCmpType { CMPT_None, CMPT_LE, CMPT_GE, CMPT_EQ, CMPT_NE, CMPT_LT,
                          CMPT_LE_0, CMPT_EQ_0, CMPT_LT_0 };
    enum EnumVarType { VT_None, VT_Int, VT_Float };

    /// struct DomainCallType describes & characterizes a possible domain constr call
    struct DomainCallType {
      const char* sFuncName=0;
      const std::vector<Type>& aParams;
  //     unsigned iItem;          // call's item number in the flat
      EnumReifType nReifType = RIT_None;   // 0/static/halfreif/reif
      EnumConstrType nConstrType = CT_None;  //
      EnumCmpType nCmpType = CMPT_None;
      EnumVarType nVarType = VT_None;
  //     double dEps = -1.0;
      DomainCallType(const char* fn, const std::vector<Type>& prm,
                    EnumReifType er, EnumConstrType ec, EnumCmpType ecmp, EnumVarType ev)
        : sFuncName(fn), aParams(prm), nReifType(er), nConstrType(ec), nCmpType(ecmp),
          nVarType(ev)  { }
    };
    
    typedef UNORDERED_NAMESPACE::unordered_map<FunctionI*, DomainCallType*> M__POSTCallTypes;
    M__POSTCallTypes mCallTypes;             // actually declared in the input
    std::vector<DomainCallType> aCT;         // all possible
    
    // Fails:
  //   DomainCallType a = { NULL, t_VII, RIT_Halfreif, CT_Comparison, CMPT_EQ, VT_Float };

    /// struct VarDescr stores some info about variables involved in domain constr
    struct VarDescr {
      typedef unsigned char boolShort;
      VarDescr(VarDecl* vd_, boolShort fi, double l_=0.0, double u_=0.0)
        : vd(vd_), fInt(fi), lb(l_), ub(u_)  { }
      double lb, ub;
      boolShort fInt;
      VarDecl* vd = 0;
      boolShort fPropagatedViews=0;
      boolShort fPropagatedLargerEqns=0;
    };
    
    std::vector<VarDescr> vVarDescr;
    
    void register__POSTconstraints()
    {
      EnvI& env = getEnv()->envi();
      GCLock lock;

      aCT.clear();
      aCT.push_back(DomainCallType("int_le_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_LE, VT_Int));
      aCT.push_back(DomainCallType("int_ge_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_GE, VT_Int));
      aCT.push_back(DomainCallType("int_eq_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_EQ, VT_Int));
      aCT.push_back(DomainCallType("int_ne__POST", t_VII, RIT_Static, CT_Comparison, CMPT_NE, VT_Int));

      aCT.push_back(DomainCallType("float_le_reif__POST", t_VFFVI, RIT_Reif, CT_Comparison, CMPT_LE, VT_Float));
      aCT.push_back(DomainCallType("float_ge_reif__POST", t_VFFVI, RIT_Reif, CT_Comparison, CMPT_GE, VT_Float));
      aCT.push_back(DomainCallType("aux_float_lt_zero_iff_1__POST", t_VFVIF, RIT_Reif, CT_Comparison, CMPT_LT, VT_Float));
      aCT.push_back(DomainCallType("float_eq_reif__POST", t_VFFVI, RIT_Reif, CT_Comparison, CMPT_EQ, VT_Float));
      aCT.push_back(DomainCallType("float_ne__POST", t_VFFF, RIT_Static, CT_Comparison, CMPT_NE, VT_Float));

      aCT.push_back(DomainCallType("aux_float_eq_zero_if_1__POST", t_VFVI, RIT_Halfreif1, CT_Comparison, CMPT_EQ_0, VT_Float));
      aCT.push_back(DomainCallType("aux_int_le_zero_if_0__POST", t_VIVI, RIT_Halfreif0, CT_Comparison, CMPT_LE_0, VT_Int));
      aCT.push_back(DomainCallType("aux_float_le_zero_if_0__POST", t_VFVI, RIT_Halfreif0, CT_Comparison, CMPT_LE_0, VT_Float));
      aCT.push_back(DomainCallType("aux_float_lt_zero_if_0__POST", t_VFVIF, RIT_Halfreif0, CT_Comparison, CMPT_LT_0, VT_Float));

      aCT.push_back(DomainCallType("equality_encoding__POST", t_VIAVI, RIT_Static, CT_Encode, CMPT_None, VT_Int));
      aCT.push_back(DomainCallType("set_in__POST", t_VISI, RIT_Static, CT_SetIn, CMPT_None, VT_Int));
      aCT.push_back(DomainCallType("set_in_reif__POST", t_VISIVI, RIT_Reif, CT_SetIn, CMPT_None, VT_Int));
      /// Registering all declared & compatible __POST constraints
      /// (First, cleanup FunctionIs' payload:  -- not doing now)
      for ( int i=0; i<aCT.size(); ++i ) {
        FunctionI* fi = env.orig->matchFn(env, ASTString(aCT[i].sFuncName), aCT[i].aParams);
        if (fi) {
          mCallTypes[fi] = aCT.data() + i;
  //         fi->pPayload = (void*)this;
  //         std::cerr << "  FOund declaration: " << aCT[i].sFuncName << std::endl;
        } else {
          std::cerr << "  MIssing declaration: " << aCT[i].sFuncName << std::endl;
        }
      }
    }
    
    
    /// Registering all __POST calls' domain-constrained variables
    void register__POSTvariables() {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      // First, cleanup VarDecls' payload which stores index in vVarDescr
      Model& mFlat = *getEnv()->flat();
      for( VarDeclIterator ivd=mFlat.begin_vardecls(); ivd!=mFlat.end_vardecls(); ++ivd )
        ivd->e()->payload(-1);
      // Iterate thru original __POST constraints to mark constrained vars:
      for( ConstraintIterator ic=mFlat.begin_constraints();
              ic != mFlat.end_constraints(); ++ic ) {
        if ( ic->removed() )
          continue;
        if ( Call* c = ic->e()->dyn_cast<Call>() ) {
          if ( auto ipct = mCallTypes.find(c->decl())
                != mCallTypes.end() ) {
            assert( c->args().size() > 1 );
          VarDecl* vd0 = expr2VarDecl(c->args()[0]);
            std::cerr << "  Call " << c->id().str()
              << " uses variable " << vd0->id()->str();
            if ( vd0->payload() == -1 ) {         // not yet visited
              vd0->payload( vVarDescr.size() );
              vVarDescr.push_back( VarDescr( vd0, vd0->type().isint() ) );  // can use /prmTypes/ as well
              // bounds/domains later for each involved var TODO
              if (vd0->e())
                CheckInitExpr(vd0);
            } else {
              std::cerr << " (already touched)";
            }
            std::cerr << std::endl;
          }
        }
      }
    }
    
    struct LinEqData {
      std::vector<double> coefs;
      std::vector<VarDecl*> vd;
      double rhs;
    };
    
    // Should only be called on a newly added variable
    // OR when looking thru all non-touched vars
    void CheckInitExpr(VarDecl* vd) {
      assert( vd->e() );
      std::cerr << "  Checking init expr  ";
      debugprint(vd);
      if ( Id* id = vd->e()->dyn_cast<Id>() ) {
        LinEqData led;
        led.vd = { vd, expr2VarDecl(id->decl()->e()) };
      }
    }

  //   typedef std::vector<int> TAgenda;
    
    /// Build var cliques (i.e. of var pairs viewing each other)
    void constructVarViewCliques() {
  //     TAgenda agenda(vVarDescr.size()), agendaNext;
  //     for ( int i=0; i<agenda.size(); ++i )
  //       agenda[i] = i;
      bool fChanges;
      do {
        fChanges = false;
        PropagateViews(fChanges);
        PropagateImplViews(fChanges);
      } while ( fChanges );
    }
    
    void PropagateViews(bool &fChanges) {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      
      // Can also go thru all vars to check init expr. TODO
      
      // Iterate thru original 2-variable equalities to mark views:
      Model& mFlat = *getEnv()->flat();
//       std::cerr << "  Model: " << std::endl;
//       debugprint(&mFlat);
      for( ConstraintIterator ic=mFlat.begin_constraints();
              ic != mFlat.end_constraints(); ++ic ) {
//         std::cerr << "  SEE constraint: " << "      ";
//         debugprint(&*ic);
//         debugprint(c->decl());
        if ( ic->removed() )
          continue;
        if ( Call* c = ic->e()->dyn_cast<Call>() ) {
          if ( int_lin_eq==c->decl() || float_lin_eq==c->decl() ) {
//             std::cerr << "  NOTE call " << std::flush;
//             debugprint(c);
            assert( c->args().size() == 3 );
            ArrayLit* al = c->args()[1]->dyn_cast<ArrayLit>();
            assert( al );
            assert( al->v().size() >= 2 );
            if ( al->v().size() == 2 ) {   // 2-term eqn
              LinEqData led;
              expr2DeclArray(c->args()[1], led.vd);
              if ( led.vd[0]->payload() >= 0 or led.vd[1]->payload()>=0 ) {
                if ( sCallLinEq2.end() != sCallLinEq2.find(c) )
                  continue;
                sCallLinEq2.insert(c);     // memorize this call
                std::cerr << "  REG call " << std::flush;
                debugprint(c);
                led.rhs = expr2Const(c->args()[2]);
                expr2Array(c->args()[0], led.coefs);
                assert( 2 == led.coefs.size() );
                fChanges = true;
                put2VarsConnection( led );
              }
            } else {                        // larger eqns
              // TODO should be here?
            }
          }
          else if ( int2float==c->decl() || constants().var_redef==c->decl() ) {
            std::cerr << "  NOTE call " << std::flush;
            debugprint(c);
            assert( c->args().size() == 2 );
            LinEqData led;
            led.vd.resize(2);
            led.vd[0] = expr2VarDecl(c->args()[0]);
            led.vd[1] = expr2VarDecl(c->args()[1]);
            if ( led.vd[0]->payload() >= 0 or led.vd[1]->payload()>=0 ) {
              if ( sCallInt2Float.end() != sCallInt2Float.find(c) )
                continue;
              sCallInt2Float.insert(c);     // memorize this call
              std::cerr << "  REG call " << std::flush;
              debugprint(c);
              led.rhs = 0.0;
              led.coefs = { 1.0, -1.0 };
              fChanges = true;
              put2VarsConnection( led );
            }
          }
        }
      }
    }

    void PropagateImplViews(bool &fChanges) {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      
      // TODO
    }
  
    // Could be better to mark the calls instead:
    UNORDERED_NAMESPACE::unordered_set<Call*> sCallLinEq2, sCallInt2Float;
    
    void put2VarsConnection( LinEqData& led ) {
    }
      
    VarDecl* expr2VarDecl(Expression* arg) {
      assert( not arg->dyn_cast<IntLit>() );
      assert( not arg->dyn_cast<FloatLit>() );
      assert( not arg->dyn_cast<BoolLit>() );
      Id* id = arg->dyn_cast<Id>();
      assert(id);
      VarDecl* vd = id->decl();
      assert(vd);
      return vd;
    }
      
    void expr2DeclArray(Expression* arg, std::vector<VarDecl*>& aVD) {
      ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
      aVD.resize(al->v().size());
      for (unsigned int i=0; i<al->v().size(); i++)
        aVD[i] = expr2VarDecl(al->v()[i]);
    }
    
    double expr2Const(Expression* arg) {
      if (IntLit* il = arg->dyn_cast<IntLit>()) {
        return ( il->v().toInt() );
      } else if (FloatLit* fl = arg->dyn_cast<FloatLit>()) {
        return ( fl->v() );
      } else if (BoolLit* bl = arg->dyn_cast<BoolLit>()) {
        return ( bl->v() );
      } else {
        throw InternalError("unexpected expression instead of a literal");
      }
      return 0.0;
    }
    
    void expr2Array(Expression* arg, std::vector<double>& vals) {
      ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
      vals.resize(al->v().size());
      for (unsigned int i=0; i<al->v().size(); i++) {
        vals[i] = expr2Const(al->v()[i]);
      }
    }
    
    void printStats(std::ostream& os) {
      os << "N cliques ";
    }

  };  // class MIPD

  void MIPdomains(Env& env) {
    MIPD mipd(&env);
    mipd.MIPdomains();
  }
  
}  // namespace MiniZinc
