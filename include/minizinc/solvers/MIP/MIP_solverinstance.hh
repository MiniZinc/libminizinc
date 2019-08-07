/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_MIP_SOLVER_INSTANCE_H__
#define __MINIZINC_MIP_SOLVER_INSTANCE_H__

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

namespace MiniZinc {
  
  // can be redefined as compilation parameter
#ifndef GETMIPWRAPPER     
  #define GETMIPWRAPPER MIP_WrapperFactory::GetDefaultMIPWrapper()
#endif
  
  class MIP_solver {
    public:
      typedef MIP_wrapper::VarId Variable;
      typedef MiniZinc::Statistics Statistics;
  };
  
  /// Generic cut generator
  /// Callback should be able to produce previously generated cuts again if needed [Gurobi]
  class CutGen {
  public:
    virtual ~CutGen() { }
    /// Say what type of cuts
    virtual int getMask() = 0;
    /// Adds new cuts to the 2nd parameter
    virtual void generate(const MIP_wrapper::Output&, MIP_wrapper::CutInput&) = 0;
    virtual void print( std::ostream& ) { }
  };

  /// XBZ cut generator
  class XBZCutGen : public CutGen {
    XBZCutGen() { }
    MIP_wrapper* pMIP=0;
  public:
    XBZCutGen( MIP_wrapper* pw ) : pMIP(pw) { }
    std::vector<MIP_wrapper::VarId> varX, varB;
    /// Say what type of cuts
    virtual int getMask() { return MIP_wrapper::MaskConsType_Usercut; }
    MIP_wrapper::VarId varZ;
    void generate(const MIP_wrapper::Output&, MIP_wrapper::CutInput&);
    void print( std::ostream& );
  };
  
  /// SEC cut generator for circuit
  class SECCutGen : public CutGen {
    SECCutGen() { }
    MIP_wrapper* pMIP=0;
  public:
    SECCutGen( MIP_wrapper* pw ) : pMIP(pw) { }
    /// Say what type of cuts
    virtual int getMask() { return MIP_wrapper::MaskConsType_Lazy | MIP_wrapper::MaskConsType_Usercut; }
    std::vector<MIP_wrapper::VarId> varXij;
    int nN=0;                        // N nodes
    /// returns error message if fails
    std::string validate() const;
    void generate(const MIP_wrapper::Output&, MIP_wrapper::CutInput&);
    void print( std::ostream& );
  };

  template<class MIPWrapper>
  class MIP_solverinstance : public SolverInstanceImpl<MIP_solver> {
    using SolverInstanceBase::_log;
    protected:
      
      const std::unique_ptr<MIP_wrapper> mip_wrap;
      std::vector< std::unique_ptr<CutGen> > cutGenerators;
      
    public:
      void registerCutGenerator( std::unique_ptr<CutGen>&& pCG ) {
        getMIPWrapper()->cbui.cutMask |= pCG->getMask();
        cutGenerators.push_back( move( pCG ) );
      }
      
    public:
      double lastIncumbent;
      double dObjVarLB=-1e300, dObjVarUB=1e300;
    public:

      MIP_solverinstance(Env& env, std::ostream& log, typename MIPWrapper::Options* opt) :
        SolverInstanceImpl(env,log,opt),
        mip_wrap(new MIPWrapper(opt))
      {
        assert(mip_wrap.get()); 
        registerConstraints();
      }
      virtual MIP_wrapper* getMIPWrapper() const { return mip_wrap.get(); }

      virtual Status next(void) { assert(0); return SolverInstance::UNKNOWN; }
      virtual void processFlatZinc(void);
      virtual void processWarmstartAnnotations( const Annotation& ann );
      virtual void processSearchAnnotations( const Annotation& ann );
      virtual Status solve(void);
      virtual void resetSolver(void) { }
      
      virtual void genCuts
        ( const MIP_wrapper::Output& , MIP_wrapper::CutInput& , bool fMIPSol);

//       void assignSolutionToOutput();   // needs to be public for the callback?
      virtual void printStatistics(bool fLegend=0);
      virtual void printStatisticsLine(bool fLegend=0);

    public:
      /// creates a var for a literal, if necessary
      VarId exprToVar(Expression* e);
      void exprToArray(Expression* e, std::vector<double> &vals);
      void exprToVarArray(Expression* e, std::vector<VarId> &vars);
      std::pair<double,bool> exprToConstEasy(Expression* e);
      double exprToConst(Expression* e);

      Expression* getSolutionValue(Id* id);

      void registerConstraints(void);
  };  // MIP_solverinstance

  template<class MIPWrapper>
  class MIP_SolverFactory: public SolverFactory {
  public:
    MIP_SolverFactory(void);
    SolverInstanceBase::Options* createOptions(void) { return new typename MIPWrapper::Options; }
    SolverInstanceBase* doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt)   {
      return new MIP_solverinstance<MIPWrapper>(env,log,static_cast<typename MIPWrapper::Options*>(opt));
    }
    bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv);
    std::string getDescription(SolverInstanceBase::Options* opt=NULL);
    std::string getVersion(SolverInstanceBase::Options* opt=NULL);
    std::string getId( );
    void printHelp(std::ostream& os) { MIPWrapper::Options::printHelp(os); }
  };

}

#include <minizinc/solvers/MIP/MIP_solverinstance.hpp>

#endif  // __MINIZINC_MIP_SOLVER_INSTANCE_H__
