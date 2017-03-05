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
namespace dino {
  class circularBuffer
  {
  public:
    circularBuffer():writeHead(0),readHead(0),bufferLength(0){};
    ~circularBuffer(){};
    
    void init(size_t length, float initialValue);
    void insertOne(float val);
    void insertMany(float* values, size_t length);
    float getOne(size_t nFrom);
    void flush();
    std::vector<float> getMany(size_t nFrom);
    std::vector<float> getUnwrapped();
    size_t getLength() { return bufferLength; };
    
    
  private:
    std::vector<float> buffer;
    size_t bufferLength;
    size_t writeHead;
    size_t readHead;
  };
}
#endif /* circularBuffer_hpp */
