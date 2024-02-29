/*
Copyright (c) 2024 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <cstddef>  // std::size_t
#include <cassert>  // assert
#include <mutex>  // NOLINT [build/c++11] std::mutex, std::lock_guard

#ifdef MEMPOL_DEBUG
#include <iostream>
#endif

namespace MemoryPool {

template <std::size_t nrBlocks, std::size_t blocksize>
class Fixed {
 public:
  Fixed()
  : _buffer{0}
  , _head(_buffer) {
    unsigned char* b = _buffer;
    std::size_t adjustedBlocksize = sizeof(sizeof(unsigned char*)) > sizeof(blocksize) ? sizeof(sizeof(unsigned char*)) : sizeof(blocksize);
    std::size_t i = blocksize - 1;
    while (i) {
      *b = b + adjustedBlocksize;
      b += adjustedBlocksize;
      --i;
    }
    *b = nullptr;
  }

  // no copy nor move
  Fixed (const Fixed&) = delete;
  Fixed& operator= (const Fixed&) = delete;

  void* malloc() {
    const std::lock_guard<std::mutex> lockGuard(_mutex);
    if (_head) {
      void* retVal = _head;
      _head = *_head;
      return retVal;
    }
    return nullptr;
  }

  void free(void* ptr) {
    const std::lock_guard<std::mutex> lockGuard(_mutex);
    *ptr = _head;
    _head = ptr;
  }

  std::size_t freeMemory() {
    const std::lock_guard<std::mutex> lockGuard(_mutex);
    unsigned char* i = _head;
    std::size_t retVal = 0;
    while (i) {
      retVal += blocksize;
      i = *i;
    }
    return retVal;
  }

  #ifdef MEMPOL_DEBUG
  void print() {
    std::size_t adjBlocksize = sizeof(sizeof(unsigned char*)) > sizeof(blocksize) ? sizeof(sizeof(unsigned char*)) : sizeof(blocksize);
    std::cout << "+--------------------" << std::endl;
    std::cout << "|start:" << static_cast<void*>(_buffer) << std::endl;
    std::cout << "|blocks:" << nrBlocks << std::endl;
    std::cout << "|blocksize:" << adjBlocksize << std::endl;
    std::cout << "|head: " << static_cast<void*>(_head) << std::endl;
    unsigned char* nextFreeBlock = _head;
    unsigned char* currentBlock = _buffer;

    for (std::size_t i = 0; i < nrBlocks; ++i) {
      std::cout << "|" << i + 1 << ": " << static_cast<void*>(currentBlock) << std::endl;
      if (currentBlock == nextFreeBlock) {
        std::cout << "|   free" << std::endl;
        nextFreeBlock = *currentBlock;
        std::cout << "|   next: " << static_cast<void*>(nextFreeBlock) << std::endl;
      } else {
        std::cout << "|   allocated" << std::endl;
      }
    }
    std::cout << "+--------------------" << std::endl;
  }
  #endif

 private:
  unsigned char _buffer[nrBlocks * (sizeof(sizeof(unsigned char*)) > sizeof(blocksize) ? sizeof(sizeof(unsigned char*)) : sizeof(blocksize))];
  unsigned char* _head;
  std::mutex _mutex;
};

}  // end namespace MemoryPool
