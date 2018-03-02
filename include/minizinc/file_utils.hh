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
#include <vector>

namespace MiniZinc { namespace FileUtils {

  /// Return full path to current executable
  std::string progpath(void);
  /// Test if \a filename exists
  bool file_exists(const std::string& filename);
  /// Test if \a dirname exists and is a directory
  bool directory_exists(const std::string& dirname);
  /// Return full path to file
  std::string file_path(const std::string& filename);
  /// Return list of files with extension \a ext in directory \a dir
  std::vector<std::string> directory_list(const std::string& dir,
                                          const std::string& ext=std::string("*"));
  /// Inflate string \a s
  void inflateString(std::string& s);
  /// Deflate string \a s
  std::string deflateString(const std::string& s);
  /// Encode string into base 64
  std::string encodeBase64(const std::string& s);
  /// Decode string from base 64
  std::string decodeBase64(const std::string& s);
}}

#endif
