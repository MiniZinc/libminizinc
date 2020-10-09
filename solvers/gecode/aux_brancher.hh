/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)
 *     Andrea Rendl (andrea.rendl@nicta.com.au)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solvers/gecode_solverinstance.hh>

/**
 * \brief Branching on the introduced variables
 *
 * This brancher makes sure that when a solution is found for the model
 * variables, all introduced variables are either assigned, or the solution
 * can be extended to a solution of the introduced variables.
 *
 * The advantage over simply branching over the introduced variables is that
 * only one such extension will be searched for, instead of enumerating all
 * possible (equivalent) extensions.
 *
 */
namespace MiniZinc {

class AuxVarBrancher : public Gecode::Brancher {
protected:
  /// Flag whether brancher is done
  bool done;
  /// Construct brancher
  AuxVarBrancher(Gecode::Home home, const Gecode::TieBreak<Gecode::IntVarBranch>& int_varsel0,
                 const Gecode::IntValBranch& int_valsel0,
                 const Gecode::TieBreak<Gecode::BoolVarBranch>& bool_varsel0,
                 const Gecode::BoolValBranch& bool_valsel0
#ifdef GECODE_HAS_SET_VARS
                 ,
                 const Gecode::SetVarBranch& set_varsel0, const Gecode::SetValBranch& set_valsel0
#endif
#ifdef GECODE_HAS_FLOAT_VARS
                 ,
                 const Gecode::TieBreak<Gecode::FloatVarBranch>& float_varsel0,
                 const Gecode::FloatValBranch& float_valsel0
#endif
                 )
      : Brancher(home),
        done(false),
        int_varsel(int_varsel0),
        int_valsel(int_valsel0),
        bool_varsel(bool_varsel0),
        bool_valsel(bool_valsel0)
#ifdef GECODE_HAS_SET_VARS
        ,
        set_varsel(set_varsel0),
        set_valsel(set_valsel0)
#endif
#ifdef GECODE_HAS_FLOAT_VARS
        ,
        float_varsel(float_varsel0),
        float_valsel(float_valsel0)
#endif
  {
  }
  /// Copy constructor
  AuxVarBrancher(Gecode::Space& home, AuxVarBrancher& b) : Brancher(home, b), done(b.done) {}

  /// %Choice that only signals failure or success
  class Choice : public Gecode::Choice {
  public:
    /// Whether brancher should fail
    bool fail;
    /// Initialize choice for brancher \a b
    Choice(const Brancher& b, bool fail0) : Gecode::Choice(b, 1), fail(fail0) {}
    /// Report size occupied
    virtual size_t size() const { return sizeof(Choice); }
    /// Archive into \a e
    virtual void archive(Gecode::Archive& e) const {
      Gecode::Choice::archive(e);
      e.put(fail);
    }
  };

  Gecode::TieBreak<Gecode::IntVarBranch> int_varsel;
  Gecode::IntValBranch int_valsel;
  Gecode::TieBreak<Gecode::BoolVarBranch> bool_varsel;
  Gecode::BoolValBranch bool_valsel;
#ifdef GECODE_HAS_SET_VARS
  Gecode::SetVarBranch set_varsel;
  Gecode::SetValBranch set_valsel;
#endif
#ifdef GECODE_HAS_FLOAT_VARS
  Gecode::TieBreak<Gecode::FloatVarBranch> float_varsel;
  Gecode::FloatValBranch float_valsel;
#endif

public:
  /// Check status of brancher, return true if alternatives left.
  virtual bool status(const Gecode::Space& _home) const {
    if (done) return false;
    const MiniZinc::FznSpace& home = static_cast<const MiniZinc::FznSpace&>(_home);
    for (int i = 0; i < home.ivAux.size(); i++)
      if (!home.ivAux[i].assigned()) return true;
    for (int i = 0; i < home.bvAux.size(); i++)
      if (!home.bvAux[i].assigned()) return true;
#ifdef GECODE_HAS_SET_VARS
    for (int i = 0; i < home.svAux.size(); i++)
      if (!home.svAux[i].assigned()) return true;
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    for (int i = 0; i < home.fvAux.size(); i++)
      if (!home.fvAux[i].assigned()) return true;
#endif
    // No non-assigned variables left
    return false;
  }
  /// Return choice
  virtual Choice* choice(Gecode::Space& home) {
    done = true;
    MiniZinc::FznSpace& fzs = static_cast<MiniZinc::FznSpace&>(*home.clone());
    fzs.copyAuxVars = false;
    Gecode::branch(fzs, fzs.ivAux, int_varsel, int_valsel);
    Gecode::branch(fzs, fzs.bvAux, bool_varsel, bool_valsel);
#ifdef GECODE_HAS_SET_VARS
    Gecode::branch(fzs, fzs.svAux, set_varsel, set_valsel);
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    Gecode::branch(fzs, fzs.fvAux, float_varsel, float_valsel);
#endif
    Gecode::Search::Options opt;
    opt.clone = false;
    MiniZinc::FznSpace* sol = Gecode::dfs(&fzs, opt);
    if (sol) {
      delete sol;
      return new Choice(*this, false);
    } else {
      return new Choice(*this, true);
    }
  }
  /// Return choice
  virtual Choice* choice(const Gecode::Space&, Gecode::Archive& e) {
    bool fail;
    e >> fail;
    return new Choice(*this, fail);
  }
  /// Perform commit for choice \a c
  virtual Gecode::ExecStatus commit(Gecode::Space&, const Gecode::Choice& c, unsigned int) {
    return static_cast<const Choice&>(c).fail ? Gecode::ES_FAILED : Gecode::ES_OK;
  }
  /// Print explanation
  virtual void print(const Gecode::Space&, const Gecode::Choice& c, unsigned int,
                     std::ostream& o) const {
    o << "FlatZinc(" << (static_cast<const Choice&>(c).fail ? "fail" : "ok") << ")";
  }
  /// Copy brancher
  virtual Actor* copy(Gecode::Space& home) { return new (home) AuxVarBrancher(home, *this); }
  /// Post brancher
  static void post(Gecode::Home home, const Gecode::TieBreak<Gecode::IntVarBranch>& int_varsel,
                   const Gecode::IntValBranch& int_valsel,
                   const Gecode::TieBreak<Gecode::BoolVarBranch>& bool_varsel,
                   const Gecode::BoolValBranch& bool_valsel
#ifdef GECODE_HAS_SET_VARS
                   ,
                   const Gecode::SetVarBranch& set_varsel, const Gecode::SetValBranch& set_valsel
#endif
#ifdef GECODE_HAS_FLOAT_VARS
                   ,
                   const Gecode::TieBreak<Gecode::FloatVarBranch>& float_varsel,
                   const Gecode::FloatValBranch& float_valsel
#endif
  ) {
    (void)new (home) AuxVarBrancher(home, int_varsel, int_valsel, bool_varsel, bool_valsel
#ifdef GECODE_HAS_SET_VARS
                                    ,
                                    set_varsel, set_valsel
#endif
#ifdef GECODE_HAS_FLOAT_VARS
                                    ,
                                    float_varsel, float_valsel
#endif
    );
  }
  /// Delete brancher and return its size
  virtual size_t dispose(Gecode::Space&) { return sizeof(*this); }
};

}  // namespace MiniZinc
