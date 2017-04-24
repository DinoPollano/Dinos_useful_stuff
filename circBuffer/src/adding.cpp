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
    size_t buffsize = 512;
    circularBuffer<float> circBuffer;
    circBuffer.init(buffsize, 0.);
    size_t tick = 20;
    float  output;
    circBuffer.insertOne(1.);
    for( int i = 20;  ++i < buffsize;)
    {
      if(i % tick == 0)
      {
        output = circBuffer.getOne(tick);
        CHECK(output == 1.0);
      }
      (i % tick == 0) ? circBuffer.insertOne(1.) :circBuffer.insertOne(0.);
    }
  }
  
  
  SECTION("adding blocks")
  {
    using dino::circularBuffer;
    size_t buffsize = 128;
    circularBuffer<float> circBuffer;
    circBuffer.init(buffsize, 0.);
    float  output;
    size_t blockSize = 8;
    std::unique_ptr<float[]> input(new float[blockSize]);
    std::iota(input.get(), input.get()+8, 1);
    for( int i = 0; i <  32; i++)
    {
      circBuffer.insertMany( &input[0], blockSize);
      for (size_t k = 1; k <= blockSize; k++) {
        float output = circBuffer.getOne(k);
        CHECK(output == (9-k));
      }
    }
  }
  
  
}
