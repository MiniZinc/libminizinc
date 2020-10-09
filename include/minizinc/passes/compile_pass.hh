/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flatten.hh>

namespace MiniZinc {

struct CompilePassFlags {
  bool noMIPdomains;
  bool verbose;
  bool statistics;
  bool optimize;
  bool chainCompression;
  bool newfzn;
  bool werror;
  bool modelCheckOnly;
  bool modelInterfaceOnly;
  bool allowMultiAssign;
};

class CompilePass : public Pass {
private:
  Env* _env;
  FlatteningOptions _fopts;
  CompilePassFlags _compflags;
  std::string _library;
  std::vector<std::string> _includePaths;
  bool _changeLibrary;
  bool _ignoreUnknownIds;

public:
  CompilePass(Env* e, FlatteningOptions& opts, CompilePassFlags& cflags,
              std::string globals_library, std::vector<std::string> include_paths, bool change_lib,
              bool ignore_unknown);

  Env* run(Env* store, std::ostream& log) override;
  ~CompilePass() override;
};

}  // namespace MiniZinc
