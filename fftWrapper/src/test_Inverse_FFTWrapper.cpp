//
//  test_Inverse_FFTWrapper.cpp
//  FFTWrapper
//
//  Created by Christofero Pollano on 17/05/2017.
//  Copyright © 2017 dino. All rights reserved.
//

#include <stdio.h>
#include "catch.hh"
#include "IFFTWrapper.h"
#include <iostream>
#include <vector>
#include <numeric>
using dino::IFFTWrapper;
using dino::IFFTWrapperStatus;

TEST_CASE("IFFT Wrapper - IFFT","[IFFT]")
{
  SECTION(" double Version")
  {
    IFFTWrapper<double> ifft;
    
    const size_t NFFT        = 128;
    const size_t kSignalSize = NFFT;
    
    
    // Test signal
    std::vector<double> bufferRe (kSignalSize, 1.);
    std::vector<double> bufferIm (kSignalSize, 0.);
    std::vector<double> Xfre (NFFT, 0.);
    std::vector<double> Xfim (NFFT, 0.);
    ifft.prepFFT (NFFT);
    //  fft.performHanningWindow(buffer.data(), NFFT);
    ifft.calculateIFFT(bufferRe.data(),bufferIm.data(), Xfre.data(), Xfim.data());
    
    bool test = false;
  }
  
}
