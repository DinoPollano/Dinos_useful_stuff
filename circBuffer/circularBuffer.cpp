//
//  circularBuffer.cpp
//  circBuffer
//
//  Created by Christofero Pollano on 26/02/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#include "circularBuffer.hpp"

template <class T> void dino::circularBuffer<T>::init(size_t length, T initialValue)
{
  bufferLength = length;
  buffer.resize(bufferLength, initialValue);
}

template <class T> void dino::circularBuffer<T>::insertOne(T val)
{
  buffer[writeHead] = val;
  writeHead++;
  if(writeHead >= bufferLength)
  {
    writeHead = 0;
  }
}


template <class T> void dino::circularBuffer<T>::insertMany(T *values, size_t length)
{
  for(int i = 0; i < length; i++)
  {
    buffer[writeHead] = values[i];
    if(++writeHead >= bufferLength)
    {
      writeHead = 0;
    }
  }
}


template <class T> T dino::circularBuffer<T>::getOne(size_t nFrom)
{
  assert(nFrom < bufferLength);
  readHead = ((writeHead - nFrom) % bufferLength + bufferLength)%bufferLength;
  T output = buffer[readHead];
  return output;
}

template <class T> std::vector<T> dino::circularBuffer<T>::getMany(size_t nFrom)
{
  assert(nFrom < bufferLength);
  std::vector<T> output(nFrom,NULL);
  readHead = writeHead;
  for (size_t i = 0; i <  nFrom; i++)
  {
    ++writeHead;
    if(writeHead >= bufferLength){writeHead = 0;}
    output[i] = buffer[readHead];
  }
  return output;
}

template <class T> void dino::circularBuffer<T>::flush()
{
  if (bufferLength > 0)
  {
    std::fill(buffer.begin(), buffer.end(), 0.);
  }
}

template<class T> std::vector<T> dino::circularBuffer<T>::getUnwrapped()
{
  std::vector<T> unwrapped(bufferLength,0.);
  
  typename std::vector<T>::iterator oldestElement = buffer.begin() + writeHead + 1;
  typename std::vector<T>::iterator remaindingHalf =  buffer.begin();
  
  std::copy(oldestElement, buffer.end(), unwrapped.begin());
  std::copy(remaindingHalf, remaindingHalf + writeHead, unwrapped.begin() + (buffer.end()-oldestElement));
  return unwrapped;
}
