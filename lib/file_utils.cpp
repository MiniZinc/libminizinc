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

#include <minizinc/_thirdparty/b64/decode.h>
#include <minizinc/_thirdparty/b64/encode.h>
#include <minizinc/_thirdparty/miniz.h>
#include <minizinc/config.hh>
#include <minizinc/exception.hh>
#include <minizinc/file_utils.hh>

#include <cstring>
#include <sstream>
#include <string>

#ifdef HAS_PIDPATH
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libproc.h>
#include <unistd.h>
#elif defined(HAS_GETMODULEFILENAME) || defined(HAS_GETFILEATTRIBUTES)
#define NOMINMAX  // Ensure the words min/max remain available
#include <windows.h>
#undef ERROR
#else
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _MSC_VER
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")
#include "Shlobj.h"

#include <direct.h>
#else
#include <dirent.h>
#include <ftw.h>
#include <libgen.h>
#endif

namespace MiniZinc {
namespace FileUtils {

#ifdef HAS_PIDPATH
std::string progpath() {
  pid_t pid = getpid();
  char path[PROC_PIDPATHINFO_MAXSIZE];
  int ret = proc_pidpath(pid, path, sizeof(path));
  if (ret <= 0) {
    return "";
  }
  std::string p(path);
  size_t slash = p.find_last_of('/');
  if (slash != std::string::npos) {
    p = p.substr(0, slash);
  }
  return p;
}
#elif defined(HAS_GETMODULEFILENAME)
std::string progpath() {
  wchar_t path[MAX_PATH];
  int ret = GetModuleFileNameW(nullptr, path, MAX_PATH);
  if (ret <= 0) {
    return "";
  }
  std::string p = wide_to_utf8(path);
  size_t slash = p.find_last_of("/\\");
  if (slash != std::string::npos) {
    p = p.substr(0, slash);
  }
  return p;
}
#else
std::string progpath() {
  const int bufsz = 2000;
  char path[bufsz + 1];
  ssize_t sz = readlink("/proc/self/exe", path, bufsz);
  if (sz < 0) {
    return "";
  }
  path[sz] = '\0';
  std::string p(path);
  size_t slash = p.find_last_of('/');
  if (slash != std::string::npos) {
    p = p.substr(0, slash);
  }
  return p;
}
#endif

bool file_exists(const std::string& filename) {
#if defined(HAS_GETFILEATTRIBUTES)
  DWORD dwAttrib = GetFileAttributesW(utf8_to_wide(filename).c_str());

  return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
  struct stat info;
  return stat(filename.c_str(), &info) == 0 && ((info.st_mode & S_IFREG) != 0);
#endif
}

bool directory_exists(const std::string& dirname) {
#if defined(HAS_GETFILEATTRIBUTES)
  DWORD dwAttrib = GetFileAttributesW(utf8_to_wide(dirname).c_str());

  return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
  struct stat info;
  return stat(dirname.c_str(), &info) == 0 && ((info.st_mode & S_IFDIR) != 0);
#endif
}

std::string file_path(const std::string& filename, const std::string& basePath) {
#ifdef _MSC_VER
  LPWSTR lpFilePart;
  DWORD nBufferLength = GetFullPathNameW(utf8_to_wide(filename).c_str(), 0, nullptr, &lpFilePart);
  auto lpBuffer = static_cast<LPWSTR>(LocalAlloc(LMEM_FIXED, sizeof(WCHAR) * nBufferLength));
  if (lpBuffer == nullptr) {
    return "";
  }

  std::string ret;
  DWORD error =
      GetFullPathNameW(utf8_to_wide(filename).c_str(), nBufferLength, lpBuffer, &lpFilePart);
  DWORD fileAttr = GetFileAttributesW(lpBuffer);
  DWORD lastError = GetLastError();

  if (error == 0 || (fileAttr == INVALID_FILE_ATTRIBUTES && lastError != NO_ERROR)) {
    ret = basePath.empty() ? filename : file_path(basePath + "/" + filename);
  } else {
    ret = wide_to_utf8(lpBuffer);
  }
  LocalFree(lpBuffer);
  return ret;
#else
  char* rp = realpath(filename.c_str(), nullptr);
  if (rp == nullptr) {
    if (basePath.empty()) {
      return filename;
    }
    return file_path(basePath + "/" + filename);
  }
  std::string rp_s(rp);
  free(rp);
  return rp_s;
#endif
}

std::string dir_name(const std::string& filename) {
#ifdef _MSC_VER
  size_t pos = filename.find_last_of("\\/");
  return (pos == std::string::npos) ? "" : filename.substr(0, pos);
#else
  char* fn = strdup(filename.c_str());
  char* dn = dirname(fn);
  std::string ret(dn);
  free(fn);
  return ret;
#endif
}

std::string base_name(const std::string& filename) {
#ifdef _MSC_VER
  size_t pos = filename.find_last_of("\\/");
  return (pos == std::string::npos) ? filename : filename.substr(pos + 1);
#else
  char* fn = strdup(filename.c_str());
  char* dn = basename(fn);
  std::string ret(dn);
  free(fn);
  return ret;
#endif
}

bool is_absolute(const std::string& path) {
#ifdef _MSC_VER
  if (path.size() > 2 &&
      ((path[0] == '\\' && path[1] == '\\') || (path[0] == '/' && path[1] == '/'))) {
    return true;
  }
  return PathIsRelativeW(utf8_to_wide(path).c_str()) == FALSE;
#else
  return path.empty() ? false : (path[0] == '/');
#endif
}

std::string find_executable(const std::string& filename) {
  if (is_absolute(filename)) {
    if (file_exists(filename)) {
      return filename;
    }
#ifdef _MSC_VER
    if (FileUtils::file_exists(filename + ".exe")) {
      return filename + ".exe";
    }
    if (FileUtils::file_exists(filename + ".bat")) {
      return filename + ".bat";
    }
#endif
    return "";
  }
  char* path_c = getenv("PATH");
#ifdef _MSC_VER
  char pathsep = ';';
#else
  char pathsep = ':';
#endif
  std::string path;
  if (path_c != nullptr) {
    path = path_c;
    if (!path.empty()) {
      path += pathsep;
    }
  }
  path += progpath();
  std::string pathItem;
  std::stringstream pathStream(path);
  while (std::getline(pathStream, pathItem, pathsep)) {
    std::string fileWithPath = pathItem.append("/").append(filename);
    if (file_exists(fileWithPath)) {
      return fileWithPath;
    }
#ifdef _MSC_VER
    if (FileUtils::file_exists(fileWithPath + ".exe")) {
      return fileWithPath + ".exe";
    }
    if (FileUtils::file_exists(fileWithPath + ".bat")) {
      return fileWithPath + ".bat";
    }
#endif
  }
  return "";
}

std::vector<std::string> directory_list(const std::string& dir, const std::string& ext) {
  std::vector<std::string> entries;
#ifdef _MSC_VER
  WIN32_FIND_DATAW findData;
  HANDLE hFind = ::FindFirstFileW(utf8_to_wide(dir + "/*." + ext).c_str(), &findData);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        entries.push_back(wide_to_utf8(findData.cFileName));
      }
    } while (::FindNextFileW(hFind, &findData) == TRUE);
    ::FindClose(hFind);
  }
#else
  DIR* dirp = opendir(dir.c_str());
  if (dirp != nullptr) {
    struct dirent* dp;
    while ((dp = readdir(dirp)) != nullptr) {
      std::string fileName(dp->d_name);
      struct stat info;
      if (stat(((dir + "/").append(fileName)).c_str(), &info) == 0 &&
          ((info.st_mode & S_IFREG) != 0)) {
        if (ext == "*") {
          entries.push_back(fileName);
        } else {
          if (fileName.size() > ext.size() + 2 &&
              fileName.substr(fileName.size() - ext.size() - 1) == "." + ext) {
            entries.push_back(fileName);
          }
        }
      }
    }
    closedir(dirp);
  }
#endif
  return entries;
}

std::string working_directory() {
#ifdef _MSC_VER
  wchar_t wd[FILENAME_MAX];
  if (_wgetcwd(wd, FILENAME_MAX) == FALSE) {
    return "";
  }
  return wide_to_utf8(wd);
#else
  char wd[FILENAME_MAX];
  if (getcwd(wd, sizeof(wd)) == nullptr) {
    return "";
  }
  return wd;
#endif
}

std::string share_directory() {
#ifdef _WIN32
  if (wchar_t* MZNSTDLIBDIR = _wgetenv(L"MZN_STDLIB_DIR")) {
    return wide_to_utf8(MZNSTDLIBDIR);
  }
#else
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    return std::string(MZNSTDLIBDIR);
  }
#endif
  // NOLINTNEXTLINE(readability-redundant-string-init)
  std::string static_stdlib_dir(MZN_STATIC_STDLIB_DIR);
  if (FileUtils::file_exists(static_stdlib_dir + "/std/stdlib.mzn")) {
    return static_stdlib_dir;
  }
  std::string mypath = FileUtils::progpath();
  int depth = 0;
  for (char i : mypath) {
    if (i == '/' || i == '\\') {
      depth++;
    }
  }
  for (int i = 0; i <= depth; i++) {
    if (FileUtils::file_exists(mypath + "/share/minizinc/std/stdlib.mzn")) {
      return mypath + "/share/minizinc";
    }
    mypath += "/..";
  }
  return "";
}

std::string user_config_dir() {
#ifdef _MSC_VER
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
  if (const char* hd = getenv("HOME")) {
    return std::string(hd) + "/.minizinc";
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

TmpFile::TmpFile(const std::string& ext) {
#ifdef _WIN32
  WCHAR szTempFileName[MAX_PATH];
  WCHAR lpTempPathBuffer[MAX_PATH];

  bool didCopy;
  do {
    GetTempPathW(MAX_PATH, lpTempPathBuffer);
    GetTempFileNameW(lpTempPathBuffer, L"tmp_mzn_", 0, szTempFileName);

    _name = wide_to_utf8(szTempFileName);
    _tmpNames.push_back(_name);
    didCopy = CopyFileW(szTempFileName, utf8_to_wide(_name + ext).c_str(), TRUE) == TRUE;
  } while (!didCopy);
  _name += ext;
#else
  _tmpfileDesc = -1;
  _name = "/tmp/mznfileXXXXXX" + ext;
  char* tmpfile = strndup(_name.c_str(), _name.size());
  _tmpfileDesc = mkstemps(tmpfile, ext.size());
  if (_tmpfileDesc == -1) {
    ::free(tmpfile);
    throw InternalError("Error occurred when creating temporary file");
  }
  _name = std::string(tmpfile);
  ::free(tmpfile);
#endif
}

TmpFile::~TmpFile() {
#ifdef _WIN32
  _wremove(utf8_to_wide(_name).c_str());  // TODO: Is this necessary?
  for (auto& n : _tmpNames) {
    _wremove(utf8_to_wide(n).c_str());
  }
#else
  remove(_name.c_str());
  if (_tmpfileDesc != -1) {
    close(_tmpfileDesc);
  }
#endif
}

TmpDir::TmpDir() {
#ifdef _WIN32
  WCHAR szTempFileName[MAX_PATH];
  WCHAR lpTempPathBuffer[MAX_PATH];

  GetTempPathW(MAX_PATH, lpTempPathBuffer);
  GetTempFileNameW(lpTempPathBuffer, L"tmp_mzn_", 0, szTempFileName);

  _name = wide_to_utf8(szTempFileName);
  DeleteFileW(szTempFileName);
  CreateDirectoryW(szTempFileName, nullptr);
#else
  _name = "/tmp/mzndirXXXXXX";
  char* tmpfile = strndup(_name.c_str(), _name.size());

  if (mkdtemp(tmpfile) == nullptr) {
    ::free(tmpfile);
    throw InternalError("Error occurred when creating temporary directory");
  }
  _name = std::string(tmpfile);
  ::free(tmpfile);
#endif
}

#ifdef _WIN32
namespace {
void remove_dir(const std::string& d) {
  HANDLE dh;
  WIN32_FIND_DATAW info;
  auto dw = utf8_to_wide(d);
  auto pattern = dw + L"\\*.*";
  dh = ::FindFirstFileW(pattern.c_str(), &info);
  if (dh != INVALID_HANDLE_VALUE) {
    do {
      if (info.cFileName[0] != L'.') {
        auto fp = dw + L"\\" + info.cFileName;
        if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
          remove_dir(wide_to_utf8(fp));
        } else {
          ::SetFileAttributesW(fp.c_str(), FILE_ATTRIBUTE_NORMAL);
          ::DeleteFileW(fp.c_str());
        }
      }
    } while (::FindNextFileW(dh, &info) == TRUE);
  }
  ::FindClose(dh);
  ::SetFileAttributesW(dw.c_str(), FILE_ATTRIBUTE_NORMAL);
  ::RemoveDirectoryW(dw.c_str());
}
}  // namespace
#else
namespace {
int remove_file(const char* fpath, const struct stat* /*s*/, int /*i*/, struct FTW* /*ftw*/) {
  return unlink(fpath);
}
}  // namespace
#endif

TmpDir::~TmpDir() {
#ifdef _WIN32
  remove_dir(_name);
#else
  nftw(_name.c_str(), remove_file, 64, FTW_DEPTH | FTW_PHYS);
  rmdir(_name.c_str());
#endif
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
  auto* cc = reinterpret_cast<unsigned char*>(&s[0]);
  // autodetect compressed string
  if (s.size() >= 2 && ((cc[0] == 0x1F && cc[1] == 0x8B)     // gzip
                        || (cc[0] == 0x78 && (cc[1] == 0x01  // zlib
                                              || cc[1] == 0x9C || cc[1] == 0xDA)))) {
    const int BUF_SIZE = 1024;
    unsigned char s_outbuf[BUF_SIZE];
    z_stream stream;
    std::memset(&stream, 0, sizeof(stream));

    unsigned char* dataStart;
    int windowBits;
    size_t dataLen;
    if (cc[0] == 0x1F && cc[1] == 0x8B) {
      dataStart = cc + 10;
      windowBits = -Z_DEFAULT_WINDOW_BITS;
      if ((cc[3] & 0x4) != 0) {
        dataStart += 2;
        if (dataStart >= cc + s.size()) {
          throw(-1);
        }
      }
      if ((cc[3] & 0x8) != 0) {
        while (*dataStart != '\0') {
          dataStart++;
          if (dataStart >= cc + s.size()) {
            throw(-1);
          }
        }
        dataStart++;
        if (dataStart >= cc + s.size()) {
          throw(-1);
        }
      }
      if ((cc[3] & 0x10) != 0) {
        while (*dataStart != '\0') {
          dataStart++;
          if (dataStart >= cc + s.size()) {
            throw(-1);
          }
        }
        dataStart++;
        if (dataStart >= cc + s.size()) {
          throw(-1);
        }
      }
      if ((cc[3] & 0x2) != 0) {
        dataStart += 2;
        if (dataStart >= cc + s.size()) {
          throw(-1);
        }
      }
      dataLen = s.size() - (dataStart - cc);
    } else {
      dataStart = cc;
      windowBits = Z_DEFAULT_WINDOW_BITS;
      dataLen = s.size();
    }

    stream.next_in = dataStart;
    stream.avail_in = static_cast<unsigned int>(dataLen);
    stream.next_out = &s_outbuf[0];
    stream.avail_out = BUF_SIZE;
    int status = inflateInit2(&stream, windowBits);
    if (status != Z_OK) {
      throw(status);
    }
    std::ostringstream oss;
    while (true) {
      status = inflate(&stream, Z_NO_FLUSH);
      if (status == Z_STREAM_END || (stream.avail_out == 0U)) {
        // output buffer full or compression finished
        oss << std::string(reinterpret_cast<char*>(s_outbuf), BUF_SIZE - stream.avail_out);
        stream.next_out = &s_outbuf[0];
        stream.avail_out = BUF_SIZE;
      }
      if (status == Z_STREAM_END) {
        break;
      }
      if (status != Z_OK) {
        throw(status);
      }
    }
    status = inflateEnd(&stream);
    if (status != Z_OK) {
      throw(status);
    }
    s = oss.str();
  }
}

std::string deflate_string(const std::string& s) {
  mz_ulong compressedLength = compressBound(static_cast<mz_ulong>(s.size()));
  auto* cmpr = static_cast<unsigned char*>(::malloc(compressedLength * sizeof(unsigned char)));
  int status = compress(cmpr, &compressedLength, reinterpret_cast<const unsigned char*>(&s[0]),
                        static_cast<mz_ulong>(s.size()));
  if (status != Z_OK) {
    ::free(cmpr);
    throw(status);
  }
  std::string ret(reinterpret_cast<const char*>(cmpr), compressedLength);
  ::free(cmpr);
  return ret;
}

std::string encode_base64(const std::string& s) {
  base64::encoder E;
  std::ostringstream oss;
  oss << "@";  // add leading "@" to distinguish from valid MiniZinc code
  std::istringstream iss(s);
  E.encode(iss, oss);
  return oss.str();
}

std::string decode_base64(const std::string& s) {
  if (s.empty() || s[0] != '@') {
    throw InternalError("string is not base64 encoded");
  }
  base64::decoder D;
  std::ostringstream oss;
  std::istringstream iss(s);
  (void)iss.get();  // remove leading "@"
  D.decode(iss, oss);
  return oss.str();
}

#ifdef _WIN32
std::string wide_to_utf8(const wchar_t* str, int size) {
  int buffer_size = WideCharToMultiByte(CP_UTF8, 0, str, size, nullptr, 0, nullptr, nullptr);
  if (buffer_size == 0) {
    return "";
  }
  std::string result(buffer_size - 1, '\0');
  WideCharToMultiByte(CP_UTF8, 0, str, size, &result[0], buffer_size, nullptr, nullptr);
  return result;
}

std::string wide_to_utf8(const std::wstring& str) { return wide_to_utf8(str.c_str(), -1); }

std::wstring utf8_to_wide(const std::string& str) {
  int buffer_size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
  if (buffer_size == 0) {
    return L"";
  }
  std::wstring result(buffer_size - 1, '\0');
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], buffer_size);
  return result;
}
#endif
}  // namespace FileUtils
}  // namespace MiniZinc
