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

struct MemTestStruct {
  size_t size;
  int value;
  unsigned char* data[10];
};

void emptyPool() {
  const size_t nrBlocks = 3;
  const size_t blocksize = sizeof(MemTestStruct);
  MemoryPool::Fixed<nrBlocks, blocksize> pool;

  pool.print();

  TEST_ASSERT_EQUAL_UINT(nrBlocks * blocksize, pool.freeMemory());
}

void mallocFull() {
  const size_t nrBlocks = 3;
  const size_t blocksize = sizeof(MemTestStruct);
  MemoryPool::Fixed<nrBlocks, blocksize> pool;

  MemTestStruct* var1 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var2 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var3 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var4 = reinterpret_cast<MemTestStruct*>(pool.malloc());

  pool.print();

  TEST_ASSERT_NOT_NULL(var1);
  TEST_ASSERT_NOT_NULL(var2);
  TEST_ASSERT_NOT_NULL(var3);
  TEST_ASSERT_NULL(var4);

  var1->value = 1;
  var2->value = 2;
  var3->value = 3;
  TEST_ASSERT_EQUAL_INT(1, var1->value);
  TEST_ASSERT_EQUAL_INT(2, var2->value);
  TEST_ASSERT_EQUAL_INT(3, var3->value);
  TEST_ASSERT_EQUAL_UINT(0, pool.freeMemory());
}

void freePartial() {
  const size_t nrBlocks = 4;
  const size_t blocksize = sizeof(MemTestStruct);
  MemoryPool::Fixed<nrBlocks, blocksize> pool;

  MemTestStruct* var1 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var2 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var3 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var4 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var5 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  pool.print();

  (void) var1;
  (void) var3;
  pool.free(var2);
  pool.print();
  TEST_ASSERT_EQUAL_UINT(1 * blocksize, pool.freeMemory());
  pool.free(var4);
  pool.print();
  TEST_ASSERT_EQUAL_UINT(2 * blocksize, pool.freeMemory());
  var5 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  TEST_ASSERT_NOT_NULL(var5);
  pool.print();

  TEST_ASSERT_EQUAL_UINT(1 * blocksize, pool.freeMemory());
}

void freeEmpty() {
  const size_t nrBlocks = 4;
  const size_t blocksize = sizeof(MemTestStruct);
  MemoryPool::Fixed<nrBlocks, blocksize> pool;

  MemTestStruct* var1 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var2 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var3 = reinterpret_cast<MemTestStruct*>(pool.malloc());
  MemTestStruct* var4 = reinterpret_cast<MemTestStruct*>(pool.malloc());

  pool.print();
  pool.free(var1);
  pool.print();
  pool.free(var2);
  pool.print();
  pool.free(var3);
  pool.print();
  pool.free(var4);
  pool.print();

  TEST_ASSERT_EQUAL_UINT(nrBlocks * blocksize, pool.freeMemory());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(emptyPool);
  RUN_TEST(mallocFull);
  RUN_TEST(freePartial);
  RUN_TEST(freeEmpty);
  return UNITY_END();
}
