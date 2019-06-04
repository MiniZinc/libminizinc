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

#include <minizinc/file_utils.hh>
#include <minizinc/exception.hh>
#include <minizinc/config.hh>
#include <minizinc/thirdparty/miniz.h>
#include <minizinc/thirdparty/b64/encode.h>
#include <minizinc/thirdparty/b64/decode.h>
#include <sstream>
#include <string>
#include <cstring>

#ifdef HAS_PIDPATH
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libproc.h>
#include <unistd.h>
#elif defined(HAS_GETMODULEFILENAME) || defined(HAS_GETFILEATTRIBUTES)
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")
#include "Shlobj.h"
#include <direct.h>
#else
#include <dirent.h>
#include <libgen.h>
#include <ftw.h>
#endif

namespace MiniZinc { namespace FileUtils {
  
#ifdef HAS_PIDPATH
  std::string progpath(void) {
    pid_t pid = getpid();
    char path[PROC_PIDPATHINFO_MAXSIZE];
    int ret = proc_pidpath (pid, path, sizeof(path));
    if ( ret <= 0 ) {
      return "";
    } else {
      std::string p(path);
      size_t slash = p.find_last_of("/");
      if (slash != std::string::npos) {
        p = p.substr(0,slash);
      }
      return p;
    }
  }
#elif defined(HAS_GETMODULEFILENAME)
  std::string progpath(void) {
    char path[MAX_PATH];
    int ret = GetModuleFileName(NULL, path, MAX_PATH);
    if ( ret <= 0 ) {
      return "";
    } else {
      std::string p(path);
      size_t slash = p.find_last_of("/\\");
      if (slash != std::string::npos) {
        p = p.substr(0,slash);
      }
      return p;
    }
  }
#else
  std::string progpath(void) {
    const int bufsz = 2000;
    char path[bufsz+1];
    ssize_t sz = readlink("/proc/self/exe", path, bufsz);
    if ( sz < 0 ) {
      return "";
    } else {
      path[sz] = '\0';
      std::string p(path);
      size_t slash = p.find_last_of("/");
      if (slash != std::string::npos) {
        p = p.substr(0,slash);
      }
      return p;
    }
  }
#endif
  
  bool file_exists(const std::string& filename) {
#if defined(HAS_GETFILEATTRIBUTES)
    DWORD dwAttrib = GetFileAttributes(filename.c_str());
    
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat info;
    return stat(filename.c_str(), &info)==0 && (info.st_mode & S_IFREG);
#endif
  }
  
  bool directory_exists(const std::string& dirname) {
#if defined(HAS_GETFILEATTRIBUTES)
    DWORD dwAttrib = GetFileAttributes(dirname.c_str());
      
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat info;
    return stat(dirname.c_str(), &info)==0 && (info.st_mode & S_IFDIR);
#endif
  }

  std::string file_path(const std::string& filename, const std::string& basePath) {
#ifdef _MSC_VER
    LPSTR lpBuffer, lpFilePart;
    DWORD nBufferLength = GetFullPathName(filename.c_str(), 0,0,&lpFilePart);
    if (!(lpBuffer = (LPTSTR)LocalAlloc(LMEM_FIXED, sizeof(TCHAR) * nBufferLength)))
      return 0;
    std::string ret;
    DWORD error = GetFullPathName(filename.c_str(), nBufferLength, lpBuffer, &lpFilePart);
    DWORD fileAttr = GetFileAttributes(lpBuffer);
    DWORD lastError = GetLastError();

    if(error == 0 || (fileAttr == INVALID_FILE_ATTRIBUTES && lastError != NO_ERROR)) {
      if (basePath.empty())
        ret = filename;
      else
        ret = file_path(basePath+"/"+filename);
    } else {
      ret = std::string(lpBuffer);
    }
    LocalFree(lpBuffer);
    return ret;
#else
    char* rp = realpath(filename.c_str(), NULL);
    if (rp==NULL) {
      if (basePath.empty())
        return filename;
      else
        return file_path(basePath+"/"+filename);
    }
    std::string rp_s(rp);
    free(rp);
    return rp_s;
#endif
  }
  
  std::string dir_name(const std::string& filename) {
#ifdef _MSC_VER
    size_t pos = filename.find_last_of("\\/");
    return (pos==std::string::npos) ? "" : filename.substr(0,pos);
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
    return (pos==std::string::npos) ? filename : filename.substr(pos+1);
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
    return !PathIsRelative(path.c_str());
#else
    return path.empty() ? false : (path[0]=='/');
#endif
  }
  
  std::string find_executable(const std::string& filename) {
    if (is_absolute(filename)) {
      if (file_exists(filename)) {
        return filename;
      }
#ifdef _MSC_VER
      if (FileUtils::file_exists(filename+".exe")) {
        return filename+".exe";
      } else if (FileUtils::file_exists(filename+".bat")) {
        return filename+".bat";
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
    if (path_c) {
      path = path_c;
      if (path.size()) {
        path += pathsep;
      }
    }
    path += progpath();
    std::string pathItem;
    std::stringstream pathStream(path);
    while (std::getline(pathStream, pathItem, pathsep)) {
      std::string fileWithPath = pathItem+"/"+filename;
      if (file_exists(fileWithPath))
        return fileWithPath;
#ifdef _MSC_VER
      if (FileUtils::file_exists(fileWithPath+".exe")) {
        return fileWithPath+".exe";
      } else if (FileUtils::file_exists(fileWithPath+".bat")) {
        return fileWithPath+".bat";
      }
#endif
    }
    return "";
  }
  

  std::vector<std::string> directory_list(const std::string& dir,
                                          const std::string& ext) {
    std::vector<std::string> entries;
#ifdef _MSC_VER
    WIN32_FIND_DATA findData;
    HANDLE hFind = ::FindFirstFile( (dir+"/*."+ext).c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        if ( !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
          entries.push_back(findData.cFileName);
        }
      } while(::FindNextFile(hFind, &findData));
      ::FindClose(hFind);
    }
#else
    DIR* dirp = opendir(dir.c_str());
    if (dirp) {
      struct dirent* dp;
      while ((dp = readdir(dirp)) != NULL) {
        std::string fileName(dp->d_name);
        struct stat info;
        if (stat( (dir+"/"+fileName).c_str(), &info)==0 && (info.st_mode & S_IFREG)) {
          if (ext=="*") {
            entries.push_back(fileName);
          } else {
            if (fileName.size() > ext.size()+2 &&
                fileName.substr(fileName.size()-ext.size()-1)=="."+ext) {
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

  std::string working_directory(void) {
    char wd[FILENAME_MAX];
#ifdef _MSC_VER
    if (!_getcwd(wd,sizeof(wd)))
      return "";
#else
    if (!getcwd(wd,sizeof(wd)))
      return "";
#endif
    return wd;
  }
  
  std::string share_directory(void) {
    if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
      return std::string(MZNSTDLIBDIR);
    }
    std::string mypath = FileUtils::progpath();
    int depth = 0;
    for (unsigned int i=0; i<mypath.size(); i++)
      if (mypath[i]=='/' || mypath[i]=='\\')
        depth++;
    for (int i=0; i<=depth; i++) {
      if (FileUtils::file_exists(mypath+"/share/minizinc/std/builtins.mzn"))
        return mypath+"/share/minizinc";
      mypath += "/..";
    }
    return "";
  }
  
  std::string user_config_dir(void) {
#ifdef _MSC_VER
    HRESULT hr;
    PWSTR pszPath = NULL;
    
    hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath);
    if (SUCCEEDED(hr)) {
      int charsRequired = WideCharToMultiByte(CP_ACP, 0, pszPath, -1, 0, 0, 0, 0);
      std::string configPath;
      if (charsRequired > 0) {
        char* tmp = new char[charsRequired];
        if (WideCharToMultiByte(CP_ACP, 0, pszPath, -1, tmp, charsRequired, 0, 0) != 0) {
          tmp[charsRequired-1]=0;
          configPath=tmp;
        }
        delete[] tmp;
      }
      CoTaskMemFree(pszPath);
      if (configPath.empty()) {
        return "";
      } else {
        return configPath+"/MiniZinc";
      }
    }
    return "";
#else
    if (const char* hd = getenv("HOME")) {
      return std::string(hd)+"/.minizinc";
    }
    return "";
#endif
  }

  std::string global_config_file(void) {
    std::string sd = share_directory();
    if (sd.empty())
      return "";
    return sd+"/Preferences.json";
  }
  
  std::string user_config_file(void) {
    return user_config_dir()+"/Preferences.json";
  }

  TmpFile::TmpFile(const std::string& ext) {
#ifdef _WIN32
    TCHAR szTempFileName[MAX_PATH];
    TCHAR lpTempPathBuffer[MAX_PATH];
    
    GetTempPath(MAX_PATH, lpTempPathBuffer);
    GetTempFileName(lpTempPathBuffer,
                    "tmp_mzn_", 0, szTempFileName);
    
    _name = szTempFileName;
    MoveFile(_name.c_str(), (_name + ext).c_str());
    _name += ext;
#else
    _tmpfile_desc = -1;
    _name = "/tmp/mznfileXXXXXX"+ext;
    char* tmpfile = strndup(_name.c_str(), _name.size());
    _tmpfile_desc = mkstemps(tmpfile, ext.size());
    if (_tmpfile_desc == -1) {
      ::free(tmpfile);
      throw InternalError("Error occurred when creating temporary file");
    }
    _name = std::string(tmpfile);
    ::free(tmpfile);
#endif
  }
  
  TmpFile::~TmpFile(void) {
    remove(_name.c_str());
#ifndef _WIN32
    if (_tmpfile_desc != -1)
      close(_tmpfile_desc);
#endif
  }

  TmpDir::TmpDir(void) {
#ifdef _WIN32
    do {
      _name = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());
    } while (file_exists(_name));
    Directory.CreateDirectory(_name);
#else
    _name = "/tmp/mzndirXXXXXX";
    char* tmpfile = strndup(_name.c_str(), _name.size());
    
    if (mkdtemp(tmpfile) == NULL) {
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
      WIN32_FIND_DATA info;
      
      std::string pattern = d+"\\*.*";
      dh = ::FindFirstFile(pattern.c_str(), &info);
      if (dh != INVALID_HANDLE_VALUE) {
        do {
          if (info.cFileName[0] != '.') {
            std::string fp = d+"\\"+info.cFileName;
            if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
              remove_dir(fp);
            } else {
              ::SetFileAttributes(fp.c_str(), FILE_ATTRIBUTE_NORMAL);
              ::DeleteFile(fp.c_str());
            }
          }
        } while (::FindNextFile(dh, &info) == TRUE);
      }
      ::FindClose(dh);
      ::SetFileAttributes(d.c_str(), FILE_ATTRIBUTE_NORMAL);
      ::RemoveDirectory(d.c_str());
    }
  }
#else
  namespace {
    int remove_file(const char *fpath, const struct stat *, int, struct FTW *) {
      return unlink(fpath);
    }
  }
#endif
  
  TmpDir::~TmpDir(void) {
#ifdef _WIN32
    remove_dir(_name);
#else
    nftw(_name.c_str(), remove_file, 64, FTW_DEPTH | FTW_PHYS);
    rmdir(_name.c_str());
#endif
  }

  std::vector<std::string> parseCmdLine(const std::string& s) {
    // Break the string up at whitespace, except inside quotes, but ignore escaped quotes
    std::vector<std::string> c;
    size_t cur = 0;
    size_t l = s.length();
    std::ostringstream oss;
    bool inside_quote = false;
    bool had_escape = false;
    for (; cur < l; cur++) {
      if (inside_quote) {
        if (s[cur]=='"') {
          if (had_escape) {
            oss << "\"";
            had_escape = false;
          } else {
            inside_quote = false;
          }
        } else if (s[cur]=='\\') {
          had_escape = true;
        } else {
          if (had_escape) {
            oss << "\\";
            had_escape = false;
          }
          oss << s[cur];
        }
      } else {
        if (s[cur]==' ') {
          if (had_escape) {
            oss << " ";
            had_escape = false;
          } else {
            c.push_back(oss.str());
            oss.str(std::string());
          }
        } else if (s[cur]=='\\') {
          if (had_escape) {
            oss << "\\";
            had_escape = false;
          } else {
            had_escape = true;
          }
        } else if (s[cur]=='"') {
          if (had_escape) {
            oss << "\"";
            had_escape = false;
          } else {
            inside_quote = true;
          }
        } else {
          if (had_escape) {
            switch (s[cur]) {
              case 'a' : oss << "\a"; break;
              case 'b' : oss << "\b"; break;
              case 'f' : oss << "\f"; break;
              case 'n' : oss << "\n"; break;
              case 'r' : oss << "\r"; break;
              case 't' : oss << "\t"; break;
              case 'v' : oss << "\v"; break;
              default: oss << "\\" << s[cur]; break;
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
  
  std::string combineCmdLine(const std::vector<std::string>& cmd) {
    std::ostringstream ret;
    for (unsigned int i=0; i<cmd.size(); i++) {
      auto& c = cmd[i];
      ret << "\"";
      for (size_t i=0; i<c.size(); i++) {
        switch (c[i]) {
          case '\a' : ret << "\\a"; break;
          case '\b' : ret << "\\b"; break;
          case '\f' : ret << "\\f"; break;
          case '\n' : ret << "\\n"; break;
          case '\r' : ret << "\\r"; break;
          case '\t' : ret << "\\t"; break;
          case '\v' : ret << "\\v"; break;
          case '"' :  ret << "\\\""; break;
          case '\\':  ret << "\\\\"; break;
          default:    ret << c[i]; break;
        }
      }
      ret << "\"";
      if (i<cmd.size()-1) {
        ret << " ";
      }
    }
    return ret.str();
  }
  
  void inflateString(std::string& s) {
    unsigned char* cc = reinterpret_cast<unsigned char*>(&s[0]);
    // autodetect compressed string
    if (s.size() >= 2 &&
        ((cc[0] == 0x1F && cc[1] == 0x8B)         // gzip
         || (cc[0] == 0x78 && (cc[1] == 0x01      // zlib
                               || cc[1] == 0x9C
                               || cc[1] == 0xDA)))) {
      const int BUF_SIZE=1024;
      unsigned char s_outbuf[BUF_SIZE];
      z_stream stream;
      std::memset(&stream, 0, sizeof(stream));
      
      unsigned char* dataStart;
      int windowBits;
      size_t dataLen;
      if (cc[0]==0x1F && cc[1]==0x8B) {
        dataStart = cc+10;
        windowBits = -Z_DEFAULT_WINDOW_BITS;
        if (cc[3] & 0x4) {
          dataStart += 2;
          if (dataStart >= cc+s.size())
            throw(-1);
        }
        if (cc[3] & 0x8) {
          while (*dataStart != '\0') {
            dataStart++;
            if (dataStart >= cc+s.size())
              throw(-1);
          }
          dataStart++;
          if (dataStart >= cc+s.size())
            throw(-1);
        }
        if (cc[3] & 0x10) {
          while (*dataStart != '\0') {
            dataStart++;
            if (dataStart >= cc+s.size())
              throw(-1);
          }
          dataStart++;
          if (dataStart >= cc+s.size())
            throw(-1);
        }
        if (cc[3] & 0x2) {
          dataStart += 2;
          if (dataStart >= cc+s.size())
            throw(-1);
        }
        dataLen = s.size() - (dataStart-cc);
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
      if (status != Z_OK)
        throw(status);
      std::ostringstream oss;
      while (true) {
        status = inflate(&stream, Z_NO_FLUSH);
        if (status == Z_STREAM_END || !stream.avail_out) {
          // output buffer full or compression finished
          oss << std::string(reinterpret_cast<char*>(s_outbuf),BUF_SIZE - stream.avail_out);
          stream.next_out = &s_outbuf[0];
          stream.avail_out = BUF_SIZE;
        }
        if (status == Z_STREAM_END)
          break;
        if (status != Z_OK)
          throw(status);
      }
      status = inflateEnd(&stream);
      if (status != Z_OK)
        throw(status);
      s = oss.str();
    }
  }

  std::string deflateString(const std::string& s) {
    mz_ulong compressedLength = compressBound(static_cast<mz_ulong>(s.size()));
    unsigned char* cmpr = static_cast<unsigned char*>(::malloc(compressedLength*sizeof(unsigned char)));
    int status = compress(cmpr,&compressedLength,reinterpret_cast<const unsigned char*>(&s[0]),static_cast<mz_ulong>(s.size()));
    if (status != Z_OK) {
      ::free(cmpr);
      throw(status);
    }
    std::string ret(reinterpret_cast<const char*>(cmpr),compressedLength);
    ::free(cmpr);
    return ret;
  }

  std::string encodeBase64(const std::string& s) {
    base64::encoder E;
    std::ostringstream oss;
    oss << "@"; // add leading "@" to distinguish from valid MiniZinc code
    std::istringstream iss(s);
    E.encode(iss, oss);
    return oss.str();
  }

  std::string decodeBase64(const std::string& s) {
    if (s.size()==0 || s[0] != '@')
      throw InternalError("string is not base64 encoded");
    base64::decoder D;
    std::ostringstream oss;
    std::istringstream iss(s);
    (void) iss.get(); // remove leading "@"
    D.decode(iss, oss);
    return oss.str();
  }

}}
