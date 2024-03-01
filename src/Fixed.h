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
    unsigned char* b = _head;
    std::cout << "head: " << reinterpret_cast<void*>(_head) << std::endl;
    std::size_t adjustedBlocksize = sizeof(sizeof(unsigned char*)) > sizeof(blocksize) ? sizeof(sizeof(unsigned char*)) : sizeof(blocksize);
    std::size_t currentIndex = 0;
    std::size_t nextIndex = currentIndex + adjustedBlocksize;
    for (std::size_t i = 0; i < nrBlocks - 1; ++i) {
      std::cout << "set " << reinterpret_cast<void*>(&b[currentIndex]) << " to ";
      std::cout << reinterpret_cast<void*>(&b[nextIndex]) << std::endl;
      reinterpret_cast<unsigned char**>(b)[currentIndex] = &b[nextIndex];
      currentIndex = nextIndex;
      nextIndex += adjustedBlocksize;
    }
    std::cout << "set " << reinterpret_cast<void*>(&b[currentIndex]) << " to 0" << std::endl;
    reinterpret_cast<unsigned char**>(b)[currentIndex] = nullptr;
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
    std::size_t adjustedBlocksize = sizeof(sizeof(unsigned char*)) > sizeof(blocksize) ? sizeof(sizeof(unsigned char*)) : sizeof(blocksize);
    unsigned char* i = _head;
    std::size_t retVal = 0;
    while (i) {
      retVal += adjustedBlocksize;
      i = reinterpret_cast<unsigned char**>(i)[0];
    }
    return retVal;
  }

  #ifdef MEMPOL_DEBUG
  void print() {
    std::size_t adjustedBlocksize = sizeof(sizeof(unsigned char*)) > sizeof(blocksize) ? sizeof(sizeof(unsigned char*)) : sizeof(blocksize);
    std::cout << "+--------------------" << std::endl;
    std::cout << "|start:" << reinterpret_cast<void*>(_buffer) << std::endl;
    std::cout << "|blocks:" << nrBlocks << std::endl;
    std::cout << "|blocksize:" << adjustedBlocksize << std::endl;
    std::cout << "|head: " << reinterpret_cast<void*>(_head) << std::endl;
    unsigned char* currentBlock = _buffer;

    for (std::size_t i = 0; i < nrBlocks; ++i) {
      std::cout << "|" << i + 1 << ": " << reinterpret_cast<void*>(currentBlock) << std::endl;
      std::cout << "|   free" << std::endl;
      std::cout << "|   next: " << reinterpret_cast<void*>(*reinterpret_cast<unsigned char**>(currentBlock)) << std::endl;
      currentBlock += adjustedBlocksize;
    }
    std::cout << "+--------------------" << std::endl;
  }

  bool _isFree(unsigned char* ptr) {
    unsigned char* b = _head;
    while (b) {
      if (b == ptr) return true;
      b = *reinterpret_cast<unsigned char**>(b);
    }
    std::cout << reinterpret_cast<void*>(b) << " is allocated" << std::endl;
    return false;
  }
  #endif

 private:
  unsigned char _buffer[nrBlocks * (sizeof(sizeof(unsigned char*)) > sizeof(blocksize) ? sizeof(sizeof(unsigned char*)) : sizeof(blocksize))];
  unsigned char* _head;
  std::mutex _mutex;
};

}  // end namespace MemoryPool
