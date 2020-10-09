/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <string>
#include <vector>

// Macro so that we can use overloaded wide versions of fstream::open for Windows
#ifdef _WIN32
#define FILE_PATH(path) MiniZinc::FileUtils::utf8_to_wide(path)
#else
#define FILE_PATH(path) (path)
#endif

namespace MiniZinc {
namespace FileUtils {

/// Return full path to current executable
std::string progpath();
/// Test if \a filename exists
bool file_exists(const std::string& filename);
/// Test if \a dirname exists and is a directory
bool directory_exists(const std::string& dirname);
/// Find executable \a filename anywhere on the path
/// On Windows, also check extensions .exe and .bat
std::string find_executable(const std::string& filename);
/// Return full path to file. If \a basePath is not empty, also try resolving
/// relative paths with respect to \a basePath.
std::string file_path(const std::string& filename, const std::string& basePath = std::string());
/// Return directory name containing \a filename
std::string dir_name(const std::string& filename);
/// Return base name of \a filename (without dir_name)
std::string base_name(const std::string& filename);
/// Check whether path is absolute
bool is_absolute(const std::string& path);
/// Return list of files with extension \a ext in directory \a dir
std::vector<std::string> directory_list(const std::string& dir,
                                        const std::string& ext = std::string("*"));
/// Return share/minizinc directory if present anywhere above the executable
std::string share_directory();
/// Return current working directory
std::string working_directory();
/// Get global configuration file name (in share/minizinc directory)
std::string global_config_file();
/// Get per-user configuration file name (usually in home directory or AppData directory)
std::string user_config_file();
/// Get per-user configuration directory name (usually in home directory or AppData directory)
std::string user_config_dir();
/// Parse command line \a s into individual arguments
std::vector<std::string> parse_cmd_line(const std::string& s);
/// Combine individual arguments \a cmd into properly quoted command line
std::string combine_cmd_line(const std::vector<std::string>& cmd);

/// Create a temporary file
class TmpFile {
private:
  std::string _name;
#ifdef _WIN32
  std::vector<std::string> _tmpNames;
#endif
#ifndef _WIN32
  int _tmpfileDesc;
#endif
public:
  // Constructor for file with extension \a ext
  TmpFile(const std::string& ext);
  /// Destructor (removes file)
  ~TmpFile();
  std::string name() const { return _name; }
};

/// Create a temporary directory
class TmpDir {
private:
  std::string _name;

public:
  // Constructor for difrectory
  TmpDir();
  /// Destructor (removes directory)
  ~TmpDir();
  std::string name() const { return _name; }
};

/// Inflate string \a s
void inflate_string(std::string& s);
/// Deflate string \a s
std::string deflate_string(const std::string& s);
/// Encode string into base 64
std::string encode_base64(const std::string& s);
/// Decode string from base 64
std::string decode_base64(const std::string& s);

#ifdef _WIN32
/// Convert UTF-16 string to UTF-8
std::string wide_to_utf8(const wchar_t* str, int size = -1);
/// Convert UTF-16 string to UTF-8
std::string wide_to_utf8(const std::wstring& str);
/// Convert UTF-8 string to UTF-16
std::wstring utf8_to_wide(const std::string& str);
#endif
}  // namespace FileUtils
}  // namespace MiniZinc
