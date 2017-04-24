//
//  adding.cpp
//  circBuffer
//
//  Created by Christofero Pollano on 28/02/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#include <stdio.h>
#include "catch.hh"
#include "circularBuffer.hpp"
#include <vector>
#include <memory>
#include  <numeric>


TEST_CASE("adding and retrieval")
{
  
  
  SECTION("adding individually")
  {
    using dino::circularBuffer;
    size_t buffsize = 100;
    circularBuffer<float> circBuffer;
    circBuffer.init(buffsize, 0.);
    for (int i =0; i < std::round(buffsize/2); i++) {
      circBuffer.insertOne(0.f);
    }
    for( int i = 0;  ++i <= buffsize;)
    {
      circBuffer.insertOne(static_cast<float>(i));
    }
    for (size_t k = buffsize; k >= 1; k--) {
      float out = circBuffer.getOne(k);
      float rev = (buffsize + 1) - k;
      CHECK(rev == out);
    }
  }
  
  
  SECTION("adding blocks")
  {
    using dino::circularBuffer;
    size_t buffsize = 128;
    circularBuffer<float> circBuffer;
    circBuffer.init(buffsize, 0.);
    float  output;
    size_t blockSize = 9;
    std::unique_ptr<float[]> input(new float[blockSize]);
    std::iota(input.get(), input.get()+blockSize, 1);
    for( int i = 0; i <  64; i++)
    {
      circBuffer.insertMany( &input[0], blockSize);
      for (size_t k = 1; k <= blockSize; k++) {
        float output = circBuffer.getOne(k);
        float rev = (blockSize + 1) - k;
        INFO(" should be : " << rev);
        CHECK(output == rev);
      }
    }
  }
  
  
}
