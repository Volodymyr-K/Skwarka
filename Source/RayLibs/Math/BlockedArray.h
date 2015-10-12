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

#ifndef BLOCKED_ARRAY_H
#define BLOCKED_ARRAY_H

#include <Common/Common.h>
#include <vector>
#include <algorithm>

/**
* Blocked 2D array implementation.
* The values are grouped into square blocks. Values in each block are ordered consecutively into memory thus reducing cache misses when neighbor elements are accessed.
* The blocked array only has performance advantage comparing to a usual array when neighbor elements are frequently accessed (like in image texture or film implementation).
* 
* This is a class template with a value type as the first template parameter. The second template parameter is a binary logarithm of the block size (in one dimension).
*/
template<typename T, size_t block_size_log=2>
class BlockedArray
  {
  public:
    /**
    * Creates BlockedArray instance with the specified size. The values are initialized with their default constructor.
    * @param i_size_u Array size in U dimension. Should be greater than zero.
    * @param i_size_v Array size in V dimension. Should be greater than zero.
    */
    BlockedArray(size_t i_size_u, size_t i_size_v);

    /**
    * Creates BlockedArray instance from the specified 2D array.
    * @param i_values Source 2D array. All inner vectors should have the same size. Should have at least one row and at least one column.
    */
    BlockedArray(const std::vector<std::vector<T>> &i_values);

    /**
    * Returns size of the 2D array in the first dimension (U dimension).
    */
    size_t GetSizeU() const;

    /**
    * Returns size of the 2D array in the second dimension (V dimension).
    */
    size_t GetSizeV() const;

    /**
    * Returns non-const reference to the element with the specified indices.
    */
    T &Get(size_t i_u, size_t i_v);

    /**
    * Returns const reference to the element with the specified indices.
    */
    const T &Get(size_t i_u, size_t i_v) const;

    /**
    * Fills array with the specified value.
    */
    void Fill(const T &i_value);

  private:
    /**
    * Private helper method that given a coordinate (in any of the dimension) returns the block index this coordinate corresponds to.
    */
    size_t _BlockIndex(size_t i_x) const;

    /**
    * Private helper method that given a coordinate (in any of the dimension) returns the offset in the block this coordinate corresponds to.
    */
    size_t _BlockOffset(size_t i_x) const;

    /**
    * Private helper method that given two coordinates returns the corresponding index in the private 1D array.
    */
    size_t _ElementIndex(size_t i_u, size_t i_v) const;

  private:
    size_t m_size_u, m_size_v, m_u_blocks;
    std::vector<T> m_data;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, size_t block_size_log>
BlockedArray<T,block_size_log>::BlockedArray(size_t i_size_u, size_t i_size_v):
m_size_u(i_size_u), m_size_v(i_size_v)
  {
  ASSERT(i_size_u>0 && i_size_v>0); 

  size_t block_size = (size_t)1 << block_size_log;
  size_t rounded_u_size = (i_size_u+block_size-1) & ~(block_size-1);
  size_t rounded_v_size = (i_size_v+block_size-1) & ~(block_size-1);
  m_u_blocks = rounded_u_size >> block_size_log;

  m_data.assign(rounded_u_size*rounded_v_size, T());
  }

template<typename T, size_t block_size_log>
BlockedArray<T,block_size_log>::BlockedArray(const std::vector<std::vector<T>> &i_values):
m_size_u(i_values.size()), m_size_v(i_values[0].size())
  {
  ASSERT(i_values.size()>0 && i_values[0].size()>0);

  size_t block_size = (size_t)1 << block_size_log;
  size_t rounded_u_size = (m_size_u+block_size-1) & ~(block_size-1);
  size_t rounded_v_size = (m_size_v+block_size-1) & ~(block_size-1);
  m_u_blocks = rounded_u_size >> block_size_log;

  m_data.assign(rounded_u_size*rounded_v_size, T());
  for(size_t i=0;i<m_size_u;++i)
    {
    ASSERT(i_values[i].size()==i_values[0].size());

    for(size_t j=0;j<m_size_v;++j)
      Get(i,j)=i_values[i][j];
    }
  }

template<typename T, size_t block_size_log>
size_t BlockedArray<T,block_size_log>::GetSizeU() const
  {
  return m_size_u;
  }

template<typename T, size_t block_size_log>
size_t BlockedArray<T,block_size_log>::GetSizeV() const
  {
  return m_size_v;
  }

template<typename T, size_t block_size_log>
size_t BlockedArray<T,block_size_log>::_BlockIndex(size_t i_x) const
  {
  return i_x >> block_size_log;
  }

template<typename T, size_t block_size_log>
size_t BlockedArray<T,block_size_log>::_BlockOffset(size_t i_x) const
  {
  size_t block_size = (size_t)1 << block_size_log;
  return (i_x & (block_size - 1));
  }

template<typename T, size_t block_size_log>
size_t BlockedArray<T,block_size_log>::_ElementIndex(size_t i_u, size_t i_v) const
  {
  ASSERT(i_u < m_size_u && i_v < m_size_v);

  size_t block_u = _BlockIndex(i_u), block_v = _BlockIndex(i_v);
  size_t offset_u = _BlockOffset(i_u), offset_v = _BlockOffset(i_v);
  size_t offset = (m_u_blocks * block_v + block_u) << (2*block_size_log);
  offset += (offset_v<<block_size_log) + offset_u;

  return offset;
  }

template<typename T, size_t block_size_log>
T &BlockedArray<T,block_size_log>::Get(size_t i_u, size_t i_v)
  {
  return m_data[_ElementIndex(i_u,i_v)];
  }

template<typename T, size_t block_size_log>
const T &BlockedArray<T,block_size_log>::Get(size_t i_u, size_t i_v) const
  {
  return m_data[_ElementIndex(i_u,i_v)];
  }

template<typename T, size_t block_size_log>
void BlockedArray<T,block_size_log>::Fill(const T &i_value)
  {
  std::fill(m_data.begin(), m_data.end(), i_value);
  }

#endif // BLOCKED_ARRAY_H