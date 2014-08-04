/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_FILE_UTILS_HH__
#define __MINIZINC_FILE_UTILS_HH__

#include <string>

namespace MiniZinc { namespace FileUtils {

  /// Return full path to current executable
  std::string progpath(void);
  /// Test if \a filename exists
  bool file_exists(const std::string& filename);
  /// Test if \a dirname exists and is a directory
  bool directory_exists(const std::string& dirname);

}}

#endif
