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

      std::vector<double, MemoryPoolAllocator<double> > v(alloc);
      for(size_t i=0;i<1234;++i)
        v.push_back(i);

      for(size_t i=0;i<1234;++i)
        if (v[i]!=i)
          {
          TS_FAIL("Verification failed");
          break;
          }
      }
  };

#endif // MEMORY_POOL_TEST_H