//
//  test_Inverse_FFTWrapper.cpp
//  FFTWrapper
//
//  Created by Christofero Pollano on 17/05/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#include <stdio.h>
#include "catch.hh"
#include "FFTWrapper.h"
#include <iostream>
#include <vector>
#include <numeric>
using dino::FFTWrapper;

TEST_CASE("IFFT Wrapper - IFFT","[IFFT]")
{
  SECTION(" double Version")
  {
    FFTWrapper<double>  fft;
    
    const size_t NFFT        = 128;
    const size_t kSignalSize = NFFT;
    
    // Test signal
    std::vector<double> xT (NFFT,0.);
    std::vector<double> Xfre (NFFT, 0.);
    std::vector<double> Xfim (NFFT, 0.);
    std::vector<double> reconXre (NFFT,0.);
    std::vector<double> reconXim (NFFT,0.);
    xT[4] = 1.0;
    
    fft.prepFFT (NFFT);
    
    
    fft.calculateFFT   (xT.data(), Xfre.data(), Xfim.data());
    fft.calculateIFFT (Xfre.data(),Xfim.data(), reconXre.data(), reconXim.data());
    
    Approx (reconXre == xT);
    
    
  }
  
}
