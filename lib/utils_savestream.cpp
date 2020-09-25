/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <io.h>
#else
#include <unistd.h>
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
  _streamInfo.fd = dup(fileno(_file0));
  dup2(fileno(s1), fileno(_file0));
}

void StreamRedir::restore(bool fFLush) {
  if (fFLush) {
    fflush(_file0);
  }
  dup2(_streamInfo.fd, fileno(_file0));
  close(_streamInfo.fd);
  clearerr(_file0);
  fsetpos(_file0, &(_streamInfo.pos));
}
