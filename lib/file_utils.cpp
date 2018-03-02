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

#ifndef _MSC_VER
#include <dirent.h>
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

  std::string file_path(const std::string& filename) {
#ifdef _MSC_VER
    LPSTR lpBuffer, lpFilePart;
    DWORD nBufferLength = GetFullPathName(filename.c_str(), 0,0,&lpFilePart);
    if (!(lpBuffer = (LPTSTR)LocalAlloc(LMEM_FIXED, sizeof(TCHAR) * nBufferLength)))
      return 0;
    std::string ret;
    if (!GetFullPathName(filename.c_str(), nBufferLength, lpBuffer, &lpFilePart)) {
      ret = "";
    } else {
      ret = std::string(lpBuffer);
    }
    LocalFree(lpBuffer);
    return ret;
#else
    char* rp = realpath(filename.c_str(), NULL);
    std::string rp_s(rp);
    free(rp);
    return rp_s;
#endif
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
      if (cc[0]==0x1F & cc[1]==0x8B) {
        dataStart = cc+10;
        windowBits = -Z_DEFAULT_WINDOW_BITS;
        if (cc[3] & 0x4) {
          dataStart += 2;
          if (dataStart-cc >= s.size())
            throw(-1);
        }
        if (cc[3] & 0x8) {
          while (*dataStart != '\0') {
            dataStart++;
            if (dataStart-cc >= s.size())
              throw(-1);
          }
          dataStart++;
          if (dataStart-cc >= s.size())
            throw(-1);
        }
        if (cc[3] & 0x10) {
          while (*dataStart != '\0') {
            dataStart++;
            if (dataStart-cc >= s.size())
              throw(-1);
          }
          dataStart++;
          if (dataStart-cc >= s.size())
            throw(-1);
        }
        if (cc[3] & 0x2) {
          dataStart += 2;
          if (dataStart-cc >= s.size())
            throw(-1);
        }
        dataLen = s.size() - (dataStart-cc);
      } else {
        dataStart = cc;
        windowBits = Z_DEFAULT_WINDOW_BITS;
        dataLen = s.size();
      }
      
      stream.next_in = dataStart;
      stream.avail_in = dataLen;
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
    size_t compressedLength = compressBound(s.size());
    unsigned char* cmpr = static_cast<unsigned char*>(::malloc(compressedLength*sizeof(unsigned char)));
    int status = compress(cmpr,&compressedLength,reinterpret_cast<const unsigned char*>(&s[0]),s.size());
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
    oss << ":"; // add leading ":" to distinguish from valid MiniZinc code
    std::istringstream iss(s);
    E.encode(iss, oss);
    return oss.str();
  }

  std::string decodeBase64(const std::string& s) {
    if (s.size()==0 || s[0] != ':')
      throw InternalError("string is not base64 encoded");
    base64::decoder D;
    std::ostringstream oss;
    std::istringstream iss(s);
    (void) iss.get(); // remove leading ":"
    D.decode(iss, oss);
    return oss.str();
  }

}}
