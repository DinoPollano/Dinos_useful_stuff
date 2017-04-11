//
//  different_types.cpp
//  circBuffer
//
//  Created by Christofero Pollano on 11/04/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#include <stdio.h>
#include <stdio.h>
#include "catch.hh"
#include "circularBuffer.hpp"
#include <vector>
#include <memory>
#include <numeric>

TEST_CASE("circBuffer of Vectors")
{
  using dino::circularBuffer;
  int buffLength = 10;
  circularBuffer<std::vector<float>> circBuffer;
  
  circBuffer.init(3, std::vector<float, std::allocator<float> > (10));
  std::vector<float> buff1(buffLength,0.);
  std::vector<float> buff2(buffLength,0.);
  std::iota(buff1.begin(), buff1.end(), 1);
  std::iota(buff2.begin(), buff2.end(), 10);
  circBuffer.insertOne(buff1);
 std::vector<float> t =  circBuffer.getOne(1);
  CHECK(t == buff1);
  
}
