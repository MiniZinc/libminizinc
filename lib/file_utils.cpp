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
#include <minizinc/config.hh>

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
#include <libgen.h>
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
    if (!GetFullPathName(filename.c_str(), nBufferLength, lpBuffer, &lpFilePart)) {
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
  
}}
