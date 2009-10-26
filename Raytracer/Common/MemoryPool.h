#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <vector>
#include <numeric>

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
    // Not implemented, not a value type.
    MemoryPool(const MemoryPool&);
    MemoryPool &operator=(const MemoryPool&);

  private:
    size_t m_current_block_pos, m_block_size;
    char *m_current_block;

    std::vector<char *> m_used_blocks, m_available_blocks;
  };

/**
* Custom allocator for STL containers based on MemoryPool.
* Just like the MemoryPool, the allocator can only allocate objects, it can not deallocate them (the corresponding method is empty).
* Although the allocator has state (memory pool object), it is not a problem because the memory pool is never used for deallocation.
*/
template<class T>
class MemoryPoolAllocator
  {
  public:
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;
    template <class U>
    struct rebind { typedef MemoryPoolAllocator<U> other; };

    MemoryPoolAllocator(MemoryPool &i_pool);
    MemoryPoolAllocator(const MemoryPoolAllocator &i_allocator);

    template <class U> MemoryPoolAllocator(const MemoryPoolAllocator<U> &i_allocator);
    ~MemoryPoolAllocator();

    pointer address(reference i_ref) const;
    const_pointer address(const_reference i_ref) const;
    pointer allocate(size_type i_count);
    void deallocate(pointer i_p, size_type i_n);
    size_type max_size() const;
    void construct(pointer i_p, const T& i_val);
    void destroy(pointer i_p);

  private:
    MemoryPool &m_pool;
  };

template<class T, class Other>
bool operator==(const MemoryPoolAllocator<T>&, const MemoryPoolAllocator<Other>&);

template<class T, class Other>
bool operator!=(const MemoryPoolAllocator<T>&, const MemoryPoolAllocator<Other>&);

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////// MemoryPool //////////////////////////////////////////////////

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

///////////////////////////////////////// MemoryPoolAllocator /////////////////////////////////////////////

template<class T>
MemoryPoolAllocator<T>::MemoryPoolAllocator(MemoryPool &i_pool): m_pool(i_pool)
  {
  }

template<class T>
MemoryPoolAllocator<T>::MemoryPoolAllocator(const MemoryPoolAllocator &i_allocator): m_pool(i_allocator.m_pool)
  {
  }

template<class T>
template <class U> MemoryPoolAllocator<T>::MemoryPoolAllocator(const MemoryPoolAllocator<U> &i_allocator): m_pool(i_allocator.m_pool)
  {
  }

template<class T>
MemoryPoolAllocator<T>::~MemoryPoolAllocator()
  {
  }

template<class T>
typename MemoryPoolAllocator<T>::pointer MemoryPoolAllocator<T>::address(typename MemoryPoolAllocator<T>::reference i_ref) const
  {
  return (&i_ref);
  }

template<class T>
typename MemoryPoolAllocator<T>::const_pointer MemoryPoolAllocator<T>::address(typename MemoryPoolAllocator<T>::const_reference i_ref) const
  {
  return (&i_ref);
  }

template<class T>
typename MemoryPoolAllocator<T>::pointer MemoryPoolAllocator<T>::allocate(typename MemoryPoolAllocator<T>::size_type i_count)
  {
  return (T*)m_pool.Alloc(i_count*sizeof(T));
  }

template<class T>
void MemoryPoolAllocator<T>::deallocate(typename MemoryPoolAllocator<T>::pointer i_p, typename MemoryPoolAllocator<T>::size_type i_n)
  {
  // Do nothing.
  }

template<class T>
typename MemoryPoolAllocator<T>::size_type MemoryPoolAllocator<T>::max_size() const
  {
  size_t count = std::numeric_limits<size_t>::max() / sizeof (size_t);
  return count > 0 ? count : 1;
  }

template<class T>
void MemoryPoolAllocator<T>::construct(typename MemoryPoolAllocator<T>::pointer i_p, const T& i_val)
  {
  ASSERT(i_p);
  void *void_ptr = i_p;
  ::new (void_ptr) T(i_val);
  }

template<class T>
void MemoryPoolAllocator<T>::destroy(typename MemoryPoolAllocator<T>::pointer i_p)
  {
  ASSERT(i_p);
  i_p->~T();
  }

/**
* As the STL conventions require, the == operator always returns true.
*/
template<class T, class Other>
bool operator==(const MemoryPoolAllocator<T>&, const MemoryPoolAllocator<Other>&)
  {
  return true;
  }

/**
* As the STL conventions require, the != operator always returns false.
*/
template<class T, class Other>
bool operator!=(const MemoryPoolAllocator<T>&, const MemoryPoolAllocator<Other>&)
  {
  return false;
  }

#endif // MEMORY_POOL_H