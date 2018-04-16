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
 
  /** circular iterator: [n] operators will return the value from the n elements from the iterator "head" which will increment with every call of the insert function*/
  template <class T>
  class circularIterator : public std::iterator <std::random_access_iterator_tag, T>
  {
      public:
        typedef typename std::vector<T>::iterator iterator;

        circularIterator()
        {
          distanceFromStart = 0;
        }
        /** give the beging and end iterators to the data you're pointing at */
        circularIterator (iterator b, iterator e) : iter (b), end (e), distanceFromStart (0)
        {
          numElements = std::distance (iter, end);
        }
  
        /** will insert value then increment the iterator */
        void insert (T val)
        {
//          assert (iter);
          *iter = val;
          ++distanceFromStart;
          if (++iter == end)
          {
            distanceFromStart -= numElements;
            iter              -= numElements;
          }
        }
  
        T operator[] (int n)
        {
//          assert (iter);
            ++n;
          return (n <= distanceFromStart)? *(iter - n) : *(end - (n-distanceFromStart));
        }

        T operator[] (float x)
        {
            assert  (std::is_floating_point<T>::value);
            float x1 = std::ceil (x);
            float x0 = std::floor (x);
            T y1 = (x1 <= distanceFromStart)? *(iter - x1) : *(end - (x1-distanceFromStart));
            T y0 = (x0 <= distanceFromStart)? *(iter - x0) : *(end - (x0-distanceFromStart));
            return y0 + (y1 - y0)/(x1 - x0) * (x - x0);
        }
    
         circularIterator<T>& operator= (const circularIterator<T> &rhs)
        {
          iter              = rhs.iter;
          end               = rhs.end;
          numElements       = rhs.numElements;
          distanceFromStart = rhs.distanceFromStart;
          return *this;
        }
  
      private:
        iterator iter;
        iterator end;
        size_t numElements;
        int distanceFromStart;
    };


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
      circularIter = circularIterator<T> (buffer.begin(),buffer.end());
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
      
    circularIterator<T> getIterator()
    {
      return circularIter;
    }   

    circularIterator<T> circularIter;
   private:
    std::vector<T>  buffer;
    signed long int bufferLength;
    size_t          writeHead;
    size_t          readHead;
  };
}
/* circularBuffer_hpp */
