/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace MiniZinc::FileUtils {

/// Convert the UTF-8 encoded \a path to a filesystem path (e.g. to open a file stream)
inline std::filesystem::path to_path(std::string_view path) {
  return std::filesystem::u8path(path);
}
/// Convert \a path to a UTF-8 encoded string
inline std::string to_utf8(const std::filesystem::path& path) { return path.u8string(); }

/// Return the directory of the current executable
std::string progpath();
/// Return the UTF-8 encoded value of environment variable \a env, if it is set
std::optional<std::string> get_env(const std::string& env);
/// Test if \a filename exists
bool file_exists(std::string_view filename);
/// Test if \a dirname exists and is a directory
bool directory_exists(std::string_view dirname);
/// Return the contents of a PATH-like environment variable.
std::vector<std::string> get_env_list(const std::string& env);
/// Find executable \a filename relative to basePath if set,
/// or if filename is a basename only, find anywhere on the PATH.
/// On Windows, also check extensions .exe, .bat and .cmd
/// If \a filename is a basename only, \a extraSearchDirs are searched before
/// the PATH.
std::string find_executable(const std::string& filename, const std::string& basePath = "",
                            const std::vector<std::string>& extraSearchDirs = {});
/// Find a dynamically loadable library named \a name. If \a name is a bare name
/// (no directory separator), the platform library-naming convention is applied
/// (e.g. `lib<name>.so` / `<name>.dll`) and it is searched for in \a basePath,
/// the dynamic-loader search path, and common system library directories.
/// Returns the absolute path if found, or the empty string otherwise.
std::string find_library(const std::string& name, const std::string& basePath = "");
/// Return full path to file. If \a basePath is not empty, resolve
/// relative paths with respect to \a basePath.
std::string file_path(std::string_view filename, std::string_view basePath = {});
/// Return \a name appended to \a dir, with a separator if needed
std::string join_path(std::string_view dir, std::string_view name);
/// Return directory name containing \a filename, or "." if it has no directory part
std::string dir_name(std::string_view filename);
/// Return base name of \a filename (without dir_name)
std::string base_name(std::string_view filename);
/// Check whether path is absolute
bool is_absolute(std::string_view path);
/// Return share/minizinc directory if present anywhere above the executable
std::string share_directory();
/// Return the contents of \a filename, decoding base64-encoded deflate data
/// when the file starts with "@". Throws errno if the file cannot be read.
std::string read_file_contents(const std::string& filename);
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

/// A new temporary directory, only accessible by the user
class TmpDir {
private:
  std::string _name;

public:
  /// Create the directory
  TmpDir();
  /// Remove the directory and its contents
  ~TmpDir();
  TmpDir(const TmpDir&) = delete;
  TmpDir& operator=(const TmpDir&) = delete;
  const std::string& name() const { return _name; }
};

/// A new empty temporary file, in its own temporary directory
class TmpFile {
private:
  TmpDir _dir;
  std::string _name;

public:
  /// Create the file, with extension \a ext
  explicit TmpFile(const std::string& ext);
  const std::string& name() const { return _name; }
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
std::string wide_to_utf8(const std::wstring& str);
/// Convert UTF-8 string to UTF-16
std::wstring utf8_to_wide(const std::string& str);
#endif
}  // namespace MiniZinc::FileUtils
