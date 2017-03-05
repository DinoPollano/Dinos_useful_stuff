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

TEST_CASE("Unwrapping")
{
  using dino::circularBuffer;
  size_t bufferLength = 64;
  circularBuffer circBuffer;
  circBuffer.init(bufferLength, 0.);
  
  std::unique_ptr<float[]> initial(new float[32]);
  
  
  circBuffer.insertMany(*initial, 32);
}
