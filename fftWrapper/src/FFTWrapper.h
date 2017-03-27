#pragma once

//
//
//
//
//
/*

        wrapper of the Intel Primitives Protocol FFT functions

 */

#include <stdio.h>
#include "ipp.h"
#include <vector>
#include "math.h"

#pragma region Common Code
namespace dino
{
struct FFTWrapper
{
	enum Status
	{
		GetSizeStatus,         //
		FFTinitStatus,         //
		FFTStatus,             //
		FormartConvertStatus,  //
		MagnitudeStatus,       //
		totalNumStatus
	};
};
#pragma endregion

#pragma mark - 32-bit implementation
class FFTWrapper32
{
 public:
	FFTWrapper32 ();
	~FFTWrapper32 ();

	void prepFFT (int fftLength);

	IppStatus getStatus (size_t StatusIndex) { return m_status[StatusIndex]; }
	// this method assumes source is a real signal, it should also be the same
	// length as the FFT size
	void calculateMagnitude (const float* source, float* magnitude);

	// in-line hanning function
	void performHanningWindow (float* source, int lengthInsamples);

 private:
	IppStatus ipps_interleaved_to_complex (const float* src, Ipp32fc* dst,
	                                       int length);
	IppStatus ipps_magnitude_from_complex (const Ipp32fc* src, float* dst,
	                                       int length);
	IppStatus ippFFTinit (IppsFFTSpec_R_32f** FFTSpec, int order, int flag,
	                      IppHintAlgorithm hint, Ipp8u* pSpec,
	                      Ipp8u* pSpecBuffer);
	IppStatus ipps_getFFTsize (int order, int flag, IppHintAlgorithm hint,
	                           int* SpecSize, int* specBufferSize,
	                           int* bufferSize);
	IppStatus ippsFFT_RT_to_interleaved (const float* SRC, float* DST,
	                                     IppsFFTSpec_R_32f* spec, Ipp8u* buff);

	size_t   m_fftSize;
	Ipp8u *  m_pSpecMem = nullptr, *m_pMemInit = nullptr, *m_pBuffer = nullptr;
	float*   m_interleavedFreqData = nullptr;
	Ipp32fc* m_cmplxFreqData       = nullptr;
	IppsFFTSpec_R_32f* m_FFTSpec   = nullptr;
	IppStatus          m_status[FFTWrapper::totalNumStatus];
};

#pragma mark - 64-bit implementation

class FFTWrapper64
{
 public:
	FFTWrapper64 ();
	~FFTWrapper64 ();

	void prepFFT (int fftLength);

	IppStatus getStatus (size_t StatusIndex) { return m_status[StatusIndex]; }
	// this method assumes source is a real signal, it should also be the same
	// length as the FFT size
	void calculateMagnitude (const double* source, double* magnitude);

	// in-line hanning function
	void performHanningWindow (double* source, int lengthInsamples);

 private:
	IppStatus ippFFTinit (IppsFFTSpec_R_64f** FFTSpec, int order, int flag,
	                      IppHintAlgorithm hint, Ipp8u* pSpec,
	                      Ipp8u* pSpecBuffer);
	IppStatus ipps_getFFTsize (int order, int flag, IppHintAlgorithm hint,
	                           int* SpecSize, int* specBufferSize,
	                           int* bufferSize);
	IppStatus ippsFFT_RT_to_interleaved (const double* SRC, double* DST,
	                                     IppsFFTSpec_R_64f* spec, Ipp8u* buff);
	IppStatus ipps_interleaved_to_complex (const double* src, Ipp64fc* dst,
	                                       int length);
	IppStatus ipps_magnitude_from_complex (const Ipp64fc* src, double* dst,
	                                       int length);

	size_t   m_fftSize;
	Ipp8u *  m_pSpecMem = nullptr, *m_pMemInit = nullptr, *m_pBuffer = nullptr;
	double*  m_interleavedFreqData = nullptr;
	Ipp64fc* m_cmplxFreqData       = nullptr;
	IppsFFTSpec_R_64f* m_FFTSpec   = nullptr;
	IppStatus          m_status[FFTWrapper::totalNumStatus];
};
}  // namespace dino
