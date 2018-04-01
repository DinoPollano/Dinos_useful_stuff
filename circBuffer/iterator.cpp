//
//  iterator.cpp
//  circBuffer
//
//  Created by Christofero Pollano on 31/03/2018.
//  Copyright © 2018 dino. All rights reserved.
//

#include <stdio.h>
#include "catch.hh"
#include "circularBuffer.hpp"

TEST_CASE("Circular Buffer Iterator")
{
  dino::circularBuffer<int> circular;
  circular.init (64,0.f);
  auto iter = circular.getIterator();
  for (int i = 0; i < 3; ++i)
  {
    iter.insert (i);
  }
  for (int i =  3; i < (128-3); ++i)
  {
    iter.insert (i);
    CHECK (iter[3] == (i-3));
  }
  circular.flush();
}
