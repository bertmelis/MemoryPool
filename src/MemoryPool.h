/*
Copyright (c) 2024 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <stdlib.h>  // malloc, free
#include <cstddef>  // std::size_t
#include <cassert>  // assert
#include <mutex>  // NOLINT [build/c++11] std::mutex, std::lock_guard

#ifdef MEMPOL_DEBUG
#include <iostream>
#endif

namespace MemoryPool {

class Variable {
 public:
  Variable(std::size_t nrBlocks, std::size_t blocksize);
  ~Variable();

  // no copy nor move
  Variable (const Variable&) = delete;
  Variable& operator= (const Variable&) = delete;

  void* malloc(size_t size);
  void free(void* ptr);

  std::size_t freeMemory();
  std::size_t maxBlockSize();

  #ifdef MEMPOL_DEBUG
  void print();
  #endif

 private:
  struct BlockHeader {
    BlockHeader* next;
    std::size_t size;
  };
  std::size_t _normBlocksize;
  unsigned char* _buffer;
  BlockHeader* _head;
  std::mutex _mutex;

  #ifdef MEMPOL_DEBUG
  std::size_t _bufferSize;
  #endif
};

}  // end namespace MemoryPool
