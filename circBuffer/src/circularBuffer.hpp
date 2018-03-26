//
//  circularBuffer.hpp
//  circBuffer
//
//  Created by Christofero Pollano on 26/02/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#pragma once
#include "common.hpp"
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <iterator>

namespace dino
{
template <class T>
  class circularBuffer
  {
   public:
    circularBuffer ()
        : bufferLength (0),
          writeHead (0),
          readHead (0){};

    ~circularBuffer (){};

    /** Rounds the size given to the nearest power of two*/
    void init (size_t length, T initialValue)
    {
      bufferLength = std::pow (2, std::ceil (log2 (length)));
      buffer.resize (bufferLength, initialValue);
    }

    /** adds one at the current writeHead then increments writeHead */
    inline void insertOne (const T val)
    {
      buffer[writeHead] = val;
      ++writeHead &= bufferLength - 1;
    }

    /** Inserts as many as set in the write head and increments the write head */
    inline void insertMany (const T* values, size_t length)
    {
      for (int i = 0; i < length; i++)
      {
        buffer[writeHead] = values[i];
        ++writeHead &= bufferLength - 1;
      }
    }

    /** returns the nth thing from the writeHead*/
    inline T getOne (size_t nFrom)
    {
      assert (nFrom <= bufferLength);
      long int offset = writeHead - nFrom;

      readHead = ((offset & bufferLength - 1) + bufferLength) & bufferLength - 1;
      T output = buffer[readHead];
      return output;
    }

    inline T getInterpolatedOne (float nFrom)
    {
      static_assert (std::is_arithmetic<T> (),"must be float (or double) to get interpolated value");
    }

    /** returns a vector of things starting from the most recently added to  the nth*/
    inline std::vector<T> getMany (size_t nFrom)
    {
      assert (nFrom < bufferLength);
      std::vector<T> output (nFrom, NULL);
      readHead = writeHead;
      for (size_t i = 0; i < nFrom; i++)
      {
        output[i] = buffer[readHead];
        ((--readHead & bufferLength - 1) + bufferLength) & bufferLength - 1;
      }
      
      return output;
    }

    inline std::vector<T> getUnwrapped ()
    {
      std::vector<T> unwrapped;
      typename std::vector<T>::iterator oldestElement = buffer.begin () + writeHead;
      unwrapped.insert (unwrapped.begin (), oldestElement, buffer.end ());
      unwrapped.insert (unwrapped.end (), buffer.begin (),buffer.begin () + (writeHead));

      return unwrapped;
    }

    size_t getLength () { return bufferLength; };
    
    /** fills the buffer with initial values of the type given*/
    void   flush ()
    {
      if (bufferLength > 0)
      {
        std::fill (buffer.begin (), buffer.end (), T());
      }
    }

   private:
    std::vector<T>  buffer;
    signed long int bufferLength;
    size_t          writeHead;
    size_t          readHead;
  };
  
  template <class T>
  class writeHeadIterator : public std::iterator <std::random_access_iterator_tag, T>
  {
    public:
      typedef typename std::vector<T>::iterator iterator;
      /** give the beging and end iterators to the data you're pointing at */
      writeHeadIterator (iterator b, iterator e) : iter (b), end (e)
      {
        numElements = std::distance (iter, end);
      }

      /** will insert value then increment the iterator */
      void insert (T val)
      {
        *iter = val;
        if (++iter == end)
          iter -= numElements;
      }

    private:
      iterator iter;
      const iterator end;
      size_t numElements;
  };
  
  template <class T>
  class readHead : public std::iterator<std::random_access_iterator_tag, T>
  {
    public:
     typedef typename std::vector<T>::iterator iterator;

    private:
  }
}
/* circularBuffer_hpp */
