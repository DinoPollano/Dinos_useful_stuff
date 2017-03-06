//
//  unwrapping.cpp
//  circBuffer
//
//  Created by Christofero Pollano on 05/03/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#include <stdio.h>
#include "catch.hh"
#include "circularBuffer.hpp"
#include <vector>
#include <memory>
#include <algorithm>


TEST_CASE("Unwrapping")
{
  using dino::circularBuffer;
  size_t bufferLength = 64;
  circularBuffer<float> circBuffer;
  circBuffer.init(bufferLength, 0.);
  
  std::unique_ptr<float[]> initial(new float[31]);
  circBuffer.insertMany(&initial[0], 31);
  std::unique_ptr<float[]> input(new float[bufferLength]);
  
  for(int i = 0; i < bufferLength; i++)
  {
    input[i] = static_cast<float>(i);
  }
  
  circBuffer.insertMany(&input[0], bufferLength);
  
  std::vector<float>  output = circBuffer.getUnwrapped();
  SECTION("checking beginning and end")
  {
    CHECK(output[0] == input[0]);
    CHECK(output[bufferLength-1] == input[bufferLength]);
  }
  
                                 
  
}
