/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_PRINT_HH__
#define __MINIZINC_PRINT_HH__

#include <minizinc/model.hh>
#include <printer/Document.h>
#include <printer/PrettyPrinter.h>

namespace MiniZinc {
  
void print(std::ostream& os, Model* m);
void printDoc(std::ostream& os, Model* m);
  
}

#endif
