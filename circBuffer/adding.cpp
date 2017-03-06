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


TEST_CASE("adding and retrieval")
{
  using dino::circularBuffer;
  size_t buffsize = 512;
  circularBuffer<float> circBuffer;
  circBuffer.init(buffsize, 0.);
  size_t tick = 20;
  float  output;
  
  SECTION("adding individually")
  {
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
  circBuffer.flush();
  
  SECTION("adding blocks")
  {
    size_t blockSize = 8;
    std::unique_ptr<float[]> input(new float[blockSize]);
    for(int i = 0; ++i<blockSize;)
    {
      input[i] = 0.;
    }
    input[0] = 1.;
    for( int i = 0; i <  1024; i++)
    {
      circBuffer.insertMany( &input[0], blockSize);
      if(i % tick == 0)
      {
        output = circBuffer.getOne(blockSize);
        CHECK(output == 1.0);
      }
    }
  }
  circBuffer.flush();
  
}
