#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <vector>

/**
* Memory pool implementation that is used for fast memory allocation for small objects of unfixed size.
* The main difference between this implementation and the pool implementations provided by the boost library is that this implementation
* does not support freeing individual chunks but does support allocating chunks of different size.
* As usually, pool allocates memory blocks to serve allocating requests. When a block has no free memory left to serve a request
* a new memory block is allocated. When all chunks are freed all allocated memory blocks are stored for the future reuse.
* The class provides an option to release unused memory blocks.
*/
class MemoryPool
  {
  public:
    /**
    * Creates MemoryPool instance with the specified block size (in bytes).
    */
    MemoryPool(size_t i_block_size = 32768);

    /**
    * Releases all allocated memory blocks.
    */
    ~MemoryPool();

    /**
    * Allocates memory chunk of the specified size.
    */
    void *Alloc(size_t i_size);

    /**
    * Frees all allocated memory chunks. The allocated memory blocks are not released.
    */
    void FreeAll();

    /**
    * Frees every memory block that doesn't have any allocated chunks. Returns true if at least one memory block was freed.
    * Note, that exactly one memory block still won't be released. This is the block allocated to serve the next Alloc() request.
    */
    bool ReleaseMemory();

  private:
    size_t m_current_block_pos, m_block_size;
    char *m_current_block;

    std::vector<char *> m_used_blocks, m_available_blocks;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline MemoryPool::MemoryPool(size_t i_block_size):
  m_block_size(i_block_size), m_current_block_pos(0)
  {
  ASSERT(i_block_size>0);

  m_current_block = (char *)new char[i_block_size];
  }

inline MemoryPool::~MemoryPool()
  {
  delete[] m_current_block;
  for (size_t i = 0; i < m_used_blocks.size(); ++i)
    delete[] m_used_blocks[i];
  for (size_t i = 0; i < m_available_blocks.size(); ++i)
    delete[] m_available_blocks[i];
  }

inline void *MemoryPool::Alloc(size_t i_size)
  {
  // Round up i_size to minimum machine alignment.
  i_size = ((i_size + 7) & (~7));

  if (m_current_block_pos + i_size > m_block_size)
    {
    // Get new block of memory for MemoryPool.
    m_used_blocks.push_back(m_current_block);
    if (m_available_blocks.empty()==false && i_size <= m_block_size)
      {
      m_current_block = m_available_blocks.back();
      m_available_blocks.pop_back();
      }
    else
      m_current_block = (char *)new char[std::max(i_size, m_block_size)];

    m_current_block_pos = 0;
    }

  void *ret = m_current_block + m_current_block_pos;
  m_current_block_pos += i_size;
  return ret;
  }

inline void MemoryPool::FreeAll()
  {
  m_current_block_pos = 0;
  m_available_blocks.insert(m_available_blocks.end(), m_used_blocks.begin(), m_used_blocks.end());
  m_used_blocks.clear();
  }

inline bool MemoryPool::ReleaseMemory()
  {
  size_t available_blocks_size=m_available_blocks.size();
  for (size_t i = 0; i < available_blocks_size; ++i)
    delete[] m_available_blocks[i];

  m_available_blocks.clear();
  return available_blocks_size != 0;
  }

#endif // MEMORY_POOL_H