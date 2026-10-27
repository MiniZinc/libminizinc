/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/config.hh>
#include <minizinc/exception.hh>
#include <minizinc/file_utils.hh>

#include <algorithm>
#include <cerrno>
#include <cstdint>  // Required on some platforms for miniz
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <optional>
#include <random>
#include <sstream>
#include <string>

#include <minizinc/_thirdparty/miniz.h>

#ifdef __APPLE__
#include <libproc.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#undef ERROR
#include <shlobj.h>
#else
#include <unistd.h>
#endif

namespace MiniZinc::FileUtils {

namespace fs = std::filesystem;

#ifdef __APPLE__
std::string progpath() {
  char path[PROC_PIDPATHINFO_MAXSIZE];
  if (proc_pidpath(getpid(), path, sizeof(path)) <= 0) {
    return "";
  }
  return to_utf8(to_path(path).parent_path());
}
#elif defined(_WIN32)
std::string progpath() {
  wchar_t path[MAX_PATH];
  if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0) {
    return "";
  }
  return to_utf8(fs::path(path).parent_path());
}
#else
std::string progpath() {
  std::error_code ec;
  auto exe = fs::read_symlink("/proc/self/exe", ec);
  return ec ? "" : to_utf8(exe.parent_path());
}
#endif

bool file_exists(std::string_view filename) {
  std::error_code ec;
  return fs::is_regular_file(to_path(filename), ec);
}

bool directory_exists(std::string_view dirname) {
  std::error_code ec;
  return fs::is_directory(to_path(dirname), ec);
}

std::string file_path(std::string_view filename, std::string_view basePath) {
  // Add base path to relative paths if there is one
  auto f = to_path(basePath) / to_path(filename);
  std::error_code ec;
#ifdef _WIN32
  // Like GetFullPathName: make absolute, but leave links in place
  auto full = fs::absolute(f, ec);
#else
  // Like realpath: resolve links, and fail if the file does not exist
  auto full = fs::canonical(f, ec);
#endif
  return to_utf8(ec ? f : full);
}

std::string join_path(std::string_view dir, std::string_view name) {
  return to_utf8(to_path(dir) / to_path(name));
}

namespace {
/// Convert \a filename to a path, ignoring a trailing separator like dirname(3) and basename(3)
fs::path without_trailing_separator(std::string_view filename) {
  auto p = to_path(filename);
  return p.has_filename() ? p : p.parent_path();
}
}  // namespace

std::string dir_name(std::string_view filename) {
  auto dir = without_trailing_separator(filename).parent_path();
  return dir.empty() ? "." : to_utf8(dir);
}

std::string base_name(std::string_view filename) {
  return to_utf8(without_trailing_separator(filename).filename());
}

bool is_absolute(std::string_view path) {
  // On Windows, a path such as \foo is rooted on the current drive, so it is
  // treated as absolute even though it has no drive letter
  return to_path(path).has_root_directory();
}

std::optional<std::string> get_env(const std::string& env) {
#ifdef _WIN32
  if (const wchar_t* value = _wgetenv(utf8_to_wide(env).c_str())) {
    return wide_to_utf8(value);
  }
#else
  if (const char* value = getenv(env.c_str())) {
    return value;
  }
#endif
  return std::nullopt;
}

std::vector<std::string> get_env_list(const std::string& env) {
#ifdef _WIN32
  const char pathsep = ';';
#else
  const char pathsep = ':';
#endif
  std::vector<std::string> ret;
  std::string pathItem;
  std::stringstream pathStream(get_env(env).value_or(""));
  while (std::getline(pathStream, pathItem, pathsep)) {
    if (!pathItem.empty()) {
      ret.push_back(pathItem);
    }
  }
  return ret;
}

std::string find_executable(const std::string& filename, const std::string& basePath,
                            const std::vector<std::string>& extraSearchDirs) {
#ifdef _WIN32
  // On Windows, executables must end with one of these extensions
  std::vector<std::string> exeSuffixes = get_env_list("PATHEXT");
  if (std::find_if(exeSuffixes.begin(), exeSuffixes.end(), [&](const auto& suffix) {
        if (filename.size() < suffix.size()) {
          return false;
        }
        auto fileSuffix = filename.substr(filename.size() - suffix.size(), suffix.size());
        return _stricmp(suffix.c_str(), fileSuffix.c_str()) == 0;
      }) != exeSuffixes.end()) {
    // There's already an executable extension so try it first
    exeSuffixes.insert(exeSuffixes.begin(), "");
  }
#else
  std::vector<std::string> exeSuffixes = {""};
#endif
  if (is_absolute(filename)) {
    // If the path is absolute, use it
    for (const auto& suffix : exeSuffixes) {
      if (file_exists(filename + suffix)) {
        return file_path(filename + suffix);
      }
    }
    return "";
  }

  std::vector<std::string> searchDirs = {};
  if (base_name(filename) == filename) {
    // Filename only, so search the extra directories first, then the PATH
    searchDirs.insert(searchDirs.end(), extraSearchDirs.begin(), extraSearchDirs.end());
    auto path_dirs = get_env_list("PATH");
    searchDirs.insert(searchDirs.end(), path_dirs.begin(), path_dirs.end());
  } else {
    // Relative path, so search relative to base path
    searchDirs.push_back(basePath);
  }

  for (const auto& path : searchDirs) {
    auto fileWithPath = file_path(join_path(path, filename));
    for (const auto& suffix : exeSuffixes) {
      if (file_exists(fileWithPath + suffix)) {
        return file_path(fileWithPath + suffix);
      }
    }
  }
  return "";
}

std::string find_library(const std::string& name, const std::string& basePath) {
  const bool bare = (base_name(name) == name) && !is_absolute(name);

  // Candidate leaf filenames to try, in priority order, following the platform
  // library-naming convention. `name` itself is always tried (it may already
  // carry an extension or `lib` prefix).
  std::vector<std::string> leaves;
#ifdef _WIN32
  if (bare) {
    leaves = {name + ".dll", "lib" + name + ".dll", name};
  } else {
    leaves = {name, name + ".dll"};
  }
#elif defined(__APPLE__)
  if (bare) {
    leaves = {"lib" + name + ".dylib", name + ".dylib", "lib" + name + ".so", name + ".so", name};
  } else {
    leaves = {name, name + ".dylib", name + ".so"};
  }
#else
  if (bare) {
    leaves = {"lib" + name + ".so", name + ".so", name};
  } else {
    leaves = {name, name + ".so"};
  }
#endif

  // Directories to search, approximating the dynamic loader's search order.
  std::vector<std::string> dirs;
  if (bare) {
    if (!basePath.empty()) {
      dirs.push_back(basePath);
    }
#ifdef _WIN32
    auto env_dirs = get_env_list("PATH");
#elif defined(__APPLE__)
    auto env_dirs = get_env_list("DYLD_LIBRARY_PATH");
#else
    auto env_dirs = get_env_list("LD_LIBRARY_PATH");
#endif
    dirs.insert(dirs.end(), env_dirs.begin(), env_dirs.end());
#ifndef _WIN32
    dirs.emplace_back("/usr/local/lib");
    dirs.emplace_back("/usr/lib");
    dirs.emplace_back("/lib");
#endif
  } else if (is_absolute(name)) {
    dirs.emplace_back("");  // the leaves are (or resolve to) absolute paths
  } else {
    dirs.push_back(basePath);  // relative path: resolve against the base path
  }

  for (const auto& dir : dirs) {
    for (const auto& leaf : leaves) {
      const std::string full = is_absolute(leaf) ? leaf : file_path(join_path(dir, leaf));
      if (file_exists(full)) {
        return file_path(full);
      }
    }
  }
  return "";
}

std::string share_directory() {
  if (auto stdlibDir = get_env("MZN_STDLIB_DIR")) {
    return *stdlibDir;
  }
  // NOLINTNEXTLINE(readability-redundant-string-init)
  std::string static_stdlib_dir(MZN_STATIC_STDLIB_DIR);
  if (FileUtils::file_exists(static_stdlib_dir + "/std/stdlib.mzn")) {
    return static_stdlib_dir;
  }
  auto mypath = to_path(FileUtils::progpath());
  if (mypath.empty()) {
    return "";
  }
  // Search the directory of the executable and all directories above it
  for (;; mypath = mypath.parent_path()) {
    auto share = mypath / "share" / "minizinc";
    std::error_code ec;
    if (fs::is_regular_file(share / "std" / "stdlib.mzn", ec)) {
      return to_utf8(share);
    }
    if (mypath == mypath.parent_path()) {
      return "";
    }
  }
}

// fastest way to read a file into a string (especially big files)
// see: http://insanecoding.blogspot.be/2011/11/how-to-read-in-file-in-c.html
std::string read_file_contents(const std::string& filename) {
  std::ifstream in(to_path(filename), std::ios::binary);
  if (in) {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(static_cast<unsigned int>(in.tellg()));
    in.seekg(0, std::ios::beg);
    in.read(contents.data(), static_cast<long>(contents.size()));
    in.close();
    if (!contents.empty() && contents[0] == '@') {
      contents = decode_base64(contents);
      inflate_string(contents);
    }
    return contents;
  }
  throw errno;  // NOLINT(misc-throw-by-value-catch-by-reference)
}

std::string user_config_dir() {
#ifdef _WIN32
  HRESULT hr;
  PWSTR pszPath = nullptr;

  hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pszPath);
  if (SUCCEEDED(hr)) {
    auto configPath = wide_to_utf8(pszPath);
    CoTaskMemFree(pszPath);
    if (configPath.empty()) {
      return "";
    }
    return configPath + "/MiniZinc";
  }
  return "";
#else
  if (auto home = get_env("HOME"); home && !home->empty()) {
    return join_path(*home, ".minizinc");
  }
  return "";
#endif
}

std::string global_config_file() {
  std::string sd = share_directory();
  if (sd.empty()) {
    return "";
  }
  return sd + "/Preferences.json";
}

std::string user_config_file() { return user_config_dir() + "/Preferences.json"; }

namespace {
/// The directory for temporary files: $TMPDIR (or similar) if it is valid, otherwise /tmp
fs::path temp_dir() {
  std::error_code ec;
  auto dir = fs::temp_directory_path(ec);
  return ec ? fs::path("/tmp") : dir;
}
}  // namespace

TmpDir::TmpDir() {
#ifdef _WIN32
  // The temporary directory belongs to the user on Windows, so a new name is enough
  auto base = temp_dir();
  // (std::random_device is deterministic in older MinGW versions)
  std::seed_seq seed{std::random_device{}(), static_cast<unsigned int>(GetCurrentProcessId()),
                     static_cast<unsigned int>(GetTickCount())};
  std::mt19937_64 rng(seed);
  for (int attempt = 0; attempt < 100; ++attempt) {
    std::ostringstream name;
    name << "mzn" << std::hex << rng();
    auto dir = base / name.str();
    std::error_code ec;
    // Creating the directory is atomic: it returns false without an error if the name is taken
    if (fs::create_directory(dir, ec)) {
      _name = to_utf8(dir);
      return;
    }
    if (ec) {
      break;
    }
  }
#else
  // Shared temporary directories need mkdtemp, which creates the directory accessible only by the
  // user in one step
  auto name = to_utf8(temp_dir() / "mznXXXXXX");
  if (mkdtemp(name.data()) != nullptr) {
    _name = name;
    return;
  }
#endif
  throw Error("Error occurred when creating temporary directory");
}

TmpDir::~TmpDir() {
  std::error_code ec;
  fs::remove_all(to_path(_name), ec);
}

TmpFile::TmpFile(const std::string& ext) : _name(join_path(_dir.name(), "mzn" + ext)) {
  if (!std::ofstream(to_path(_name))) {
    throw Error("Error occurred when creating temporary file");
  }
}

std::vector<std::string> parse_cmd_line(const std::string& s) {
  // Break the string up at whitespace, except inside quotes, but ignore escaped quotes
  std::vector<std::string> c;
  size_t cur = 0;
  size_t l = s.length();
  std::ostringstream oss;
  bool inside_quote = false;
  bool had_escape = false;
  for (; cur < l; cur++) {
    if (inside_quote) {
      if (s[cur] == '"') {
        if (had_escape) {
          oss << "\"";
          had_escape = false;
        } else {
          inside_quote = false;
        }
      } else if (s[cur] == '\\') {
        had_escape = true;
      } else {
        if (had_escape) {
          oss << "\\";
          had_escape = false;
        }
        oss << s[cur];
      }
    } else {
      if (s[cur] == ' ') {
        if (had_escape) {
          oss << " ";
          had_escape = false;
        } else {
          c.push_back(oss.str());
          oss.str(std::string());
        }
      } else if (s[cur] == '\\') {
        if (had_escape) {
          oss << "\\";
          had_escape = false;
        } else {
          had_escape = true;
        }
      } else if (s[cur] == '"') {
        if (had_escape) {
          oss << "\"";
          had_escape = false;
        } else {
          inside_quote = true;
        }
      } else {
        if (had_escape) {
          switch (s[cur]) {
            case 'a':
              oss << "\a";
              break;
            case 'b':
              oss << "\b";
              break;
            case 'f':
              oss << "\f";
              break;
            case 'n':
              oss << "\n";
              break;
            case 'r':
              oss << "\r";
              break;
            case 't':
              oss << "\t";
              break;
            case 'v':
              oss << "\v";
              break;
            default:
              oss << "\\" << s[cur];
              break;
          }
          had_escape = false;
        } else {
          oss << s[cur];
        }
      }
    }
  }
  c.push_back(oss.str());
  return c;
}

std::string combine_cmd_line(const std::vector<std::string>& cmd) {
  std::ostringstream ret;
  for (unsigned int i = 0; i < cmd.size(); i++) {
    const auto& c = cmd[i];
    ret << "\"";
    for (char i : c) {
      switch (i) {
        case '\a':
          ret << "\\a";
          break;
        case '\b':
          ret << "\\b";
          break;
        case '\f':
          ret << "\\f";
          break;
        case '\n':
          ret << "\\n";
          break;
        case '\r':
          ret << "\\r";
          break;
        case '\t':
          ret << "\\t";
          break;
        case '\v':
          ret << "\\v";
          break;
        case '"':
          ret << "\\\"";
          break;
        case '\\':
          ret << "\\\\";
          break;
        default:
          ret << i;
          break;
      }
    }
    ret << "\"";
    if (i < cmd.size() - 1) {
      ret << " ";
    }
  }
  return ret.str();
}

void inflate_string(std::string& s) {
  const auto* cc = reinterpret_cast<const unsigned char*>(s.data());
  // autodetect compressed string
  if (s.size() >= 2 && ((cc[0] == 0x1F && cc[1] == 0x8B)     // gzip
                        || (cc[0] == 0x78 && (cc[1] == 0x01  // zlib
                                              || cc[1] == 0x9C || cc[1] == 0xDA)))) {
    const int BUF_SIZE = 1024;
    unsigned char s_outbuf[BUF_SIZE];
    z_stream stream;
    std::memset(&stream, 0, sizeof(stream));

    const unsigned char* dataStart = cc[0] == 0x1F && cc[1] == 0x8B ? (cc + 10) : cc;
    int windowBits;
    size_t dataLen;
    if (cc[0] == 0x1F && cc[1] == 0x8B) {
      windowBits = -Z_DEFAULT_WINDOW_BITS;
      if ((cc[3] & 0x4) != 0) {
        dataStart += 2;
        if (dataStart >= cc + s.size()) {
          throw -1;
        }
      }
      if ((cc[3] & 0x8) != 0) {
        while (*dataStart != '\0') {
          dataStart++;
          if (dataStart >= cc + s.size()) {
            throw -1;
          }
        }
        dataStart++;
        if (dataStart >= cc + s.size()) {
          throw -1;
        }
      }
      if ((cc[3] & 0x10) != 0) {
        while (*dataStart != '\0') {
          dataStart++;
          if (dataStart >= cc + s.size()) {
            throw -1;
          }
        }
        dataStart++;
        if (dataStart >= cc + s.size()) {
          throw -1;
        }
      }
      if ((cc[3] & 0x2) != 0) {
        dataStart += 2;
        if (dataStart >= cc + s.size()) {
          throw -1;
        }
      }
      dataLen = s.size() - (dataStart - cc);
    } else {
      windowBits = Z_DEFAULT_WINDOW_BITS;
      dataLen = s.size();
    }

    stream.next_in = dataStart;
    stream.avail_in = static_cast<unsigned int>(dataLen);
    stream.next_out = &s_outbuf[0];
    stream.avail_out = BUF_SIZE;
    int status = inflateInit2(&stream, windowBits);
    if (status != Z_OK) {
      throw status;
    }
    // Appended to directly rather than through a stringstream: for a whole
    // compressed library that would hold the result twice.
    std::string out;
    while (true) {
      status = inflate(&stream, Z_NO_FLUSH);
      if (status == Z_STREAM_END || (stream.avail_out == 0U)) {
        // output buffer full or compression finished
        out.append(reinterpret_cast<char*>(s_outbuf), BUF_SIZE - stream.avail_out);
        stream.next_out = &s_outbuf[0];
        stream.avail_out = BUF_SIZE;
      }
      if (status == Z_STREAM_END) {
        break;
      }
      if (status != Z_OK) {
        throw status;
      }
    }
    status = inflateEnd(&stream);
    if (status != Z_OK) {
      throw status;
    }
    s = std::move(out);
  }
}

std::string deflate_string(const std::string& s) {
  mz_ulong compressedLength = compressBound(static_cast<mz_ulong>(s.size()));
  std::string ret(compressedLength, '\0');
  int status =
      compress(reinterpret_cast<unsigned char*>(ret.data()), &compressedLength,
               reinterpret_cast<const unsigned char*>(s.data()), static_cast<mz_ulong>(s.size()));
  if (status != Z_OK) {
    throw status;
  }
  ret.resize(compressedLength);
  return ret;
}

namespace {
const char* const BASE64_ALPHABET =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}  // namespace

std::string encode_base64(const std::string& s) {
  // Exact encoded size, so every character below can be written by index
  std::string ret(1 + (s.size() + 2) / 3 * 4, '\0');
  ret[0] = '@';  // leading "@" distinguishes it from valid MiniZinc code
  size_t out = 1;
  for (size_t i = 0; i < s.size(); i += 3) {
    const size_t rest = s.size() - i;
    unsigned int block = static_cast<unsigned int>(static_cast<unsigned char>(s[i])) << 16;
    if (rest > 1) {
      block |= static_cast<unsigned int>(static_cast<unsigned char>(s[i + 1])) << 8;
    }
    if (rest > 2) {
      block |= static_cast<unsigned int>(static_cast<unsigned char>(s[i + 2]));
    }
    ret[out++] = BASE64_ALPHABET[(block >> 18) & 0x3f];
    ret[out++] = BASE64_ALPHABET[(block >> 12) & 0x3f];
    ret[out++] = rest > 1 ? BASE64_ALPHABET[(block >> 6) & 0x3f] : '=';
    ret[out++] = rest > 2 ? BASE64_ALPHABET[block & 0x3f] : '=';
  }
  return ret;
}

std::string decode_base64(const std::string& s) {
  if (s.empty() || s[0] != '@') {
    throw InternalError("string is not base64 encoded");
  }
  // Reverse of BASE64_ALPHABET; any value above 63 marks padding, line breaks and other filler,
  // which are skipped. Built per call: it costs a memset and 64 stores, which even for a few
  // hundred bytes of input is far cheaper than a static with the thread-safe-initialisation
  // guard the inner loop would then pay on every character.
  unsigned char value[256];
  std::memset(value, 0xff, sizeof(value));
  for (unsigned int i = 0; i < 64; i++) {
    value[static_cast<unsigned char>(BASE64_ALPHABET[i])] = static_cast<unsigned char>(i);
  }
  std::string ret(s.size() / 4 * 3 + 3, '\0');  // upper bound, trimmed once decoded
  size_t out = 0;
  unsigned int block = 0;
  int bits = 0;
  for (size_t i = 1; i < s.size(); i++) {
    const unsigned int v = value[static_cast<unsigned char>(s[i])];
    if (v > 63) {
      continue;
    }
    block = (block << 6) | v;
    bits += 6;
    if (bits >= 8) {
      bits -= 8;
      ret[out++] = static_cast<char>((block >> bits) & 0xff);
    }
  }
  ret.resize(out);
  return ret;
}

#ifdef _WIN32
std::string wide_to_utf8(const std::wstring& str) {
  int buffer_size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
  if (buffer_size == 0) {
    return "";
  }
  std::string result(buffer_size - 1, '\0');
  WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, result.data(), buffer_size, nullptr, nullptr);
  return result;
}

std::wstring utf8_to_wide(const std::string& str) {
  int buffer_size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
  if (buffer_size == 0) {
    return L"";
  }
  std::wstring result(buffer_size - 1, '\0');
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), buffer_size);
  return result;
}
#endif
}  // namespace MiniZinc::FileUtils
