//
//  circularBuffer.hpp
//  circBuffer
//
//  Created by Christofero Pollano on 26/02/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#ifndef circularBuffer_hpp
#define circularBuffer_hpp

#include <stdio.h>
#include <vector>
#include <type_traits>
#include <assert.h>
#include <algorithm>
#include <type_traits>
namespace dino {
  template<class T>
  class circularBuffer
  {
  public:
    circularBuffer():writeHead(0),readHead(0),bufferLength(0){
      static_assert(std::is_floating_point<T>(), "must be float (or double)");
    };
    
    ~circularBuffer(){};
    
    void init(size_t length, T initialValue)
    {
      bufferLength = length;
      buffer.resize(bufferLength, initialValue);
    }
    
    inline void insertOne(const T val)
    {
      buffer[writeHead] = val;
      if(++writeHead >= bufferLength){ writeHead = 0;}
    }
    
    inline void insertMany(const T* values, size_t length)
    {
      for(int i = 0; i < length; i++)
      {
        buffer[writeHead] = values[i];
        if(++writeHead >= bufferLength){ writeHead = 0;}
      }
    }
    
   inline T getOne(size_t nFrom)
    {
      assert(nFrom < bufferLength);
      readHead = ((writeHead - nFrom) % bufferLength + bufferLength)%bufferLength;
      T output = buffer[readHead];
      return output;
    }
   
   inline std::vector<T> getMany(size_t nFrom)
    {
      assert(nFrom < bufferLength);
      std::vector<T> output(nFrom,NULL);
      readHead = writeHead;
      for (size_t i = 0; i <  nFrom; i++)
      {
        output[i] = buffer[readHead];
        if(++writeHead >= bufferLength){writeHead = 0;}
      }
      return output;
    }
    
   inline std::vector<T> getUnwrapped()
    {
      std::vector<T> unwrapped;
      typename std::vector<T>::iterator oldestElement = buffer.begin() + writeHead;
      typename std::vector<T>::iterator remaindingHalf =  buffer.begin();
      unwrapped.insert(unwrapped.begin(),oldestElement,buffer.end());
      unwrapped.insert(unwrapped.end(),buffer.begin(),buffer.begin()+(writeHead));

      return unwrapped;
    }

    size_t getLength() { return bufferLength; };
    
    void flush()
    {
      if (bufferLength > 0)
      {
        std::fill(buffer.begin(), buffer.end(), 0.);
      }
    }

    
  private:
    std::vector<T> buffer;
    size_t bufferLength;
    size_t writeHead;
    size_t readHead;
  };
}
//#include "circularBuffer.cpp"
#endif /* circularBuffer_hpp */
