//
//  FilterMaker.h
//  FilterMaker
//
//  Created by Christofero Pollano on 25/11/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#ifndef FilterMaker_h
#define FilterMaker_h
#include "circularBuffer.hpp"

namespace dino
{
  template <class T>
  struct Coefficient
  {
    T A = 0;
    T B = 0;
  };
  template <class T>
  class Filter
  {
    enum Types
    {
      LPF,
      HPF
    };
    public:
      Filter(){static_assert (std::is_floating_point <T>(), "filter type must be float or double");}
      void init (int type, int order, T initialCo, int Fs)
      {
        
      }
    private:
      T cutOFf;
      circularBuffer<T> buff;
      std::vector <Coefficient<T>> coeffs;
  };
} //dino

#endif /* FilterMaker_h */
