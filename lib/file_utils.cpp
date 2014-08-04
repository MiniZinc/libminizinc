/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/file_utils.hh>
#include <config.hh>

#ifdef HAS_PIDPATH
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libproc.h>
#include <unistd.h>
#elif defined(HAS_GETMODULEFILENAME)
#include <windows.h>
#else
#include <unistd.h>
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
    if (FILE *file = fopen(filename.c_str(), "r")) {
      fclose(file);
      return true;
    } else {
      return false;
    }
  }
  
}}
