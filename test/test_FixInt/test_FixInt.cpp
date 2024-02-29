/*
Copyright (c) 2024 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#include <unity.h>

#include <MemoryPool.h>

void setUp() {}
void tearDown() {}

void emptyPool() {
  const size_t nrBlocks = 3;
  const size_t blocksize = sizeof(int);
  MemoryPool::Fixed<nrBlocks, blocksize> pool;

  size_t adjustedBlocksize = std::max(sizeof(void*), blocksize);

  pool.print();

  TEST_ASSERT_EQUAL_UINT(nrBlocks * adjustedBlocksize, pool.freeMemory());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(emptyPool);
  return UNITY_END();
}
