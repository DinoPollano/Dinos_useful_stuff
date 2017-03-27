//
//  laFFTMachine.cpp
//  laFFT~
//
//  Created by Christofero Pollano on 19/01/2017.
//
//

#include "LAFFTWrapper.h"

// ==============   32-bit (float)   ===================

LAFFTWrapper32::LAFFTWrapper32() {};

LAFFTWrapper32::~LAFFTWrapper32()
{
  if (m_pMemInit)
  {
    ippFree(m_pMemInit);
  }
  if (m_FFTSpec)
  {
    ippFree(m_FFTSpec);
  }
  if (m_pBuffer)
  {
    ippFree(m_pBuffer);
  }
  if (m_interleavedFreqData)
  {
    delete[] m_interleavedFreqData;
  }
  if (m_cmplxFreqData)
  {
    delete[] m_cmplxFreqData;
  }
}

IppStatus LAFFTWrapper32::ippsFFT_RT_to_interleaved(const float* SRC,
  float*       DST,
  IppsFFTSpec_R_32f* spec,
  Ipp8u*       buff)
{
  IppStatus stat = ippsFFTFwd_RToCCS_32f(SRC, DST, spec, buff);
  return stat;
}


void LAFFTWrapper32::performHanningWindow(float* source, int    lengthInSamples)
{
  ippsWinHann_32f_I(source, lengthInSamples);  // uses IPP's windowing function
}

IppStatus LAFFTWrapper32::ipps_interleaved_to_complex(
  const float* src, Ipp32fc* dst, int length)
{
  IppStatus stat = ippsConjCcs_32fc(src, dst, length);
  return stat;
}
IppStatus LAFFTWrapper32::ipps_magnitude_from_complex(
  const Ipp32fc* src, float* dst, int length)
{
  IppStatus stat = ippsMagnitude_32fc(src, dst, length);
  return stat;
}

IppStatus LAFFTWrapper32::ippFFTinit(IppsFFTSpec_R_32f** FFTSpec,
  int order, int flag,
  IppHintAlgorithm hint, Ipp8u* pSpec,
  Ipp8u* pSpecBuffer)
{
  IppStatus stat =
    ippsFFTInit_R_32f(FFTSpec, order, flag, hint, pSpec, pSpecBuffer);
  return stat;
}

IppStatus LAFFTWrapper32::ipps_getFFTsize(int order, int flag, IppHintAlgorithm hint, int * SpecSize, int * specBufferSize, int * bufferSize)
{
  IppStatus stat = ippsFFTGetSize_R_32f(order, flag, hint, SpecSize,
    specBufferSize, bufferSize);
  return stat;
}


void LAFFTWrapper32::prepFFT(int fftLength)
{
  if (fftLength != m_fftSize)  // if it's the first call
  {
    m_fftSize = fftLength;
    // prep ipp
    unsigned int fftOrder = static_cast<unsigned int> (ceil(log2(m_fftSize)));
    int          sizeSpec = 0, sizeInit = 0, sizeBuf = 0;
    m_status[LAFFTWrapper::GetSizeStatus] =
      ipps_getFFTsize(fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone,
        &sizeSpec, &sizeInit, &sizeBuf);

    if (!m_pSpecMem)
    {
      m_pSpecMem = (Ipp8u*)ippMalloc(sizeSpec);
    }
    if (!m_pBuffer)
    {
      m_pBuffer = (Ipp8u*)ippMalloc(sizeBuf);
    }
    if (!m_pMemInit)
    {
      m_pMemInit = (Ipp8u*)ippMalloc(sizeInit);
    }
    m_status[LAFFTWrapper::FFTinitStatus] =
      ippFFTinit(&m_FFTSpec, fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone,
        m_pSpecMem, m_pMemInit);  // initialise FFT
    m_interleavedFreqData = new float[m_fftSize + 2];
    m_cmplxFreqData = new Ipp32fc[m_fftSize];
  }
};


void LAFFTWrapper32::calculateMagnitude(const float* source, float* magnitude)
{
  // perform fft, this will create an interleaved pointer to pointer of
  // complex data
  m_status[LAFFTWrapper::FFTStatus] = ippsFFT_RT_to_interleaved(source, m_interleavedFreqData,
    m_FFTSpec, m_pBuffer);

  // move the interleaved data into a complex pointer to pointer
  m_status[LAFFTWrapper::FormartConvertStatus] = ipps_interleaved_to_complex(
    m_interleavedFreqData, m_cmplxFreqData, static_cast<int> (m_fftSize));

  // calculates magnitude (needs the frequency data in a complex pointer to
  // pointer)
  m_status[LAFFTWrapper::MagnitudeStatus] = ipps_magnitude_from_complex(
    m_cmplxFreqData, magnitude, static_cast<int> ((m_fftSize / 2)));
}


// ==============   64-bit (double)   ===================

LAFFTWrapper64::LAFFTWrapper64()
{
}

LAFFTWrapper64::~LAFFTWrapper64()
{
  if (m_pMemInit)
  {
    ippFree(m_pMemInit);
  }
  if (m_FFTSpec)
  {
    ippFree(m_FFTSpec);
  }
  if (m_pBuffer)
  {
    ippFree(m_pBuffer);
  }
  if (m_interleavedFreqData)
  {
    delete[] m_interleavedFreqData;
  }
  if (m_cmplxFreqData)
  {
    delete[] m_cmplxFreqData;
  }
}

void LAFFTWrapper64::prepFFT(int fftLength)
{
  if (fftLength != m_fftSize)  // if it's the first call
  {
    m_fftSize = fftLength;
    // prep ipp
    unsigned int fftOrder = static_cast<unsigned int> (ceil(log2(m_fftSize)));
    int          sizeSpec = 0, sizeInit = 0, sizeBuf = 0;
    m_status[LAFFTWrapper::GetSizeStatus] =
      ipps_getFFTsize(fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone,
        &sizeSpec, &sizeInit, &sizeBuf);

    if (!m_pSpecMem)
    {
      m_pSpecMem = (Ipp8u*)ippMalloc(sizeSpec);
    }
    if (!m_pBuffer)
    {
      m_pBuffer = (Ipp8u*)ippMalloc(sizeBuf);
    }
    if (!m_pMemInit)
    {
      m_pMemInit = (Ipp8u*)ippMalloc(sizeInit);
    }
    m_status[LAFFTWrapper::FFTinitStatus] =
      ippFFTinit(&m_FFTSpec, fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone,
        m_pSpecMem, m_pMemInit);  // initialise FFT
    m_interleavedFreqData = new double[m_fftSize + 2];
    m_cmplxFreqData = new Ipp64fc[m_fftSize];
  }
}

void LAFFTWrapper64::performHanningWindow(double* source, int lengthInSamples)
{
  ippsWinHann_64f_I(source, lengthInSamples);  // uses IPP's windowing function
}

IppStatus LAFFTWrapper64::ippsFFT_RT_to_interleaved(const double* SRC, double* DST, IppsFFTSpec_R_64f* spec, Ipp8u* buff)
{
  IppStatus stat = ippsFFTFwd_RToCCS_64f(SRC, DST, spec, buff);
  return stat;
}

IppStatus LAFFTWrapper64::ipps_interleaved_to_complex(const double* src, Ipp64fc* dst, int length)
{
  IppStatus stat = ippsConjCcs_64fc(src, dst, length);
  return stat;
}

IppStatus LAFFTWrapper64::ipps_magnitude_from_complex(const Ipp64fc* src, double* dst, int length)
{
  IppStatus stat = ippsMagnitude_64fc(src, dst, length);
  return stat;
}

void LAFFTWrapper64::calculateMagnitude(const double * source, double * magnitude)
{
  // perform fft, this will create an interleaved pointer to pointer of
  // complex data
  m_status[LAFFTWrapper::FFTStatus] = ippsFFT_RT_to_interleaved(source, m_interleavedFreqData, m_FFTSpec, m_pBuffer);

  // move the interleaved data into a complex pointer to pointer
  m_status[LAFFTWrapper::FormartConvertStatus] = ipps_interleaved_to_complex(m_interleavedFreqData, m_cmplxFreqData, static_cast<int> (m_fftSize));

  // calculates magnitude (needs the frequency data in a complex pointer to
  // pointer)
  m_status[LAFFTWrapper::MagnitudeStatus] = ipps_magnitude_from_complex(
    m_cmplxFreqData, magnitude, static_cast<int> ((m_fftSize / 2)));
}

IppStatus LAFFTWrapper64::ippFFTinit(IppsFFTSpec_R_64f** FFTSpec, int order,
  int flag, IppHintAlgorithm hint,
  Ipp8u* pSpec, Ipp8u* pSpecBuffer)
{
  IppStatus stat =
    ippsFFTInit_R_64f(FFTSpec, order, flag, hint, pSpec, pSpecBuffer);
  return stat;
}

IppStatus LAFFTWrapper64::ipps_getFFTsize(int order, int flag,
  IppHintAlgorithm hint,
  int*             SpecSize,
  int*             specBufferSize,
  int*             bufferSize)
{
  IppStatus stat = ippsFFTGetSize_R_64f(order, flag, hint, SpecSize,
    specBufferSize, bufferSize);
  return stat;
}
