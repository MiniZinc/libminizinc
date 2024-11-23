/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#include <io.h>
#define mzn_dup _dup
#define mzn_dup2 _dup2
#define mzn_close _close
#define mzn_fileno _fileno
#else
#include <unistd.h>
#define mzn_dup dup
#define mzn_dup2 dup2
#define mzn_close close
#define mzn_fileno fileno
#endif

#include <minizinc/utils_savestream.hh>

#include <iostream>

using namespace std;
using namespace MiniZinc;

// StreamRedir::StreamRedir(FILE *s0) : d_s0(s0) { }

StreamRedir::StreamRedir(FILE* s0, FILE* s1, bool fFlush) : _file0(s0) {
  replaceStream(s1, fFlush);
}
StreamRedir::~StreamRedir() { restore(); }

void StreamRedir::replaceStream(FILE* s1, bool fFlush) {
  if (fFlush) {
    fflush(_file0);
  }
  fgetpos(_file0, &(_streamInfo.pos));
  _streamInfo.fd = mzn_dup(mzn_fileno(_file0));
  mzn_dup2(mzn_fileno(s1), mzn_fileno(_file0));
}

void StreamRedir::restore(bool fFLush) {
  if (fFLush) {
    fflush(_file0);
  }
  mzn_dup2(_streamInfo.fd, mzn_fileno(_file0));
  mzn_close(_streamInfo.fd);
  clearerr(_file0);
  fsetpos(_file0, &(_streamInfo.pos));
}
