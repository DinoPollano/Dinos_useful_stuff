//
//  circularBuffer.cpp
//  circBuffer
//
//  Created by Christofero Pollano on 26/02/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#include "circularBuffer.hpp"

void dino::circularBuffer::init(size_t length, float initialValue)
{
  bufferLength = length;
  buffer.resize(bufferLength, initialValue);
}

void dino::circularBuffer::insertOne(float val)
{
  buffer[writeHead] = val;
  writeHead++;
  if(writeHead >= bufferLength)
  {
    writeHead = 0;
  }
}


void dino::circularBuffer::insertMany(float *values, size_t length)
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


float dino::circularBuffer::getOne(size_t nFrom)
{
  assert(nFrom < bufferLength);
  readHead = ((writeHead - nFrom) % bufferLength + bufferLength)%bufferLength;
  float output = buffer[readHead];
  return output;
}

std::vector<float> dino::circularBuffer::getMany(size_t nFrom)
{
  assert(nFrom < bufferLength);
  std::vector<float> output(nFrom,NULL);
  readHead = writeHead;
  for (size_t i = 0; i <  nFrom; i++)
  {
    ++writeHead;
    if(writeHead >= bufferLength){writeHead = 0;}
    output[i] = buffer[readHead];
  }
  return output;
}

void dino::circularBuffer::flush()
{
  if (bufferLength > 0)
  {
    std::fill(buffer.begin(), buffer.end(), 0.);
  }
}

std::vector<float> dino::circularBuffer::getUnwrapped()
{
  std::vector<float> unwrapped(bufferLength,0.);
  
  std::vector<float>::iterator oldestElement = buffer.begin() + writeHead + 1;
  std::vector<float>::iterator remaindingHalf =  buffer.begin();
  
  std::copy(oldestElement, buffer.end(), unwrapped.begin());
  std::copy(remaindingHalf, remaindingHalf + writeHead, unwrapped.begin() + (buffer.end()-oldestElement));
  return unwrapped;
}
