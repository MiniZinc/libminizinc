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
/// CSE for lineq coefs     TODO

#define __MZN__DBGOUT__MIPDOMAINS__
#ifdef __MZN__DBGOUT__MIPDOMAINS__
  #define DBGOUT_MIPD(s) std::cerr << s << std::endl
  #define DBGOUT_MIPD__(s) std::cerr << s << std::flush
  #define DBGOUT_MIPD_SELF(op) op
#else
  #define DBGOUT_MIPD(s) do { } while ( false )
  #define DBGOUT_MIPD__(s) do { } while ( false )
  #define DBGOUT_MIPD_SELF(op)  do { } while ( false )
#endif

namespace MiniZinc {
  
  std::vector<double> MIPD__stats( N_POSTs__size );
  
  class MIPD {  
  public:
    MIPD(Env* env) : __env(env) { getEnv(); }
    bool MIPdomains() {
      MIPD__stats[ N_POSTs__domSizeMin ] = 1e100;
      
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
    
//     double float_lt_EPS_coef__ = 1e-5;
      
    void registerLinearConstraintDecls()
    {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      
      int_lin_eq = env.orig->matchFn(env, constants().ids.int_.lin_eq, int_lin_eq_t);
      DBGOUT_MIPD ( "  int_lin_eq = " << int_lin_eq );
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
      // For this to work, need to define a function, see mzn_only_range_domains()
//       {
//         GCLock lock;
//         Call* call_EPS_for_LT =
//           new Call(Location(),"mzn_float_lt_EPS_coef__", std::vector<Expression*>());
//         call_EPS_for_LT->type(Type::parfloat());
//         call_EPS_for_LT->decl(env.orig->matchFn(getEnv()->envi(), call_EPS_for_LT));
//         float_lt_EPS_coef__ = eval_float(getEnv()->envi(), call_EPS_for_LT);
//       }
    }
  //   bool matchAndMarkFunction();
  //   std::set<FunctionI*> funcs;

    /// Possible function param sets
    std::vector<Type> t_VII = { Type::varint(), Type::parint() };
    std::vector<Type> t_VIVI = { Type::varint(), Type::varint() };
    std::vector<Type> t_VIIVI = { Type::varint(), Type::parint(), Type::varint() };
    std::vector<Type> t_VFVI = { Type::varfloat(), Type::varint() };
    std::vector<Type> t_VFVF = { Type::varfloat(), Type::varfloat() };
    std::vector<Type> t_VFFVI = { Type::varfloat(), Type::parfloat(), Type::varint() } ;
    std::vector<Type> t_VFFVIF = { Type::varfloat(), Type::parfloat(), Type::varint(),
      Type::parfloat() } ;
    std::vector<Type> t_VFVIF = { Type::varfloat(), Type::varint(), Type::parfloat() };
    std::vector<Type> t_VFVFF = { Type::varfloat(), Type::varfloat(), Type::parfloat() };
    std::vector<Type> t_VFFF = { Type::varfloat(), Type::parfloat(), Type::parfloat() };
  //     std::vector<Type> t_VFVFVIF({ Type::varfloat(), Type::varfloat(), Type::varint(), Type::parfloat() });

    std::vector<Type> t_VIAVI = { Type::varint(), Type::varint(1) };
    std::vector<Type> t_VISI = { Type::varint(), Type::parsetint() };
    std::vector<Type> t_VISIVI = { Type::varint(), Type::parsetint(), Type::varint() };
    
  //     std::vector<Type> t_intarray(1);
  //     t_intarray[0] = Type::parint(-1);
    
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
//       std::vector<Call*> aCalls;
      std::vector<ConstraintI*> aCalls;
      boolShort fInt=0;
      ConstraintI* pEqEncoding=0;
      boolShort fDomainConstrProcessed=0;
//       boolShort fPropagatedViews=0;
//       boolShort fPropagatedLargerEqns=0;
    };
    
    std::vector<VarDescr> vVarDescr;
    
    FunctionI *int_le_reif__POST=0, *int_ge_reif__POST=0, *int_eq_reif__POST=0, *int_ne__POST=0,
      *float_le_reif__POST=0, *float_ge_reif__POST=0, *aux_float_lt_zero_iff_1__POST=0, 
        *float_eq_reif__POST=0, *float_ne__POST=0,
      *aux_float_eq_zero_if_1__POST=0, *aux_int_le_zero_if_1__POST=0,
        *aux_float_le_zero_if_1__POST=0, *aux_float_lt_zero_if_1__POST=0,
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

      aCT.push_back(DCT("float_le_reif__POST", t_VFFVIF, RIT_Reif, CT_Comparison, CMPT_LE, VT_Float, float_le_reif__POST));
      aCT.push_back(DCT("float_ge_reif__POST", t_VFFVIF, RIT_Reif, CT_Comparison, CMPT_GE, VT_Float, float_ge_reif__POST));
      aCT.push_back(DCT("aux_float_lt_zero_iff_1__POST", t_VFVIF, RIT_Reif, CT_Comparison, CMPT_LT, VT_Float,
                        aux_float_lt_zero_iff_1__POST));
      aCT.push_back(DCT("float_eq_reif__POST", t_VFFVIF, RIT_Reif, CT_Comparison, CMPT_EQ, VT_Float, float_eq_reif__POST));
      aCT.push_back(DCT("float_ne__POST", t_VFFF, RIT_Static, CT_Comparison, CMPT_NE, VT_Float, float_ne__POST));

      aCT.push_back(DCT("aux_float_eq_zero_if_1__POST", t_VFVF, RIT_Halfreif, CT_Comparison, CMPT_EQ_0, VT_Float,
                        aux_float_eq_zero_if_1__POST));
      aCT.push_back(DCT("aux_int_le_zero_if_1__POST", t_VIVI, RIT_Halfreif, CT_Comparison, CMPT_LE_0, VT_Int,
                        aux_int_le_zero_if_1__POST));
      aCT.push_back(DCT("aux_float_le_zero_if_1__POST", t_VFVF, RIT_Halfreif, CT_Comparison, CMPT_LE_0, VT_Float,
                        aux_float_le_zero_if_1__POST));
      aCT.push_back(DCT("aux_float_lt_zero_if_1__POST", t_VFVFF, RIT_Halfreif, CT_Comparison, CMPT_LT_0, VT_Float,
                        aux_float_lt_zero_if_1__POST));
      
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
          DBGOUT_MIPD ( "  MIssing declaration: " << aCT[i].sFuncName );
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
            // No not here because might be deleted immediately in later versions.
//             ic->remove();                              // mark removed at once
            assert( c->args().size() > 1 );
            ++MIPD__stats[ N_POSTs__all ];
            VarDecl* vd0 = expr2VarDecl(c->args()[0]);
            DBGOUT_MIPD__ ( "  Call " << c->id().str()
              << " uses variable " << vd0->id()->str() );
            if ( vd0->payload() == -1 ) {         // not yet visited
              vd0->payload( vVarDescr.size() );
              vVarDescr.push_back( VarDescr( vd0, vd0->type().isint() ) );  // can use /prmTypes/ as well
              // bounds/domains later for each involved var TODO
              if (vd0->e())
                checkInitExpr(vd0);
            } else {
              DBGOUT_MIPD__ ( " (already touched)" );
            }
            DBGOUT_MIPD ( "" );
            if ( equality_encoding__POST == c->decl() ) {
              assert( not vVarDescr[ vd0->payload() ].pEqEncoding );
              vVarDescr[ vd0->payload() ].pEqEncoding = &*ic;
              DBGOUT_MIPD ( " Variable " << vd0->id()->str() << " has eq_encode." );
            }   // + if has aux_ constraints?
            else
              vVarDescr[ vd0->payload() ].aCalls.push_back(&*ic);
          }
        }
      }
      MIPD__stats[ N_POSTs__varsDirect ] = vVarDescr.size();
    }
    
    // Should only be called on a newly added variable
    // OR when looking thru all non-touched vars
    /// Checks init expr of a variable
    /// Return true IFF new connection
    /// The bool param requires RHS to be POST-touched
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
          DBGOUT_MIPD__ ( "  Checking init expr  " );
          DBGOUT_MIPD_SELF( debugprint(vd) );
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
//           ArrayLit* al = c->args()[1]->dyn_cast<ArrayLit>();
          ArrayLit* al = follow_id(c->args()[1])->cast<ArrayLit>();
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
              DBGOUT_MIPD__ ( "  REG call " );
              DBGOUT_MIPD_SELF ( debugprint(vd) );
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

      MIPD__stats[ N_POSTs__varsInvolved ] = vVarDescr.size();
    }
    
    void propagateViews(bool &fChanges) {
      EnvI& env = getEnv()->envi();
      GCLock lock;
      
      // Iterate thru original 2-variable equalities to mark views:
      Model& mFlat = *getEnv()->flat();
      
      DBGOUT_MIPD ( "  CHECK ALL INITEXPR if they access a touched variable:" );
      for( VarDeclIterator ivd=mFlat.begin_vardecls(); ivd!=mFlat.end_vardecls(); ++ivd ) {
        if ( ivd->removed() )
          continue;
        if ( ivd->e()->e() and ivd->e()->payload()<0       // untouched
          and ( ivd->e()->type().isint() or ivd->e()->type().isfloat() ) )   // scalars
          if ( checkInitExpr(ivd->e(), true) )
            fChanges = true;
      }
        
      DBGOUT_MIPD ( "  CHECK ALL CONSTRAINTS for 2-var equations:" );
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
            ArrayLit* al = follow_id(c->args()[1])->cast<ArrayLit>();
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
                DBGOUT_MIPD ( "  REG call " );
                DBGOUT_MIPD_SELF ( debugprint(c) );
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
              DBGOUT_MIPD ( "  REG call " );
              DBGOUT_MIPD_SELF ( debugprint(c) );
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
      DBGOUT_MIPD ( "  Register 2-var connection: " << led );
      // register if new variables
//       std::vector<bool> fHaveClq(led.vd.size(), false);
      int nCliqueAvailable = -1;
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
          for ( auto& eq2 : clqOld ) {
            for ( auto vd : eq2.vd ) {    // point all the variables to the new clique
              vVarDescr[ vd->payload() ].nClique = nCliqueAvailable;
            }
          }
          clqNew.insert(clqNew.end(), clqOld.begin(), clqOld.end());
          clqOld.clear();                // Can use C++11 move      TODO
          DBGOUT_MIPD ( "    +++ Joining cliques" );
        }          
        nMaybeClq = nCliqueAvailable;  // Could mark as 'unused'  TODO
      }
    }
    
    /// Finds a clique variable to which all domain constr are related
    class TCliqueSorter {
      MIPD& mipd;
      const int iVarStart;   // this is the first var to which all others are related
    public:
//       VarDecl* varRef0=0;  // this is the first var to which all others are related
      VarDecl* varRef1=0;  // this is the 2nd main reference.
        // it is a var with eq_encode, or
        // an (integer if any) variable with the least rel. factor
      bool fRef1HasEqEncode=false;
      /// This map stores the relations y = ax+b of all the clique's vars to y
      typedef UNORDERED_NAMESPACE::unordered_map<VarDecl*, std::pair<double, double> >
        TMapVars;
      TMapVars mRef0, mRef1;   // to the main var 0, 1
      
      class TMatrixVars : public UNORDERED_NAMESPACE::unordered_map<VarDecl*, TMapVars> {
      public:
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
              assert_soft ( std::fabs( A ) > 1e-12,
                " Very small coef: "
                  << (*begV)->id()->str() << " = "
                  << A << " * " << (*(begV+1))->id()->str()
                  << " + " << B );
              if ( fReportRepeat )
                assert_soft ( 0, "LinEqGraph: eqn between "
                  << (*begV)->id()->str() << " and " << (*(begV+1))->id()->str()
                  << " is repeated. " );
              return true;
            }
          }
          return false;
        }
      };
      
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
          assert_soft( std::fabs( *begC ) >= 1e-10,
            "  Vars " << (*begV)->id()->str() <<
            "  to " << (*(begV+1))->id()->str() <<
            ": coef=" << (*begC) );
          // Transform Ax+By=C into x = -B/Ay+C/A
          const double negBA = -(*(begC+1))/(*begC);
          const double CA = rhs/(*begC);
          checkExistingArc(begV, negBA, CA);
          (*this)[*begV][*(begV+1)] = std::make_pair(negBA, CA);
        }
        void addEdge(const LinEq2Vars& led) {
          addArc( led.coefs.begin(), led.vd.begin(), led.rhs );
          addArc( led.coefs.rbegin(), led.vd.rbegin(), led.rhs );
        }
        /// Propagate linear relations from the given variable
        void propagate(iterator itStart, TMapVars& mWhereStore) {
          assert( this->end()!=itStart );
          TMatrixVars mTemp;
          mTemp[itStart->first] = itStart->second;       // init with existing
          DBGOUT_MIPD ( "Propagation started from "
            << itStart->first->id()->str()
            << "  having " << itStart->second.size() << " connections" );
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
              if ( not mWhereStore.checkExistingArc(vd, A1A2, A1B2plusB1, false) ) {
                mWhereStore[vd[0]][vd[1]] = std::make_pair(A1A2, A1B2plusB1);
                DBGOUT_MIPD ( "   PROPAGATING: "
                  << vd[0]->id()->str() << " = "
                  << A1A2 << " * " << vd[1]->id()->str()
                  << " + " << A1B2plusB1 );
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
        for ( auto& eq2 : clq ) {
          leg.addEdge(eq2);
        }
        DBGOUT_MIPD ( " Clique " << mipd.vVarDescr[iVarStart].nClique
          << ": " << leg.size() << " variables, "
          << clq.size() << " connections." );
        for ( auto it1=leg.begin(); it1!=leg.end(); ++it1 )
          mipd.vVarDescr[ it1->first->payload() ].fDomainConstrProcessed = true;
        
        // Propagate the 1st var's relations:
        leg.propagate(leg.begin(), mRef0);
        
        // Find a best main variable according to:
        // 1. isInt 2. hasEqEncode 3. abs linFactor to ref0
        varRef1 = leg.begin()->first;
        std::array<double, 3> aCrit = { { (double)mipd.vVarDescr[varRef1->payload()].fInt,
          (double)(bool)mipd.vVarDescr[varRef1->payload()].pEqEncoding, 1.0 } };
        for ( auto it2=mRef0.begin(); it2!=mRef0.end(); ++it2 ) {
          VarDescr& vard = mipd.vVarDescr[ it2->first->payload() ];
          std::array<double, 3> aCrit1 =
            { { (double)vard.fInt, (double)(bool)vard.pEqEncoding, std::fabs(it2->second.first) } };
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
      SetOfIntvReal sDomain = { IntvReal() };   // the decomposed domain. Init to +-inf
      
      DomainDecomp(MIPD* pm, int iv) : mipd(*pm), iVarStart(iv), cls(pm, iv)  { }
      void doProcess() {
        // Choose the main variable and relate all others to it
        const int nClique =  mipd.vVarDescr[iVarStart].nClique;
        if ( nClique >= 0 ) {
          cls.doRelate();
        } else
          cls.varRef1 = mipd.vVarDescr[ iVarStart ].vd;
        // Adding itself:
        cls.mRef1[ cls.varRef1 ] = std::make_pair( 1.0, 0.0 );
        
        int iVarRef1 = cls.varRef1->payload();
        assert ( nClique ==  mipd.vVarDescr[iVarRef1].nClique );
        cls.fRef1HasEqEncode = mipd.vVarDescr[ iVarRef1 ].pEqEncoding;
        
        // First, construct the domain decomposition in any case
//         projectVariableConstr( cls.varRef1, std::make_pair(1.0, 0.0) );
//         if ( nClique >= 0 ) {
        for ( auto& iRef1 : cls.mRef1 ) {
          projectVariableConstr( iRef1.first, iRef1.second );
        }
        
        DBGOUT_MIPD( "Clique " << nClique
          << ": main ref var " <<cls.varRef1->id()->str()
          << ", domain dec: " << sDomain );
        
        if ( sDomain.empty() ) {
          std::ostringstream oss;
          oss <<  "Clique " << nClique
            << ": main ref var " <<cls.varRef1->id()->str()
            << ", domain dec: " << sDomain;
          throw oss.str();
        }
        
        assert( sDomain.checkFiniteBounds() );
        assert( sDomain.checkDisjunctStrict() );
        
        makeRangeDomains();
        
        // Then, use equality_encoding if available
        if ( cls.fRef1HasEqEncode ) {
          syncWithEqEncoding();
          syncOtherEqEncodings();          
        } else {  // not cls.fRef1HasEqEncode
          createDomainFlags();
        }
        implement__POSTs();
        
        // Statistics
        if ( sDomain.size() < MIPD__stats[ N_POSTs__domSizeMin ] )
          MIPD__stats[ N_POSTs__domSizeMin ] = sDomain.size();
        MIPD__stats[ N_POSTs__domSizeSum ] += sDomain.size();
        if ( sDomain.size() > MIPD__stats[ N_POSTs__domSizeMax ] )
          MIPD__stats[ N_POSTs__domSizeMax ] = sDomain.size();
        if ( cls.fRef1HasEqEncode )
          ++MIPD__stats[ N_POSTs__cliquesWithEqEncode ];
      }
      
      /// Project the domain-related constraints of a variable into the clique
      /// Deltas should be scaled but to a minimum of the target's discr
      /// COmparison sense changes on negated vars
      void projectVariableConstr( VarDecl* vd, std::pair<double, double> eq1 ) {
        DBGOUT_MIPD__( "  MIPD: projecting variable  " );
        DBGOUT_MIPD_SELF( debugprint(vd) );
        // Always check if domain becomes empty?         TODO
        const double A = eq1.first;                     // vd = A*arg + B.  conversion
        const double B = eq1.second;
        // process domain info
        double lb=B, ub=A+B;  // projected bounds for bool
        if ( vd->ti()->domain() ) {
          if ( vd->type().isint() ) {         // INT VAR
            SetOfIntvReal sD1;
            convertIntSet( vd->ti()->domain(), sD1, cls.varRef1, A, B );
            sDomain.intersect(sD1);
            DBGOUT_MIPD( " Clique domain after proj of the init. domain "
              << sD1 << " of varint "
              << A << " * " << vd->id()->str() << " + " << B
              << ":  " << sDomain );
            auto bnds = sD1.getBounds();
            lb = bnds.left;
            ub = bnds.right;
          }
          else if ( vd->type().isfloat() ) {  // FLOAT VAR
            BinOp* bo = vd->ti()->domain()->cast<BinOp>();
            FloatVal vmin0 = eval_float(mipd.getEnv()->envi(), bo->lhs());
            FloatVal vmax0 = eval_float(mipd.getEnv()->envi(), bo->rhs());
            if ( A < 0.0 )
              std::swap( vmin0, vmax0 );
            lb = rndUpIfInt( cls.varRef1, A * vmin0 + B );
            ub = rndDownIfInt( cls.varRef1, A * vmax0 + B );
            SetOfIntvReal intv = { IntvReal( lb, ub ) };
            sDomain.intersect( intv );               // *A + B
            DBGOUT_MIPD( " Clique domain after proj of the init. domain "
              << intv << " of varfloat "
              << A << " * " << vd->id()->str() << " + " << B
              << ":  " << sDomain );
          } else {
            std::ostringstream oss;
            oss << "Variable " << vd->id()->str()
              << " of type " << vd->type().toString()
              << " has a domain.";
            throw oss.str();
          }          
//           /// Deleting var domain:
//           vd->ti()->domain( NULL );
        }
        else {
          if ( NULL==vd->ti()->domain() && not vd->type().isbool() ) {
            lb = IntvReal::infMinus();
            ub = IntvReal::infPlus();
          }
        }
        auto bnds = sDomain.getBounds();              // can change    TODO
        // process calls. Can use the constr type info.
        auto& aCalls = mipd.vVarDescr[ vd->payload() ].aCalls;
        for ( Item* pItem : aCalls ) {
          ConstraintI* pCI = pItem->dyn_cast<ConstraintI>();
          assert( pCI );
          Call* pCall = pCI->e()->dyn_cast<Call>();
          assert( pCall );
          // check the bounds for bool in reifs?                     TODO
          auto ipct = mipd.mCallTypes.find( pCall->decl() );
          assert( mipd.mCallTypes.end() != ipct );
          const DCT& dct = *ipct->second;
          int nCmpType_ADAPTED = dct.nCmpType;
          if ( A < 0.0 ) {                                       // negative factor
            if ( std::abs( nCmpType_ADAPTED ) >= 4 )             // inequality
              nCmpType_ADAPTED = -nCmpType_ADAPTED;
          }
          switch ( dct.nConstrType ) {
            case CT_SetIn:
            {
              SetOfIntvReal SS;
              convertIntSet( pCall->args()[1], SS, cls.varRef1, A, B );
              if ( RIT_Static == dct.nReifType ) {
                sDomain.intersect(SS);
                ++MIPD__stats[ N_POSTs__setIn ];
              }
              else {
                sDomain.cutDeltas(SS, std::max( 1.0, std::fabs( A ) ) );      // deltas to scale
                ++MIPD__stats[ N_POSTs__setInReif ];
              }
            }
              break;
            case CT_Comparison:
              if ( RIT_Reif == dct.nReifType ) {
                const double rhs = ( mipd.aux_float_lt_zero_iff_1__POST == pCall->decl() )
                  ? B /* + A*0.0, relating to 0 */
                  // The 2nd argument is constant:
                  : A * mipd.expr2Const( pCall->args()[1] ) + B;
                const double rhsUp = rndUpIfInt( cls.varRef1, rhs );
                const double rhsDown = rndDownIfInt( cls.varRef1, rhs );
                const double rhsRnd = rndIfInt( cls.varRef1, rhs );
                /// Strictly, for delta we should finish domain reductions first...   TODO?
                const double delta = computeDelta( cls.varRef1, vd, bnds, A, pCall, 3 );
                switch ( nCmpType_ADAPTED ) {
                  case CMPT_LE:
                    sDomain.cutDeltas( { { IntvReal::infMinus(), rhsDown } }, delta );
                    break;
                  case CMPT_GE:
                    sDomain.cutDeltas( { { rhsUp, IntvReal::infPlus() } }, delta );
                    break;
                  case CMPT_LT_0:
                    sDomain.cutDeltas( { { IntvReal::infMinus(), rhsDown-delta } }, delta );
                    break;
                  case CMPT_GT_0:
                    sDomain.cutDeltas( { { rhsUp+delta, IntvReal::infPlus() } }, delta );
                    break;
                  case CMPT_EQ:
                    if ( not ( cls.varRef1->type().isint() &&    // skip if int target var
                        std::fabs( rhs - rhsRnd ) > 1e-8 ) )     // and fract value
                      sDomain.cutDeltas( { { rhsRnd, rhsRnd } }, delta );
                    break;
                  default:
                    assert( ( " No other reified cmp type ", 0 ) );
                }
                ++MIPD__stats[ ( vd->ti()->type().isint() ) ?
                  N_POSTs__intCmpReif : N_POSTs__floatCmpReif ];
              } else if ( RIT_Static == dct.nReifType ) {
                  // _ne, later maybe static ineq                                 TODO
                assert( CMPT_NE == dct.nCmpType );
                const double rhs = A * mipd.expr2Const( pCall->args()[1] ) + B;
                const double rhsRnd = rndIfInt( cls.varRef1, rhs );
                bool fSkipNE = ( cls.varRef1->type().isint() &&
                  std::fabs( rhs - rhsRnd ) > 1e-8 );
                if ( not fSkipNE ) {
                  const double delta = computeDelta( cls.varRef1, vd, bnds, A, pCall, 2 );
                  sDomain.cutOut( { rhsRnd-delta, rhsRnd+delta } );
                }
                ++MIPD__stats[ ( vd->ti()->type().isint() ) ?
                  N_POSTs__intNE : N_POSTs__floatNE ];
              } else {  // aux_ relate to 0.0
                        // But we don't modify domain splitting for them currently
                assert ( RIT_Halfreif==dct.nReifType );
//                 const double rhs = B;               // + A*0
//                 const double delta = vd->type().isint() ? 1.0 : 1e-5;           // TODO : eps
              }
              break;
            case CT_Encode:
              // See if any further constraints here?                             TODO
              break;
            default:
              assert( ("Unknown constraint type", 0 ) );
          }
        }
        DBGOUT_MIPD( " Clique domain after proj of "
          << A << " * " << vd->id()->str() << " + " << B
          << ":  " << sDomain );
      }
      
      static double rndIfInt( VarDecl* vdTarget, double v ) {
        return vdTarget->type().isint() ? std::round( v ) : v;
      }
      static double rndUpIfInt( VarDecl* vdTarget, double v ) {
        return vdTarget->type().isint() ? std::ceil( v-1e-8 ) : v;
      }
      static double rndDownIfInt( VarDecl* vdTarget, double v ) {
        return vdTarget->type().isint() ? std::floor( v+1e-8 ) : v;
      }
      
      void makeRangeDomains() {
        auto bnds = sDomain.getBounds();
        for ( auto& iRef1 : cls.mRef1 ) {
          VarDecl* vd = iRef1.first;
          if ( vd->type().isint() ) {
            // projecting the bounds back:
            double lb0 = ( bnds.left - iRef1.second.second ) / iRef1.second.first;
            double ub0 = ( bnds.right - iRef1.second.second ) / iRef1.second.first;
            if ( lb0 > ub0 ) {
              assert( iRef1.second.first < 0.0 );
              std::swap( lb0, ub0 );
            }
            const double lb = rndUpIfInt( vd, lb0 );
            const double ub = rndDownIfInt( vd, ub0 );
            setVarDomain( vd, lb, ub);
          }
        }
      }
      
      /// tightens element bounds in the existing eq_encoding of varRef1
      /// Can also back-check from there    TODO
      /// And further checks                TODO
      void syncWithEqEncoding() {
      }
      
      /// sync varRef1's eq_encoding with those of other variables
      void syncOtherEqEncodings() {
        // TODO  This could be in the var projection? No, need the final domain
      }
      
      /// if not eq_encoding, creates a flag for each subinterval in the domain
      /// and constrains sum(flags)==1
      void createDomainFlags() {
        std::vector<Expression*> vVars( sDomain.size() );         // flags for each subinterval
        std::vector<double> vIntvLB( sDomain.size() + 1 ), vIntvUB__( sDomain.size() + 1 );
        int i=0;
        for ( auto& intv : sDomain ) {
          intv.varFlag = addIntVar( 0.0, 1.0 );
          vVars[i] = intv.varFlag->id();
          vIntvLB[i] = intv.left;
          vIntvUB__[i] = -intv.right;
          ++i;
        }
        // Sum of flags == 1
        std::vector<double> ones( sDomain.size(), 1.0 );
        addLinConstr( ones, vVars, CMPT_EQ, 1.0 );
        // Domain decomp
        vVars.push_back( cls.varRef1->id() );
        vIntvLB[i] = -1.0;                                 // var1 >= sum(LBi*flagi)
        vIntvUB__[i] = 1.0;                               // var1 <= sum(UBi*flagi)
        addLinConstr( vIntvLB, vVars, CMPT_LE, 0.0 );
        addLinConstr( vIntvUB__, vVars, CMPT_LE, 0.0 );
      }
      
      /// deletes them as well
      void implement__POSTs() {
        auto bnds = sDomain.getBounds();
        for ( auto& iRef1 : cls.mRef1 ) {
//           DBGOUT_MIPD__( "  MIPD: implementing constraints of variable  " );
//           DBGOUT_MIPD_SELF( debugprint(vd) );
          VarDecl* vd = iRef1.first;
          auto eq1 = iRef1.second;
          const double A = eq1.first;                     // vd = A*arg + B.  conversion
          const double B = eq1.second;
          // process calls. Can use the constr type info.
          auto& aCalls = mipd.vVarDescr[ vd->payload() ].aCalls;
          for ( Item* pItem : aCalls ) {
            ConstraintI* pCI = pItem->dyn_cast<ConstraintI>();
            assert( pCI );
            Call* pCall = pCI->e()->dyn_cast<Call>();
            assert( pCall );
            DBGOUT_MIPD__( "IMPL CALL  " );
            DBGOUT_MIPD_SELF( debugprint( pCall ) );
            // check the bounds for bool in reifs?                     TODO
            auto ipct = mipd.mCallTypes.find( pCall->decl() );
            assert( mipd.mCallTypes.end() != ipct );
            const DCT& dct = *ipct->second;
            int nCmpType_ADAPTED = dct.nCmpType;
            if ( A < 0.0 ) {                                       // negative factor
              if ( std::abs( nCmpType_ADAPTED ) >= 4 )             // inequality
                nCmpType_ADAPTED = -nCmpType_ADAPTED;
            }
            switch ( dct.nConstrType ) {
              case CT_SetIn:
                if ( RIT_Reif == dct.nReifType )
                {
                  SetOfIntvReal SS;
                  convertIntSet( pCall->args()[1], SS, cls.varRef1, A, B );
                  relateReifFlag( pCall->args()[2], SS );
                }
                break;
              case CT_Comparison:
                if ( RIT_Reif == dct.nReifType ) {
                  const double rhs = ( mipd.aux_float_lt_zero_iff_1__POST == pCall->decl() )
                    ? B /* + A*0.0, relating to 0 */
                    // The 2nd argument is constant:
                    : A * mipd.expr2Const( pCall->args()[1] ) + B;
                  const double rhsUp = rndUpIfInt( cls.varRef1, rhs );
                  const double rhsDown = rndDownIfInt( cls.varRef1, rhs );
                  const double rhsRnd = rndIfInt( cls.varRef1, rhs );
                  const double delta = computeDelta( cls.varRef1, vd, bnds, A, pCall, 3 );
                  switch ( nCmpType_ADAPTED ) {
                    case CMPT_LE:
                      relateReifFlag( pCall->args()[2], { { IntvReal::infMinus(), rhsDown } } );
                      break;
                    case CMPT_GE:
                      relateReifFlag( pCall->args()[2], { { rhsUp, IntvReal::infPlus() } } );
                      break;
                    case CMPT_LT_0:
                      relateReifFlag( pCall->args()[1], { { IntvReal::infMinus(), rhsDown-delta } });
                      break;
                    case CMPT_GT_0:
                      relateReifFlag( pCall->args()[1],  { { rhsUp+delta, IntvReal::infPlus() } } );
                      break;
                    case CMPT_EQ:
                      relateReifFlag( pCall->args()[2], { { rhs, rhs } } );
                      break;
                    default:
                      break;
                  }
                } else if ( RIT_Static == dct.nReifType ) {
                    // nothing here for NE
                  assert( CMPT_NE == nCmpType_ADAPTED );
                } else {  // aux_ relate to 0.0
                          // But we don't modify domain splitting for them currently
                  assert ( RIT_Halfreif==dct.nReifType );
                  double rhs = B;               // + A*0
                  const double rhsUp = rndUpIfInt( cls.varRef1, rhs );
                  const double rhsDown = rndDownIfInt( cls.varRef1, rhs );
                  const double rhsRnd = rndIfInt( cls.varRef1, rhs );
                  double delta = 0.0;
                  if ( mipd.aux_float_lt_zero_if_1__POST==pCall->decl() )  // only float and lt
                    delta = computeDelta( cls.varRef1, vd, bnds, A, pCall, 2 );
                  if ( nCmpType_ADAPTED < 0 )
                    delta = -delta;
                  if ( cls.varRef1->type().isint() && CMPT_EQ_0!=nCmpType_ADAPTED ) {
                    if ( nCmpType_ADAPTED < 0 )
                      rhs = rhsDown;
                    else
                      rhs = rhsUp;
                  } else {
                    rhs += delta;
                  }
                  // Now we need rhs not to be in the inner of the domain
                  bool fUseDD = true;
                  if ( not cls.fRef1HasEqEncode ) {
                    auto itRhs = sDomain.lower_bound(rhs);
                    switch ( nCmpType_ADAPTED ) {
                      case CMPT_EQ_0:
                        fUseDD = ( itRhs->left==rhs && itRhs->right==rhs );  // exactly
                        break;
                      case CMPT_GT_0:
                      case CMPT_LT_0:
                      case CMPT_GE_0:
                      case CMPT_LE_0:
                      {
                        auto it2 = itRhs;
                        const bool fBegin = ( sDomain.begin()==it2 );
                        bool fInner = false;
                        if ( not fBegin ) {
                          --it2;
                          if ( it2->right > rhs )
                            fInner = true;
                        }
                        fUseDD = not fInner;
                      }
                        break;
                      default:
                        assert( ( "Unknown halfreif cmp type", 0 ) );
                    }
                  }
                  if ( fUseDD ) {               // use sDomain
                    if ( CMPT_EQ_0==nCmpType_ADAPTED ) {
                      relateReifFlag( pCall->args()[1], { { rhs, rhs } }, RIT_Halfreif );
                    } else if ( nCmpType_ADAPTED < 0 ) {
                      relateReifFlag( pCall->args()[1], { { IntvReal::infMinus(), rhsDown } }, RIT_Halfreif );
                    } else {
                      relateReifFlag( pCall->args()[1], { { rhsUp, IntvReal::infPlus() } }, RIT_Halfreif );
                    }
                  }  else {                         // use big-M
                    DBGOUT_MIPD( "   AUX BY BIG-Ms: " );
                    const bool fLE = ( CMPT_EQ_0==nCmpType_ADAPTED || 0>nCmpType_ADAPTED );
                    const bool fGE = ( CMPT_EQ_0==nCmpType_ADAPTED || 0<nCmpType_ADAPTED );
                    if ( fLE && rhs <= bnds.right ) {
                      if ( rhs > bnds.left ) {
                        std::vector<double> coefs = { 1.0, bnds.right-rhs };
                        std::vector<Expression*> vars = { pCall->args()[0], pCall->args()[1] };
                        addLinConstr( coefs, vars, CMPT_LE, bnds.right );
                      } else
                        setVarDomain( mipd.expr2VarDecl( pCall->args()[1] ), 0.0, 0.0 );
                    }
                    if ( fGE && rhs >= bnds.left ) {
                      if ( rhs < bnds.right ) {
                        std::vector<double> coefs = { -1.0, rhs-bnds.left };
                        std::vector<Expression*> vars = { pCall->args()[0], pCall->args()[1] };
                        addLinConstr( coefs, vars, CMPT_LE, -bnds.left );
                      } else
                        setVarDomain( mipd.expr2VarDecl( pCall->args()[1] ), 0.0, 0.0 );
                    }
                  }
                  ++MIPD__stats[ ( vd->ti()->type().isint() ) ?
                    N_POSTs__intAux : N_POSTs__floatAux ];
                }
                break;
              case CT_Encode:
                // See if any further constraints here?                             TODO
                break;
              default:
                assert( ("Unknown constraint type", 0 ) );
            }
            pItem->remove();                                       // removing the call
          }
          // removing the eq_encoding call
          if ( mipd.vVarDescr[ vd->payload() ].pEqEncoding ) 
            mipd.vVarDescr[ vd->payload() ].pEqEncoding->remove();
        }
      }
      
      /// sets varFlag = or <= sum( intv.varFlag : SS )
      void relateReifFlag( Expression* expFlag, const SetOfIntvReal& SS, EnumReifType nRT=RIT_Reif ) {
        assert( RIT_Reif==nRT or RIT_Halfreif==nRT );
        VarDecl* varFlag = mipd.expr2VarDecl(expFlag);
        std::vector<Expression*> vIntvFlags;
        if ( cls.fRef1HasEqEncode ) {                  // use eq_encoding
          assert( varFlag->type().isint() );
          std::vector<Expression*> pp;
          auto bnds = sDomain.getBounds();
          const long long iMin = mipd.expr2ExprArray(
            mipd.vVarDescr[ cls.varRef1->payload() ].pEqEncoding->e()->dyn_cast<Call>()->args()[1], pp );
          assert( pp.size() >= bnds.right-bnds.left+1 );
          assert( iMin<=bnds.left );
          for ( auto& intv : SS ) {
            for ( long long vv = (long long)std::max( double(iMin), intv.left );
                  vv <= (long long)std::min( double(iMin)+pp.size()-1, intv.right ); ++vv ) {
              vIntvFlags.push_back( pp[vv-iMin] );
            }
          }
        } else {
          for ( auto& intv : SS ) {
            auto it1 = sDomain.lower_bound( intv.left );
            auto it2 = sDomain.lower_bound( intv.right );
            auto it11 = it1;
            if ( sDomain.begin() != it11 ) {
              --it11;
              assert( it11->right <= intv.left );
            }
            auto it12 = it2;
            if ( sDomain.begin() != it12 ) {
              --it12;
              assert( it12->right <= intv.right );
            }
            for ( it12 = it1; it12 != it2; ++it12 ) {
              assert( it12->varFlag );
              vIntvFlags.push_back( it12->varFlag->id() );
            }
          }
        }
        if ( vIntvFlags.size() ) {
          // Could find out if reif is true                               -- TODO
          std::vector<double> onesm( vIntvFlags.size(), -1.0 );
          onesm.push_back( 1.0 );
          vIntvFlags.push_back( varFlag->id() );
          EnumCmpType nCmpType = ( RIT_Reif==nRT ) ? CMPT_EQ : CMPT_LE;
          addLinConstr( onesm, vIntvFlags, nCmpType, 0.0 );
        } else {                                    // the reif is false
          setVarDomain( varFlag, 0.0, 0.0 );
        }
      }
      
      void setVarDomain( VarDecl* vd, double lb, double ub ) {
        // need to check if the new range is in the previous bounds...   TODO
        assert( vd->type().isint() );                               // TODO
        SetLit* newDom = new SetLit(Location().introduce(),IntSetVal::a( lb, ub ));
//           TypeInst* nti = copy(mipd.getEnv()->envi(),varFlag->ti())->cast<TypeInst>();
//           nti->domain(newDom);
        vd->ti()->domain(newDom);
      }
      
      VarDecl* addIntVar(double LB, double UB) {
//         GCLock lock;
        // Cache them? Only location can be different                    TODO
        SetLit* newDom = new SetLit( Location().introduce(), IntSetVal::a( LB, UB ) );
        TypeInst* ti = new TypeInst(Location().introduce(),Type::varint(),newDom);
        VarDecl* newVar = new VarDecl(Location().introduce(),ti,mipd.getEnv()->envi().genId());
        mipd.getEnv()->envi().flat_addItem(new VarDeclI(Location().introduce(),newVar));
        return newVar;
      }
      
      void addLinConstr( std::vector<double>& coefs, std::vector<Expression*>& vars,
                         EnumCmpType nCmpType, double rhs ) {
        std::vector<Expression*> args(3);
        assert( vars.size() >= 2 );
        assert( coefs.size()==vars.size() );
        assert( CMPT_EQ==nCmpType || CMPT_LE==nCmpType );
        DBGOUT_MIPD_SELF( // LinEq leq; leq.coefs=coefs; leq.vd=vars; leq.rhs=rhs; 
          DBGOUT_MIPD__( " ADDING " << ( CMPT_EQ == nCmpType ? "LIN_EQ" : "LIN_LE" )
            << ": [ " );
          for ( auto c : coefs )
            DBGOUT_MIPD__( c << ',' );
          DBGOUT_MIPD__( " ] * [ " );
          for ( auto v : vars ) {
            if ( v->isa<Id>() )
              DBGOUT_MIPD__( v->dyn_cast<Id>()->str() << ',' );
            else if ( v->isa<VarDecl>() )
              throw "addLinConstr: only id's as variables allowed";
            else 
              DBGOUT_MIPD__( mipd.expr2Const(v) << ',' );
          }
          DBGOUT_MIPD( " ] " << ( CMPT_EQ == nCmpType ? "==" : "<=" ) << rhs );
                        );
        std::vector<Expression*> nc_c(coefs.size());
        std::vector<Expression*> nx(coefs.size());
        bool fFloat = not (*vars.begin())->type().isint();
        auto sName = constants().ids.float_.lin_eq; // "int_lin_eq";
        FunctionI* fDecl = mipd.float_lin_eq;
        if ( fFloat ) {                 // assert all vars of same type     TODO
          for ( int i=0; i<vars.size(); ++i ) {
            nc_c[i] = FloatLit::a( coefs[i] );
            nx[i] = vars[i];   // ->id();   once passing a general expression
          }
          args[2] = FloatLit::a(rhs);
          args[2]->type(Type::parfloat(0));
          args[0] = new ArrayLit(Location().introduce(),nc_c);
          args[0]->type(Type::parfloat(1));                // actually 1 int... TODO
          args[1] = new ArrayLit(Location().introduce(),nx);
          args[1]->type(Type::varfloat(1));
          if ( CMPT_LE==nCmpType ) {
            sName = constants().ids.float_.lin_le; // "float_lin_le";
            fDecl = mipd.float_lin_le;
          }
        } else {
          for ( int i=0; i<vars.size(); ++i ) {
            nc_c[i] = IntLit::a( coefs[i] );
            nx[i] = vars[i];  //->id();
          }
          args[2] = IntLit::a(rhs);
          args[2]->type(Type::parint(0));
          args[0] = new ArrayLit(Location().introduce(),nc_c);
          args[0]->type(Type::parint(1));
          args[1] = new ArrayLit(Location().introduce(),nx);
          args[1]->type(Type::varint(1));
          if ( CMPT_LE==nCmpType ) {
            sName = constants().ids.int_.lin_le; // "int_lin_le";
            fDecl = mipd.int_lin_le;
          } else {
            sName = constants().ids.int_.lin_eq; // "int_lin_eq";
            fDecl = mipd.int_lin_eq;
          }
        }
        if ( mipd.getEnv()->envi().map_end() != mipd.getEnv()->envi().map_find( args[0] ) ) {
          DBGOUT_MIPD__( " Found expr " );
          DBGOUT_MIPD_SELF( debugprint( args[0] ) );
        }
        auto nc = new Call(Location().introduce(),ASTString(sName),args);
        nc->type(Type::varbool());
        nc->decl(fDecl);
        mipd.getEnv()->envi().flat_addItem(new ConstraintI(Location().introduce(), nc));
      }
      
      /// domain / reif set of one variable into that for another
      void convertIntSet( Expression* e, SetOfIntvReal& s, VarDecl* varTarget, 
                          double A, double B ) {
        assert( A != 0.0 );
        IntSetVal* S = eval_intset( mipd.getEnv()->envi(), e );
        IntSetRanges domr(S);
        for (; domr(); ++domr) {                          // * A + B
          IntVal mmin = domr.min();
          IntVal mmax = domr.max();
          if ( A < 0.0 )
            std:: swap( mmin, mmax );
          s.insert( IntvReal(                   // * A + B
            mmin.isFinite() ?
              rndUpIfInt( varTarget, (mmin.toInt() * A + B) ) : IntvReal::infMinus(),
            mmax.isFinite() ?
              rndDownIfInt( varTarget, (mmax.toInt() * A + B) ) : IntvReal::infPlus() )
          );
        }
      }
      
      /// compute the delta for float strict ineq
      double computeDelta( VarDecl* var, VarDecl* varOrig, IntvReal bnds,
                           double A, Call* pCall, int nArg ) {
        double delta = varOrig->type().isfloat() ? 
          mipd.expr2Const( pCall->args()[nArg] ) * ( bnds.right-bnds.left )
          : std::fabs( A ) ;           // delta should be scaled as well
        if ( var->type().isint() )  // the projected-onto variable
          delta = std::max( 1.0, delta );
        return delta;
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
    ///
    /// REMARKS.
    /// Not impose effects of integrality scaling (e.g., int v = int k/3)
    /// BUT when using k's eq_encode?
    /// And when subdividing into intervals
    bool decomposeDomains() {
//       for (int iClq=0; iClq<aCliques.size(); ++iClq ) {
//         TClique& clq = aCliques[iClq];
//       }
      for ( int iVar=0; iVar<vVarDescr.size(); ++iVar ) {
//         VarDescr& var = vVarDescr[iVar];
        if ( not vVarDescr[iVar].fDomainConstrProcessed ) {
          try {
            GCLock lock;      
            DomainDecomp dd(this, iVar);
            dd.doProcess();
            vVarDescr[iVar].fDomainConstrProcessed = true;
          } catch (const std::string& str) {
            std::cerr << "  ERROR: " << str << std::endl;
            return false;
//           } catch ( ... ) {  // a contradiction
//             return false;
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
      
    /// Fills the vector of vardecls and returns the least index of the array
    template <class Array>
    long long expr2DeclArray(Expression* arg, Array& aVD) {
      ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
      checkOrResize( aVD, al->v().size() );
      for (unsigned int i=0; i<al->v().size(); i++)
        aVD[i] = expr2VarDecl(al->v()[i]);
      return al->min(0);
    }
    
    /// Fills the vector of expressions and returns the least index of the array
    template <class Array>
    long long expr2ExprArray(Expression* arg, Array& aVD) {
      ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
      checkOrResize( aVD, al->v().size() );
      for (unsigned int i=0; i<al->v().size(); i++)
        aVD[i] = ( al->v()[i] );
      return al->min(0);
    }

    double expr2Const(Expression* arg) {
      if (IntLit* il = arg->dyn_cast<IntLit>()) {
        return ( il->v().toInt() );
      } else if (FloatLit* fl = arg->dyn_cast<FloatLit>()) {
        return ( fl->v() );
      } else if (BoolLit* bl = arg->dyn_cast<BoolLit>()) {
        return ( bl->v() );
      } else {
        std::stringstream oss;
        oss << "unexpected expression instead of an int/float/bool literal: eid="
          << arg->eid() << " while E_INTLIT=" << Expression::E_INTLIT;
        throw oss.str();
      }
      return 0.0;
    }
    
    template <class Container, class Elem=int, size_t =0>
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
      if ( aCliques.empty() )
        return;
      int nc=0;
      for ( auto& cl : aCliques )
        if ( cl.size() )
          ++nc;
//       os << "N cliques " << aCliques.size() << "  total, "
//          << nc << " final" << std::endl;
      assert( nc );
      double nPDomSizeAve = MIPD__stats[ N_POSTs__domSizeSum ] / nc;
      os
      << MIPD__stats[ N_POSTs__all ] << " constr, "
      << MIPD__stats[ N_POSTs__varsDirect ] << " / "
      << MIPD__stats[ N_POSTs__varsInvolved ] << " vars, "
      << nc << " cliques, "
      << MIPD__stats[ N_POSTs__domSizeMin ] << " / "
      << nPDomSizeAve << " / "
      << MIPD__stats[ N_POSTs__domSizeMax ] << " dom size m/a/m, "
      << MIPD__stats[ N_POSTs__cliquesWithEqEncode ] << " clq eq_encoded, det.constr. "
//       << std::flush
      ;
      for ( int i=N_POSTs__intCmpReif; i<=N_POSTs__floatAux; ++i )
        os << MIPD__stats[ i ] << ',';
      os << " ... ";
    }

  };  // class MIPD
  
  template <class N> template <class N1>
  void SetOfIntervals<N>::intersect(const SetOfIntervals<N1>& s2) {
    if ( s2.empty() ) {
      this->clear();
      return;
    }
    this->cutOut( Interval<N>( Interval<N>::infMinus(), (N)s2.begin()->left ) );
    for ( auto is2=s2.begin(); is2!=s2.end(); ++is2 ) {
      auto is2next = is2;
      ++is2next;
      this->cutOut( Interval<N>( is2->right,
        s2.end()==is2next ? Interval<N>::infPlus() : (N)is2next->left ) );
    }
  }
  template <class N> template <class N1>
  void SetOfIntervals<N>::cutDeltas( const SetOfIntervals<N1>& s2, N1 delta ) {
    if ( this->empty() )
      return;
    // What if distance < delta?                 TODO
    for ( auto is2 : s2 ) {
      if ( is2.left > Interval<N1>::infMinus() )
        this->cutOut( Interval<N>( is2.left-delta, is2.left ) );
      if ( is2.right < Interval<N1>::infPlus() )
        this->cutOut( Interval<N>( is2.right, is2.right+delta ) );
    }
  }
  template <class N>
  void SetOfIntervals<N>::cutOut(const Interval<N>& intv) {
    DBGOUT_MIPD__( "Cutting " << intv
      << " from " << (*this) );
    if ( this->empty() )
      return;
    iterator it1 = ( Interval<N>::infMinus() == intv.left ) ?
      this->lower_bound( Interval<N>( intv.left, intv.right ) ) :
      this->upper_bound( Interval<N>( intv.left, intv.right ) );
    auto it2Del1 = it1;                                     // from which to delete
    if ( this->begin() != it1 ) {
      --it1;
      const N it1l = it1->left;
      assert( it1l <= intv.left );
      if ( it1->right > intv.left ) {                       // split it
        it2Del1 = split( it1, intv.left ).second;
//         it1->right = intv.left;  READ-ONLY
//         this->erase(it1);
//         it1 = this->end();
//         auto iR = this->insert( Interval<N>( it1l, intv.left ) );
//         assert( iR.second );
      }
    }
    DBGOUT_MIPD__( "; after split 1: " << (*this) );
    // Processing the right end:
    auto it2 = this->lower_bound( Interval<N>( intv.right, intv.right+1 ) );
    auto it2Del2 = it2;
    if ( this->begin() != it2 ) {
      --it2;
      assert( it2->left < intv.right );
      const N it2r = it2->right;
      if ( ( Interval<N>::infPlus() == intv.right ) ?
        ( it2r > intv.right ) : ( it2r >= intv.right ) ) {   // >=: split it
//         it2Del2 = split( it2, intv.right ).second;
          this->erase(it2);
          it2 = this->end();
          it2Del2 = this->insert( Interval<N>( intv.right, it2r ) );
      }
    }
    DBGOUT_MIPD__( "; after split 2: " << (*this) );
    DBGOUT_MIPD__( "; cutting out: " << SetOfIntervals(it2Del1, it2Del2) );
    this->erase( it2Del1,  it2Del2 );
    DBGOUT_MIPD( " ... gives " << (*this) );
  }
  template <class N> typename
  SetOfIntervals<N>::SplitResult SetOfIntervals<N>::split(iterator& it, N pos) {
    assert( pos>=it->left );
    assert( pos<=it->right );
    Interval<N> intvOld = *it;
    this->erase(it);
    iterator it_01 = this->insert( Interval<N> ( intvOld.left, pos ) );
    iterator it_02 = this->insert( Interval<N> ( pos, intvOld.right ) );
    it = this->end();
    return std::make_pair( it_01, it_02 );
  }
  template <class N>
  Interval<N> SetOfIntervals<N>::getBounds() const {
    if ( this->empty() )
      return Interval<N>( Interval<N>::infPlus(), Interval<N>::infMinus() );
    iterator it2 = this->end();
    --it2;
    return Interval<N>( this->begin()->left, it2->right );
  }
  template <class N>
  bool SetOfIntervals<N>::checkFiniteBounds() {
    if ( this->empty() )
      return false;
    auto bnds = getBounds();
    return bnds.left > Interval<N>::infMinus()
      && bnds.right < Interval<N>::infPlus();      
  }
  /// Check there are no useless interval splittings
  template <class N>
  bool SetOfIntervals<N>::checkDisjunctStrict() {
    for ( auto it=this->begin(); it!=this->end(); ++it ) {
      if ( it->left > it->right )
        return false;
      if ( this->begin() != it ) {
        auto it_1 = it;
        --it_1;
        if ( it_1->right >= it->left )
          return false;
      }
    }
    return true;
  }

  void MIPdomains(Env& env) {
    MIPD mipd(&env);
    if ( not mipd.MIPdomains() ) {
//       GCLock lock;
//       env.flat()->fail(env.envi());
    }
  }
  
}  // namespace MiniZinc
