//
//  test_laFFTWrapper.cpp
//
//  Created by Christofero Pollano on 19/01/2017.
//
//

#include <stdio.h>
#include "catch.hh"
#include "LAFFTWrapper.h"
#include <iostream>
#include <vector>
#include <numeric>

TEST_CASE ("FFT Wrapper")
{
	const size_t NFFT        = 128;
	const size_t kSignalSize = NFFT;

	LAFFTWrapper64 fft;

	// Test signal
	std::vector<double> buffer (kSignalSize, 1.);
	std::vector<double> Xf (NFFT / 2, 0.);

	fft.prepFFT (NFFT);

	SECTION ("Normal behaviour")
	{
		CHECK (fft.getStatus (LAFFTWrapper::GetSizeStatus) == ippStsNoErr);

		fft.performHanningWindow (buffer.data(), kSignalSize);
		fft.calculateMagnitude (buffer.data (), Xf.data ());

		CHECK (fft.getStatus (LAFFTWrapper::GetSizeStatus) == ippStsNoErr);
		CHECK (fft.getStatus (LAFFTWrapper::FFTinitStatus) == ippStsNoErr);
	}

	SECTION ("Limits - Giving vectors that are too long")
	{  // essentially the IPP will just truncate
		std::vector<double> xbig (kSignalSize * 2, 1.);

		fft.calculateMagnitude (xbig.data (), Xf.data ());
		CHECK (fft.getStatus (LAFFTWrapper::FFTStatus) == ippStsNoErr);
		CHECK (fft.getStatus (LAFFTWrapper::MagnitudeStatus) ==
		       ippStsNoErr);
	}
}

TEST_CASE ("FFT Wrapper - Sine Wave", "[sines]")
{
	const size_t NFFT = 512;

	LAFFTWrapper64 fft;

	size_t blockSize = 64;
	// Test signal
	std::vector<double> xn (NFFT, 0.);
	std::vector<double> XF (NFFT / 2, 0.);

	fft.prepFFT (NFFT);

	int    count     = 0;
	bool   testing   = true;
	size_t numCycles = (512 / 64) * 2;
	double fc        = 440;
	double Fs        = 44.1e3;
	double phase     = 0.;
	double T         = 1.0 / Fs;
	double incr      = 2 * M_PI * fc * T;

	for (int i = 0; i < NFFT; i++)
	{
		phase += incr;
		if (phase > 2.0 * M_PI)
		{
			phase -= 2.0 * M_PI;
		}
		xn[i] = sin (phase);
	}

	fft.calculateMagnitude (xn.data (), XF.data ());

	std::vector<double>::iterator bin = std::max_element (XF.begin (), XF.end ());
	size_t                        k   = std::distance (XF.begin (), bin);
	double                        f   = (k * Fs) / 512;

	CHECK (f == Approx (fc).epsilon (0.1));
	testing = (++count <= numCycles);

}  // Sine wave

