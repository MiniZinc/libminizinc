/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/allocator.hh>

namespace MiniZinc {

  const size_t
  BlockAllocator::blockSize;

  void
  BlockAllocator::allocateBlock(size_t bs) {
    bs = std::max(bs,blockSize);
    MemoryBlock* newBlock =
      static_cast<MemoryBlock*>(::malloc(sizeof(MemoryBlock)+bs));
    newBlock->size = bs;
    newBlock->next = curBlock;
    curBlock = newBlock;
    curP = curBlock->data;
    endP = curBlock->data+bs;
  }

  void*
  BlockAllocator::alloc(size_t size) {
    /// Align to word boundary
    size += ((8 - (size & 7)) & 7);
    if (curP + size >= endP)
      allocateBlock(size);
    char* ret = curP;
    curP += size;
    return ret;
  }

  BlockAllocator::~BlockAllocator(void) {
    while (curBlock) {
      MemoryBlock* n = curBlock->next;
      ::free(curBlock);
      curBlock = n;
    }
  }
  
}
