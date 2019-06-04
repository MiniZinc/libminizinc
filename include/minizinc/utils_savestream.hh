/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_UTILS_SAVESTREAM_H__
#define __MINIZINC_UTILS_SAVESTREAM_H__

#include <cstdio>

namespace MiniZinc {

  /// Helper class to redirect, e.g., stdout to stderr
  class StreamRedir {
    /// The stream to be changed
    FILE * const d_s0;
    /*
     * Structure for retaining information about a stream, sufficient to
     * recreate that stream later on.
     * See https://stackoverflow.com/questions/4760201/how-do-i-suppress-output-while-using-a-dynamic-library
     */
    struct stream_info {
        int fd=-1;
        fpos_t pos;
    };
    /// The original stream
    stream_info d_si;
  public:
    /// Constructs with the stream to be changed
//    StreamRedir(FILE* s0);
    /// Constructs with s0 and replaces it by s1
    StreamRedir(FILE* s0, FILE* s1, bool fFlush=true);
    ~StreamRedir();
    /// Restore original
    void restore(bool fFLush=true);
  protected:
    /// Replace & save stream by s1
    void replaceStream(FILE* s1, bool fFlush=true);
  };


}

#endif  // __MINIZINC_UTILS_SAVESTREAM_H__

