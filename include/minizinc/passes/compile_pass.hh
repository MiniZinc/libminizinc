/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_COMPILE_PASS_HH__
#define __MINIZINC_COMPILE_PASS_HH__

#include <minizinc/flatten.hh>
#include <minizinc/options.hh>

namespace MiniZinc {

  struct CompilePassFlags {
    bool noMIPdomains;
    bool verbose;
    bool statistics;
    bool optimize;
    bool chain_compression;
    bool newfzn;
    bool werror;
    bool model_check_only;
    bool model_interface_only;
    bool allow_multi_assign;
  };

  class CompilePass : public Pass {
    private:
      Env* env;
      FlatteningOptions fopts;
      CompilePassFlags compflags;
      std::string library;
      std::vector<std::string> includePaths;
      bool change_library;
      bool ignore_unknown_ids;

    public:
      CompilePass(Env* e,
          FlatteningOptions& opts,
          CompilePassFlags& cflags,
          std::string globals_library,
          std::vector<std::string> include_paths,
          bool change_lib,
          bool ignore_unknown);

      Env* run(Env* env, std::ostream& log);
      ~CompilePass();
  };

}

#endif
