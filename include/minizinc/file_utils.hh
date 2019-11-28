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
  /// Find executable \a filename anywhere on the path
  /// On Windows, also check extensions .exe and .bat
  std::string find_executable(const std::string& filename);
  /// Return full path to file. If \a basePath is not empty, also try resolving
  /// relative paths with respect to \a basePath.
  std::string file_path(const std::string& filename, const std::string& basePath=std::string());
  /// Return directory name containing \a filename
  std::string dir_name(const std::string& filename);
  /// Return base name of \a filename (without dir_name)
  std::string base_name(const std::string& filename);
  /// Check whether path is absolute
  bool is_absolute(const std::string& path);
  /// Return list of files with extension \a ext in directory \a dir
  std::vector<std::string> directory_list(const std::string& dir,
                                          const std::string& ext=std::string("*"));
  /// Return share/minizinc directory if present anywhere above the executable
  std::string share_directory(void);
  /// Return current working directory
  std::string working_directory(void);
  /// Get global configuration file name (in share/minizinc directory)
  std::string global_config_file(void);
  /// Get per-user configuration file name (usually in home directory or AppData directory)
  std::string user_config_file(void);
  /// Get per-user configuration directory name (usually in home directory or AppData directory)
  std::string user_config_dir(void);
  /// Parse command line \a s into individual arguments
  std::vector<std::string> parseCmdLine(const std::string& s);
  /// Combine individual arguments \a cmd into properly quoted command line
  std::string combineCmdLine(const std::vector<std::string>& cmd);

  /// Create a temporary file
  class TmpFile {
  private:
    std::string _name;
#ifdef _WIN32
    std::vector<std::string> _tmpNames;
#endif
#ifndef _WIN32
    int _tmpfile_desc;
#endif
  public:
    // Constructor for file with extension \a ext
    TmpFile(const std::string& ext);
    /// Destructor (removes file)
    ~TmpFile(void);
    std::string name(void) const { return _name; }
  };

  /// Create a temporary directory
  class TmpDir {
  private:
    std::string _name;
  public:
    // Constructor for difrectory
    TmpDir(void);
    /// Destructor (removes directory)
    ~TmpDir(void);
    std::string name(void) const { return _name; }
  };

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
