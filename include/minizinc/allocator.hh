/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_ALLOCATOR_HH__
#define __MINIZINC_ALLOCATOR_HH__

#include <cstdlib>
#include <algorithm>
#include <cassert>

namespace MiniZinc {

class MemoryBlock {
public:
  MemoryBlock* next;
  size_t size;
  char data[1];
};

class BlockAllocator {
private:
  /// Default size of blocks to allocate
  static const size_t blockSize = 1<<20;
  
  /// The current memory block
  MemoryBlock* curBlock;
  /// The pointer into the current memory block
  char* curP;
  /// The end of the current memory block
  char* endP;
  
  /// Allocate new block of memory
  void allocateBlock(size_t bs);
public:
  /// Allocate \a size bytes of memory (word-aligned)
  void* alloc(size_t size);
  
  /// Allocate one object of type T (no initialisation)
  template<typename T>
  T* alloc(void) { return static_cast<T*>(alloc(sizeof(T))); }
  
  /// Allocate \a n objects of type T (no initialisation)
  template<typename T>
  T* alloc(int n) { return static_cast<T*>(alloc(n*sizeof(T))); }
  
  /// Constructor
  BlockAllocator(void) : curBlock(NULL), curP(NULL), endP(NULL) {}
  
  /// Destructor (deallocates all memory)
  ~BlockAllocator(void);
};
  
}

#endif
