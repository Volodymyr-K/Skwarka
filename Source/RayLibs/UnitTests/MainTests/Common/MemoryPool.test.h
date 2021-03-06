/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MEMORY_POOL_TEST_H
#define MEMORY_POOL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <vector>

class MemoryPoolTestSuite : public CxxTest::TestSuite
  {
  public:

    // Allocates a number of memory chunks, writes a test data and verifies the written data.
    void test_MemoryPool_WriteAndRead()
      {
      MemoryPool pool;

      std::vector<unsigned char *> ptrs;
      for(size_t i=0;i<100;++i)
        {
        unsigned char *ptr = (unsigned char*)(pool.Alloc(3276));
        ptrs.push_back(ptr);

        for(size_t j=0;j<3276;++j)
          ptr[j]=(i+j)%256;
        }

      bool correct=true;
      for(size_t i=0;i<100;++i)
        {
        unsigned char *ptr = ptrs[i];
        for(size_t j=0;j<3276;++j)
          if (ptr[j]!=(i+j)%256)
            correct=false;
        }

      pool.FreeAll();

      TS_ASSERT(correct);
      }

    // Allocates a chunk bigger than the block size, writes a test data to it and verifies the written data.
    void test_MemoryPool_BigChunk()
      {
      MemoryPool pool(32768);
      unsigned char *ptr = (unsigned char*)(pool.Alloc(327680));
      for(size_t j=0;j<327680;++j)
        ptr[j]=j%256;

      bool correct=true;
      for(size_t j=0;j<327680;++j)
        if (ptr[j]!=j%256)
          correct=false;

      pool.FreeAll();

      TS_ASSERT(correct);
      }

    // Tests FreeAll() method by allocating a number of chunks, freeing them and then allocating again.
    void test_MemoryPool_FreeAll()
      {
      MemoryPool pool;

      for(size_t i=0;i<50;++i)
        {
        unsigned char *ptr = (unsigned char*)(pool.Alloc(3276));
        for(size_t j=0;j<3276;++j)
          ptr[j]=0;
        }

      pool.FreeAll();

      std::vector<unsigned char *> ptrs;
      for(size_t i=0;i<100;++i)
        {
        unsigned char *ptr = (unsigned char*)(pool.Alloc(3276));
        ptrs.push_back(ptr);

        for(size_t j=0;j<3276;++j)
          ptr[j]=(i+j)%256;
        }

      bool correct=true;
      for(size_t i=0;i<100;++i)
        {
        unsigned char *ptr = ptrs[i];
        for(size_t j=0;j<3276;++j)
          if (ptr[j]!=(i+j)%256)
            correct=false;
        }

      pool.FreeAll();

      TS_ASSERT(correct);
      }

    // Tests ReleaseUnusedMemory() method by allocating a number of chunks, freeing them and then releasing the memory.
    void test_MemoryPool_ReleaseUnusedMemory()
      {
      MemoryPool pool;
      for(size_t i=0;i<50;++i)
        {
        unsigned char *ptr = (unsigned char*)(pool.Alloc(3276));
        for(size_t j=0;j<3276;++j)
          ptr[j]=0;
        }

      TS_ASSERT( pool.ReleaseUnusedMemory()==false );

      pool.FreeAll();

      TS_ASSERT( pool.ReleaseUnusedMemory() );
      }

    void test_MemoryPoolAllocator()
      {
      MemoryPool pool;
      MemoryPoolAllocator<double> alloc(pool);

      std::vector<double, MemoryPoolAllocator<double>> v(alloc);
      for(size_t i=0;i<1234;++i)
        v.push_back((double)i);

      for(size_t i=0;i<1234;++i)
        if (v[i]!=i)
          {
          TS_FAIL("Verification failed");
          break;
          }
      }
  };

#endif // MEMORY_POOL_TEST_H