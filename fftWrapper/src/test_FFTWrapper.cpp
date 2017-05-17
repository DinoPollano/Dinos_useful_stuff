//
//  test_laFFTWrapper.cpp
//
//  Created by Christofero Pollano on 19/01/2017.
//
//

#include <stdio.h>
#include "catch.hh"
#include "FFTWrapper.h"
#include <iostream>
#include <vector>
#include <numeric>
using dino::FFTWrapper;
using dino::FFTWrapperStatus;

TEST_CASE ("FFT Wrapper - Magnitude", "[FFT]")
{
	const size_t NFFT        = 128;
	const size_t kSignalSize = NFFT;

	SECTION ("Normal behaviour")
	{
		FFTWrapper<double> fft;

		// Test signal
		std::vector<double> buffer (kSignalSize, 1.);
		std::vector<double> Xf (NFFT / 2, 0.);

		fft.prepFFT (NFFT);
		CHECK (fft.getStatus (FFTWrapperStatus::GetSizeStatus) == ippStsNoErr);

		fft.performHanningWindow (buffer.data (), kSignalSize);
		fft.calculateMagnitude (buffer.data (), Xf.data ());

		CHECK (fft.getStatus (FFTWrapperStatus::GetSizeStatus) == ippStsNoErr);
		CHECK (fft.getStatus (FFTWrapperStatus::FFTinitStatus) == ippStsNoErr);
	}
	SECTION ("Limits - Giving vectors that are too long")
	{  // essentially the IPP will just truncate
		FFTWrapper<double> fft;

		// Test signal
		std::vector<double> buffer (kSignalSize, 1.);
		std::vector<double> Xf (NFFT / 2, 0.);

		fft.prepFFT (NFFT);
		std::vector<double> xbig (kSignalSize * 2, 1.);

		fft.calculateMagnitude (xbig.data (), Xf.data ());
		CHECK (fft.getStatus (FFTWrapperStatus::FFTStatus) == ippStsNoErr);
		CHECK (fft.getStatus (FFTWrapperStatus::MagnitudeStatus) == ippStsNoErr);
	}

	SECTION ("given ones - first bin should be greatest and should equal 1 ")
	{
		FFTWrapper<double> fft;

		// Test signal
		std::vector<double> buffer (kSignalSize, 1.);
		std::vector<double> Xf (NFFT / 2, 0.);
		fft.prepFFT (NFFT);
		fft.calculateMagnitude (buffer.data (), Xf.data ());
		std::vector<double>::iterator bin =
		    std::max_element (Xf.begin (), Xf.end ());
		CHECK (bin == Xf.begin ());
		CHECK (*bin == 1.);
	}
	SECTION (
	    "given ones  and windowed - first bin should be greatest and should "
	    "equal 0.5 ")
	{
		FFTWrapper<double> fft;

		// Test signal
		std::vector<double> buffer (kSignalSize, 1.);
		std::vector<double> Xf (NFFT / 2, 0.);
		fft.prepFFT (NFFT);
		fft.performHanningWindow (buffer.data (), NFFT);
		fft.calculateMagnitude (buffer.data (), Xf.data ());
		std::vector<double>::iterator bin =
		    std::max_element (Xf.begin (), Xf.end ());
		CHECK (bin == Xf.begin ());
		CHECK (*bin == Approx (0.5).epsilon (0.1));
	}
}

TEST_CASE ("FFT Wrapper - Sine Wave", "[sines] [FFT]")
{
	const size_t NFFT = 512;

	FFTWrapper<double> fft;

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

	std::transform (xn.begin (), xn.end (), xn.begin (),
	                [&phase, incr](double x) -> double {
		                phase += incr;
		                if (phase > 2.0 * M_PI)
		                {
			                phase -= 2.0 * M_PI;
		                }
		                return sin (phase);
		              });

	fft.calculateMagnitude (xn.data (), XF.data ());

	std::vector<double>::iterator bin = std::max_element (XF.begin (), XF.end ());
	size_t                        k   = std::distance (XF.begin (), bin);
	double                        f   = (k * Fs) / 512;

	CHECK (f == Approx (fc).epsilon (0.1));
}

TEST_CASE ("FFT Wrapper - FFT", "[FFT]")
{
	SECTION (" double Version")
	{
		FFTWrapper<double> fft;

		const size_t NFFT        = 128;
		const size_t kSignalSize = NFFT;

		// Test signal
		std::vector<double> buffer (kSignalSize, 1.);
		std::vector<double> Xfre (NFFT, 0.);
		std::vector<double> Xfim (NFFT, 0.);
		fft.prepFFT (NFFT);
		//  fft.performHanningWindow(buffer.data(), NFFT);
		fft.calculateFFT (buffer.data (), Xfre.data (), Xfim.data ());
		std::vector<double>::iterator bin =
		    std::max_element (Xfre.begin (), Xfre.end ());
		CHECK (bin == Xfre.begin ());
		CHECK (*bin == 1.);
	}
	SECTION (" float Version")
	{
		FFTWrapper<float> fft;

		const size_t NFFT        = 512;
		const size_t kSignalSize = NFFT;

		// Test signal
		std::vector<float> buffer (kSignalSize, 1.);
		std::iota (buffer.begin (), buffer.end (), 1.);
		std::vector<float> Xfre (NFFT, 0.);
		std::vector<float> Xfim (NFFT, 0.);
		fft.prepFFT (NFFT);
		//      fft.performHanningWindow(buffer.data(), NFFT);
		fft.calculateFFT (buffer.data (), Xfre.data (), Xfim.data ());

		//    CHECK ( Xfim[1]==*Xfim.end());
		//    CHECK(*bin ==  1.);
	}
}

TEST_CASE ("FFT Wrapper - Linearity", "[FFT]")
{
	const size_t NFFT = 512;

	FFTWrapper<double> fft;

	size_t blockSize = 64;
	// Test signal
	std::vector<double> xn1 (NFFT, 0.);
  std::vector<double> xn2 (NFFT, 0.);
	std::vector<double> XF (NFFT / 2, 0.);

	fft.prepFFT (NFFT);

	int    count      = 0;
	bool   testing    = true;
	size_t numCycles  = (512 / 64) * 2;
	double fc1        = 440;
	double fc2        = 756;
	double Fs         = 44.1e3;
	double phase      = 0.;
	double T          = 1.0 / Fs;
	double incr       = 2 * M_PI * fc1 * T;
	auto sinGenerator = [&phase, incr](double x) -> double {
		phase += incr;
		if (phase > 2.0 * M_PI)
		{
			phase -= 2.0 * M_PI;
		}
		return sin (phase);
	};

	std::transform (xn1.begin (), xn1.end (), xn1.begin (), sinGenerator);
	incr = 2 * M_PI * fc2 * T;
  std::transform (xn2.begin (), xn2.end (), xn2.begin (), sinGenerator);

	fft.calculateMagnitude (xn.data (), XF.data ());
}
