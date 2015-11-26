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

#include <map>

/// TODOs
/// set_in etc. are not propagated between views
/// CLEANUP after work: ~destructor
/// Also check initexpr of all vars?  DONE
/// In case of only_range_domains we'd need to register inequalities
///   - so better turn that off TODO

namespace MiniZinc {
  class MIPD {  
  public:
    MIPD(Env* env) : __env(env) { getEnv(); }
    bool MIPdomains() {
      registerLinearConstraintDecls();
      register__POSTconstraintDecls();
      register__POSTvariables();
      constructVarViewCliques();
      if ( not decomposeDomains() )
        return false;
      printStats(std::cerr);
      return true;
    }
    
  private:
    
    Env* __env=0;
    Env* getEnv() { assert(__env); return __env; }
    
    typedef VarDecl* PVarDecl;
    
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
      
    void registerLinearConstraintDecls()
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
    struct DCT {
      const char* sFuncName=0;
      const std::vector<Type>& aParams;
  //     unsigned iItem;          // call's item number in the flat
      EnumReifType nReifType = RIT_None;   // 0/static/halfreif/reif
      EnumConstrType nConstrType = CT_None;  //
      EnumCmpType nCmpType = CMPT_None;
      EnumVarType nVarType = VT_None;
      FunctionI* &pfi;
  //     double dEps = -1.0;
      DCT(const char* fn, const std::vector<Type>& prm,
                    EnumReifType er, EnumConstrType ec, EnumCmpType ecmp, EnumVarType ev,
                    FunctionI* &pfi__
         )
        : sFuncName(fn), aParams(prm), nReifType(er), nConstrType(ec), nCmpType(ecmp),
          nVarType(ev), pfi(pfi__) { }
    };
    
    typedef UNORDERED_NAMESPACE::unordered_map<FunctionI*, DCT*> M__POSTCallTypes;
    M__POSTCallTypes mCallTypes;             // actually declared in the input
    std::vector<DCT> aCT;         // all possible
    
    // Fails:
  //   DomainCallType a = { NULL, t_VII, RIT_Halfreif, CT_Comparison, CMPT_EQ, VT_Float };

    /// struct VarDescr stores some info about variables involved in domain constr
    struct VarDescr {
      typedef unsigned char boolShort;
      VarDescr(VarDecl* vd_, boolShort fi, double l_=0.0, double u_=0.0)
        : vd(vd_), fInt(fi), lb(l_), ub(u_)  { }
      double lb, ub;
      VarDecl* vd = 0;
      int nClique = -1;                 // clique number
      std::vector<Call*> aCalls;
      boolShort fInt=0;
      boolShort fHasEqEncode=0;
      boolShort fDomainConstrProcessed=0;
//       boolShort fPropagatedViews=0;
//       boolShort fPropagatedLargerEqns=0;
    };
    
    std::vector<VarDescr> vVarDescr;
    
    FunctionI *int_le_reif__POST=0, *int_ge_reif__POST=0, *int_eq_reif__POST=0, *int_ne__POST=0,
      *float_le_reif__POST=0, *float_ge_reif__POST=0, *aux_float_lt_zero_iff_1__POST=0, 
        *float_eq_reif__POST=0, *float_ne__POST=0,
      *aux_float_eq_zero_if_1__POST=0, *aux_int_le_zero_if_0__POST=0,
        *aux_float_le_zero_if_0__POST=0, *aux_float_lt_zero_if_0__POST=0,
      *equality_encoding__POST=0, *set_in__POST=0, *set_in_reif__POST=0;
    
    void register__POSTconstraintDecls()
    {
      EnvI& env = getEnv()->envi();
      GCLock lock;

      aCT.clear();
      aCT.push_back(DCT("int_le_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_LE, VT_Int, int_le_reif__POST));
      aCT.push_back(DCT("int_ge_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_GE, VT_Int, int_ge_reif__POST));
      aCT.push_back(DCT("int_eq_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_EQ, VT_Int, int_eq_reif__POST));
      aCT.push_back(DCT("int_ne__POST", t_VII, RIT_Static, CT_Comparison, CMPT_NE, VT_Int, int_ne__POST));

      aCT.push_back(DCT("float_le_reif__POST", t_VFFVI, RIT_Reif, CT_Comparison, CMPT_LE, VT_Float, float_le_reif__POST));
      aCT.push_back(DCT("float_ge_reif__POST", t_VFFVI, RIT_Reif, CT_Comparison, CMPT_GE, VT_Float, float_ge_reif__POST));
      aCT.push_back(DCT("aux_float_lt_zero_iff_1__POST", t_VFVIF, RIT_Reif, CT_Comparison, CMPT_LT, VT_Float,
                        aux_float_lt_zero_iff_1__POST));
      aCT.push_back(DCT("float_eq_reif__POST", t_VFFVI, RIT_Reif, CT_Comparison, CMPT_EQ, VT_Float, float_eq_reif__POST));
      aCT.push_back(DCT("float_ne__POST", t_VFFF, RIT_Static, CT_Comparison, CMPT_NE, VT_Float, float_ne__POST));

      aCT.push_back(DCT("aux_float_eq_zero_if_1__POST", t_VFVI, RIT_Halfreif1, CT_Comparison, CMPT_EQ_0, VT_Float,
                        aux_float_eq_zero_if_1__POST));
      aCT.push_back(DCT("aux_int_le_zero_if_0__POST", t_VIVI, RIT_Halfreif0, CT_Comparison, CMPT_LE_0, VT_Int,
                        aux_int_le_zero_if_0__POST));
      aCT.push_back(DCT("aux_float_le_zero_if_0__POST", t_VFVI, RIT_Halfreif0, CT_Comparison, CMPT_LE_0, VT_Float,
                        aux_float_le_zero_if_0__POST));
      aCT.push_back(DCT("aux_float_lt_zero_if_0__POST", t_VFVIF, RIT_Halfreif0, CT_Comparison, CMPT_LT_0, VT_Float,
                        aux_float_lt_zero_if_0__POST));
      
      aCT.push_back(DCT("equality_encoding__POST", t_VIAVI, RIT_Static, CT_Encode, CMPT_None, VT_Int, equality_encoding__POST));
      aCT.push_back(DCT("set_in__POST", t_VISI, RIT_Static, CT_SetIn, CMPT_None, VT_Int, set_in__POST));
      aCT.push_back(DCT("set_in_reif__POST", t_VISIVI, RIT_Reif, CT_SetIn, CMPT_None, VT_Int, set_in_reif__POST));
      /// Registering all declared & compatible __POST constraints
      /// (First, cleanup FunctionIs' payload:  -- not doing now)
      for ( int i=0; i<aCT.size(); ++i ) {
        FunctionI* fi = env.orig->matchFn(env, ASTString(aCT[i].sFuncName), aCT[i].aParams);
        if (fi) {
          mCallTypes[fi] = aCT.data() + i;
          aCT[i].pfi = fi;
  //         fi->pPayload = (void*)this;
  //         std::cerr << "  FOund declaration: " << aCT[i].sFuncName << std::endl;
        } else {
          aCT[i].pfi = 0;
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
                checkInitExpr(vd0);
            } else {
              std::cerr << " (already touched)";
            }
            vVarDescr[ vd0->payload() ].aCalls.push_back(c);
            if ( equality_encoding__POST == c->decl() ) {
              vVarDescr[ vd0->payload() ].fHasEqEncode = true;
              std::cerr << " Variable " << vd0->id()->str() << " has eq_encode." << std::endl;
            }   // + if has aux_ constraints?
            std::cerr << std::endl;
          }
        }
      }
    }
    
    struct LinEq2Vars {
      std::array<double, 2> coefs;
      std::array<PVarDecl, 2> vd = { { 0, 0 } };
      double rhs;
    };
    
    struct LinEq {
      std::vector<double> coefs;
      std::vector<VarDecl*> vd;
      double rhs;
    };
    // Should only be called on a newly added variable
    // OR when looking thru all non-touched vars
    /// Checks init expr of a variable
    /// Return true IFF new connection
    /// The bool param states if propagating from the rhs
    // Guido: cannot be recursive in FZN
    bool checkInitExpr(VarDecl* vd, bool fCheckArg=false) {
      assert( vd->e() );
      if ( not vd->type().isint() and not vd->type().isfloat() )
        return false;
      if ( not fCheckArg )
        assert( vd->payload() >= 0 );
      if ( Id* id = vd->e()->dyn_cast<Id>() ) {
//         const int f1 = ( vd->payload()>=0 );
//         const int f2 = ( id->decl()->payload()>=0 );
        assert( not id->decl()->e() );      // no initexpr for initexpr
        if ( not fCheckArg or ( id->decl()->payload()>=0 ) ) {
          std::cerr << "  Checking init expr  ";
          debugprint(vd);
          LinEq2Vars led;
          // FAILS:
  //         led.vd = { vd, expr2VarDecl(id->decl()->e()) };
          led.vd = { vd, expr2VarDecl( vd->e() ) };
          led.coefs = { 1.0, -1.0 };
          led.rhs = 0.0;
          put2VarsConnection( led, false );
          return true;
        }
      } else if ( Call* c = vd->e()->dyn_cast<Call>() ) {
        if ( lin_exp_int==c->decl() || lin_exp_float==c->decl() ) {
//             std::cerr << "  NOTE call " << std::flush;
//             debugprint(c);
          assert( c->args().size() == 3 );
          ArrayLit* al = c->args()[1]->dyn_cast<ArrayLit>();
          assert( al );
          assert( al->v().size() >= 1 );
          if ( al->v().size() == 1 ) {   // 1-term scalar product in the rhs
            LinEq2Vars led;
            led.vd = { vd, expr2VarDecl(al->v()[0]) };
//             const int f1 = ( vd->payload()>=0 );
//             const int f2 = ( led.vd[1]->payload()>=0 );
            assert( not led.vd[1]->e() );      // no initexpr for initexpr
            if ( not fCheckArg or ( led.vd[1]->payload()>=0 ) ) {
              // Can use another map here:
//               if ( sCallLinEq2.end() != sCallLinEq2.find(c) )
//                 continue;
//               sCallLinEq2.insert(c);     // memorize this call
              std::cerr << "  REG call " << std::flush;
              debugprint(vd);
              std::array<double, 1> coef0;
              expr2Array(c->args()[0], coef0);
              led.coefs = { -1.0, coef0[0] };
              led.rhs = -expr2Const(c->args()[2]);             // MINUS
              put2VarsConnection( led, false );
              return true;
            }
          } else {                        // larger eqns
            // TODO should be here?
          }
        }
      }
      return false;
    }

  //   typedef std::vector<int> TAgenda;
    
    /// Build var cliques (i.e. of var pairs viewing each other)
    void constructVarViewCliques() {
//       std::cerr << "  Model: " << std::endl;
//       debugprint(getEnv()->flat());
      
  //     TAgenda agenda(vVarDescr.size()), agendaNext;
  //     for ( int i=0; i<agenda.size(); ++i )
  //       agenda[i] = i;
      bool fChanges;
      do {
        fChanges = false;
        propagateViews(fChanges);
        propagateImplViews(fChanges);
      } while ( fChanges );
    }
    
    void propagateViews(bool &fChanges) {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      
      // Iterate thru original 2-variable equalities to mark views:
      Model& mFlat = *getEnv()->flat();
      
      std::cerr << "  Check all initexpr if they access a touched variable:" << std::endl;
      for( VarDeclIterator ivd=mFlat.begin_vardecls(); ivd!=mFlat.end_vardecls(); ++ivd ) {
        if ( ivd->removed() )
          continue;
        if ( ivd->e()->e() and ivd->e()->payload()<0       // untouched
          and ( ivd->e()->type().isint() or ivd->e()->type().isfloat() ) )
          if ( checkInitExpr(ivd->e(), true) )
            fChanges = true;
      }
        
      std::cerr << "  Check all constraints for 2-var equations:" << std::endl;
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
              LinEq2Vars led;
              expr2DeclArray(c->args()[1], led.vd);
              // At least 1 touched var:
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
//             std::cerr << "  NOTE call " << std::flush;
//             debugprint(c);
            assert( c->args().size() == 2 );
            LinEq2Vars led;
//             led.vd.resize(2);
            led.vd[0] = expr2VarDecl(c->args()[0]);
            led.vd[1] = expr2VarDecl(c->args()[1]);
            // At least 1 touched var:
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

    void propagateImplViews(bool &fChanges) {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      
      // TODO
    }
  
    /// Could be better to mark the calls instead:
    UNORDERED_NAMESPACE::unordered_set<Call*> sCallLinEq2, sCallInt2Float;
    
    class TClique : public std::vector<LinEq2Vars> {       // need more info?
    public:
      /// This function takes the 1st variable and relates all to it
      /// Return false if contrad / disconnected graph
//       bool findRelations0() {
//         return true;
//       }
    };
    typedef std::vector<TClique> TCLiqueList;
    TCLiqueList aCliques;
    
    /// register a 2-variable lin eq
    /// add it to the var clique, joining the participants' cliques if needed
    void put2VarsConnection( LinEq2Vars& led, bool fCheckinitExpr=true ) {
      assert( led.coefs.size() == led.vd.size() );
      assert( led.vd.size() == 2 );
      std::cerr << "  Register 2-var connection: ( [";
      for (auto c : led.coefs)
        std::cerr << c << ' ';
      std::cerr << " ] * [ ";
      for (auto v : led.vd)
        std::cerr << v->id()->str() << ' ';
      std::cerr << " ] ) == " << led.rhs << std::endl;
      // register if new variables
//       std::vector<bool> fHaveClq(led.vd.size(), false);
      int nCliqueAvailable=-1;
      for ( auto vd : led.vd ) {
        if ( vd->payload() < 0 ) {         // not yet visited
          vd->payload( vVarDescr.size() );
          vVarDescr.push_back( VarDescr( vd, vd->type().isint() ) );  // can use /prmTypes/ as well
          if ( fCheckinitExpr and vd->e() )
            checkInitExpr(vd);
        } else {
          int nMaybeClq = vVarDescr[vd->payload()].nClique;
          if ( nMaybeClq >= 0 )
            nCliqueAvailable = nMaybeClq;
//           assert( nCliqueAvailable>=0 );
//           fHaveClq[i] = true;
        }
      }
      if ( nCliqueAvailable < 0 ) {    // no clique found
        nCliqueAvailable = aCliques.size();
        aCliques.resize(aCliques.size() + 1);
      }
      TClique& clqNew = aCliques[nCliqueAvailable];
      clqNew.push_back( led );
      for ( auto vd : led.vd ) {       // merging cliques
        int& nMaybeClq = vVarDescr[vd->payload()].nClique;
        if ( nMaybeClq >= 0 and nMaybeClq != nCliqueAvailable ) {
          TClique& clqOld = aCliques[nMaybeClq];
          assert( clqOld.size() );
          for ( auto eq2 : clqOld ) {
            for ( auto vd : eq2.vd ) {    // point all the variables to the new clique
              vVarDescr[ vd->payload() ].nClique = nCliqueAvailable;
            }
          }
          clqNew.insert(clqNew.end(), clqOld.begin(), clqOld.end());
          clqOld.clear();                // Can use C++11 move      TODO
          std::cerr << "    +++ Joining cliques" << std::endl;
        }          
        nMaybeClq = nCliqueAvailable;  // Could mark as 'unused'  TODO
      }
    }
    
    /// Finds a clique variable to which all domain constr are related
    class TCliqueSorter {
      MIPD& mipd;
      const int iVarStart;
    public:
//       VarDecl* varRef0=0;  // this is the first var to which all others are related
      VarDecl* varRef1=0;  // this is the chosen main reference.
        // it is a var with eq_encode, or
        // an (integer if any) variable with the least rel. factor
      bool fRef1HasEqEncode=false;
      /// This map stores the relations y = ax+b of all the clique's vars to y
      typedef UNORDERED_NAMESPACE::unordered_map<VarDecl*, std::pair<double, double> >
        TMapVars;
      TMapVars mRef0, mRef1;   // to the main var 0, 1
      typedef UNORDERED_NAMESPACE::unordered_map<VarDecl*, TMapVars> TMatrixVars;
      class LinEqGraph : public TMatrixVars {
      public:
        /// Stores the arc (x1, x2) as x1 = a*x2 + b
        /// so that a constraint on x2, say x2<=c <-> f,
        /// is equivalent to one for x1:  x1 <=/>= a*c+b <-> f
        //// ( the other way involves division:
        ////   so that a constraint on x1, say x1<=c <-> f,
        ////   can easily be converted into one for x2 as a*x2 <= c-b <-> f
        ////   <=> x2 (care for sign) (c-b)/a <-> f )

        template <class ICoef, class IVarDecl>
        void addArc(ICoef begC, IVarDecl begV, double rhs) {
          assert( std::fabs( *begC ) >= 1e-10 );
          // Transform Ax+By=C into x = -B/Ay+C/A
          const double negBA = -(*(begC+1))/(*begC);
          const double CA = rhs/(*begC);
          checkExistingArc(begV, negBA, CA);
          (*this)[*begV][*(begV+1)] = std::make_pair(negBA, CA);
        }
        void addEdge(LinEq2Vars& led) {
          addArc( led.coefs.begin(), led.vd.begin(), led.rhs );
          addArc( led.coefs.rbegin(), led.vd.rbegin(), led.rhs );
        }
        /// Check existing connection
        template <class IVarDecl>
        bool checkExistingArc(IVarDecl begV, double A, double B, bool fReportRepeat=true) {
          auto it1 = this->find(*begV);
          if ( this->end() != it1 ) {
            auto it2 = it1->second.find(*(begV+1));
            if ( it1->second.end() != it2 ) {
              assert( std::fabs( it2->second.first - A )
                < 1e-6 * std::max( std::fabs(it2->second.first), std::fabs(A) ) );
              assert( std::fabs( it2->second.second - B )
                < 1e-6 * std::max( std::fabs(it2->second.second), std::fabs(B) ) + 1e-6 );
              if ( std::fabs( A ) < 1e-12  )
                std::cerr << " Very small coef: "
                  << (*begV)->id()->str() << " = "
                  << A << " * " << (*(begV+1))->id()->str()
                  << " + " << B << std::endl;
              if ( fReportRepeat )
                std::cerr << "LinEqGraph: eqn between "
                  << (*begV)->id()->str() << " and " << (*(begV+1))->id()->str()
                  << " is repeated. " << std::endl;
              return true;
            }
          }
          return false;
        }
        /// Propagate linear relations from the given variable
        void propagate(iterator itStart, TMapVars& mWhereStore) {
          assert( this->end()!=itStart );
          TMatrixVars mTemp;
          mTemp[itStart->first] = itStart->second;       // init with existing
          std::cerr << "Propagation started from "
            << itStart->first->id()->str() << std::endl;
          propagate2(itStart, itStart, std::make_pair(1.0, 0.0), mTemp);
          mWhereStore = mTemp.begin()->second;
          assert( mWhereStore.size() == this->size()-1 );     // connectedness
        }
        /// Propagate linear relations from it1 via it2
        void propagate2(iterator itSrc, iterator itVia,
                        std::pair<double, double> rel, TMatrixVars& mWhereStore) {
          for ( auto itDst=itVia->second.begin(); itDst!=itVia->second.end(); ++itDst ) {
          // Transform x1=A1x2+B1, x2=A2x3+B2 into x1=A1A2x3+A1B2+B1
            if ( itDst->first == itSrc->first )
              continue;
            const double A1A2 = rel.first * itDst->second.first;
            const double A1B2plusB1 = rel.first*itDst->second.second + rel.second;
            bool fDive=true;
            if ( itSrc != itVia ) {
              PVarDecl vd[2] = { itSrc->first, itDst->first };
              if ( not checkExistingArc(vd, A1A2, A1B2plusB1, false) ) {
                mWhereStore[vd[0]][vd[1]] = std::make_pair(A1A2, A1B2plusB1);
                std::cerr << "   PROPAGATING: "
                  << vd[0]->id()->str() << " = "
                  << A1A2 << " * " << vd[1]->id()->str()
                  << " + " << A1B2plusB1 << std::endl;
              } else
                fDive = false;
            }
            if ( fDive ) {
              auto itDST = this->find(itDst->first);
              assert( this->end() != itDST );
              propagate2(itSrc, itDST, std::make_pair(A1A2, A1B2plusB1), mWhereStore);
            }
          }
        }        
      };
      LinEqGraph leg;
      
      TCliqueSorter(MIPD* pm, int iv) : mipd(*pm), iVarStart(iv)  { }
      void doRelate() {
        assert( mipd.vVarDescr[iVarStart].nClique >= 0 );
        const TClique& clq = mipd.aCliques[ mipd.vVarDescr[iVarStart].nClique ];
        for ( auto eq2 : clq ) {
          leg.addEdge(eq2);
        }
        std::cerr << " Clique " << mipd.vVarDescr[iVarStart].nClique
          << ": " << leg.size() << " variables, "
          << clq.size() << " connections." << std::endl;
        for ( auto it1=leg.begin(); it1!=leg.end(); ++it1 )
          mipd.vVarDescr[ it1->first->payload() ].fDomainConstrProcessed = true;
        
        // Propagate the 1st var's relations:
        leg.propagate(leg.begin(), mRef0);
        
        // Find a best main variable according to:
        // 1. isInt 2. hasEqEncode 3. linFactor to ref0
        varRef1 = leg.begin()->first;
        std::array<double, 3> aCrit = { { (double)mipd.vVarDescr[varRef1->payload()].fInt,
          (double)mipd.vVarDescr[varRef1->payload()].fHasEqEncode, 1.0 } };
        for ( auto it2=mRef0.begin(); it2!=mRef0.end(); ++it2 ) {
          VarDescr& vard = mipd.vVarDescr[ it2->first->payload() ];
          std::array<double, 3> aCrit1 =
            { { (double)vard.fInt, (double)vard.fHasEqEncode, std::fabs(it2->second.first) } };
          if ( aCrit1 > aCrit ) {
            varRef1 = it2->first;
            aCrit = aCrit1;
          }
        }
        leg.propagate(leg.find(varRef1), mRef1);
      }
    };  // class TCliqueSorter
    
    /// Build a domain decomposition for a clique
    /// a clique can consist of just 1 var without a clique object
    class DomainDecomp {
    public:
      MIPD& mipd;
      const int iVarStart;
      TCliqueSorter cls;
      
      DomainDecomp(MIPD* pm, int iv) : mipd(*pm), iVarStart(iv), cls(pm, iv)  { }
      void doProcess() {
        // Choose the main variable and relate all others to it
        if ( mipd.vVarDescr[iVarStart].nClique >= 0 ) {
          cls.doRelate();
        } else
          cls.varRef1 = mipd.vVarDescr[ iVarStart ].vd;
        
        int iVarRef1 = cls.varRef1->payload();
        cls.fRef1HasEqEncode = mipd.vVarDescr[ iVarRef1 ].fHasEqEncode;
        
        // First, construct the domain decomposition in any case
        
        
        // Then, use equality_encoding if available
        if ( cls.fRef1HasEqEncode ) {
          
        } else {  // not cls.fRef1HasEqEncode
          
        }
      }
    };  // class DomainDecomp
    
    /// Vars without explicit clique still need a decomposition.
    /// Have noticed all __POSTs, set_in's and eq_encode's to it BEFORE
    /// In each clique, relate all vars to one chosen
    /// Find all "smallest rel. factor" variables, integer and with eq_encode if avail
    /// Re-relate all vars to it
    /// Refer all __POSTs and dom() to it
    /// build domain decomposition
    /// Implement all domain constraints, incl. possible corresp, of eq_encode's
    /// Not impose effects of integrality scaling (e.g., int v = int k/3)
    /// BUT when using k's eq_encode?
    bool decomposeDomains() {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      
//       for (int iClq=0; iClq<aCliques.size(); ++iClq ) {
//         TClique& clq = aCliques[iClq];
//       }
      for ( int iVar=0; iVar<vVarDescr.size(); ++iVar ) {
//         VarDescr& var = vVarDescr[iVar];
        if ( not vVarDescr[iVar].fDomainConstrProcessed ) {
          try {
            DomainDecomp dd(this, iVar);
            dd.doProcess();
            vVarDescr[iVar].fDomainConstrProcessed = true;
          } catch ( ... ) {  // a contradiction
            return false;
          }
        }
      }
      return true;
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
      
    template <class Array>
    void expr2DeclArray(Expression* arg, Array& aVD) {
      ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
      checkOrResize( aVD, al->v().size() );
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
        throw InternalError(
          "unexpected expression instead of an int/float/bool literal");
      }
      return 0.0;
    }
    
    template <class Container, class Elem, size_t >
    void checkOrResize(Container& cnt, size_t sz) {
      cnt.resize(sz);
    }
    
    template <class Elem, size_t N>
    void checkOrResize(std::array<Elem, N>& cnt, size_t sz) {
      assert( cnt.size() == sz );
    }
    
    template <class Array>
    void expr2Array(Expression* arg, Array& vals) {
      ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
//       if ( typeid(typename Array::pointer) == typeid(typename Array::iterator) )  // fixed array
//         assert( vals.size() == al->v().size() );
//       else
//         vals.resize( al->v().size() );
      checkOrResize(vals, al->v().size());
      for (unsigned int i=0; i<al->v().size(); i++) {
        vals[i] = expr2Const(al->v()[i]);
      }
    }
    
    void printStats(std::ostream& os) {
      int nc=0;
      for ( auto cl : aCliques )
        if ( cl.size() )
          ++nc;
      os << "N cliques " << aCliques.size() << "  total, "
         << nc << " final" << std::endl;
    }

  };  // class MIPD

  void MIPdomains(Env& env) {
    MIPD mipd(&env);
    if ( not mipd.MIPdomains() )
      env.flat()->fail(env.envi());
  }
  
}  // namespace MiniZinc
