//
//  IFFTWrapper.h
//  IFFTWrapper
//
//  Created by Christofero Pollano on 17/05/2017.
//  Copyright © 2017 dino. All rights reserved.
//
#pragma once
#include <stdio.h>
#include "ipp.h"
#include <vector>
#include "math.h"
#include <type_traits>

namespace dino
{
  struct IFFTWrapperStatus
  {
    enum Status
    {
      GetSizeStatus,         //
      FormartConvertStatus,  //
      MagnitudeStatus,
      IFFTinitStatus,  //
      IFFTStatus,      //
      totalNumStatus
    };
  };
  
  template <class T>
  class IFFTWrapper
  {
  public:
    using IPPcmplxType = typename std::conditional<std::is_same<T, float>::value,
    Ipp32fc, Ipp64fc>::type;
    
    using IPPifftSpecType =
    typename std::conditional<std::is_same<T, float>::value,
    IppsFFTSpec_C_32f, IppsFFTSpec_C_64f>::type;
    
    IFFTWrapper () : m_fftSize (0)
    {
      static_assert (std::is_floating_point<T> (),
                     "must be floating point or double");
    }
    
    ~IFFTWrapper ()
    {
      if (m_piMemInit)
      {
        ippFree (m_piMemInit);
      }
      if (m_IFFTSpec)
      {
        ippFree (m_IFFTSpec);
      }
      if (m_piBuffer)
      {
        ippFree (m_piBuffer);
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
    
    IppStatus getStatus (size_t StatusIndex) { return m_status[StatusIndex]; }
#pragma mark - function shells
    void prepFFT (int fftLenght){};
    // this method assumes source is a real signal, it should also be the same
    // length as the FFT size
    void calculateIFFT (const T* reSource, const T* imSource, T* destinationRe,
                        T* destinationIm){};
  private:
    size_t m_fftSize;
    Ipp8u *m_piSpecMem = nullptr, *m_piMemInit = nullptr, *m_piBuffer = nullptr;
    T*     m_interleavedFreqData     = nullptr;
    IPPcmplxType*    m_cmplxFreqData = nullptr;
    IPPifftSpecType*  m_IFFTSpec       = nullptr;
    IppStatus        m_status[IFFTWrapperStatus::totalNumStatus];
  };
  
#pragma mark -  float implementation
  template <>
  inline void IFFTWrapper<float>::prepFFT (int fftLength)
  {
    if (fftLength != m_fftSize)
    {
      m_fftSize = fftLength;
      // prep ipp
      unsigned int fftOrder = static_cast<unsigned int> (ceil (log2 (m_fftSize)));
      int          sizeSpec = 0, sizeInit = 0, sizeBuf = 0;
      m_status[IFFTWrapperStatus::GetSizeStatus] =
		    ippsFFTGetSize_C_32f (fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone,
                              &sizeSpec, &sizeInit, &sizeBuf);
      
      if (!m_piSpecMem)
      {
        m_piSpecMem = (Ipp8u*)ippMalloc (sizeSpec);
      }
      if (!m_piBuffer)
      {
        m_piBuffer = (Ipp8u*)ippMalloc (sizeBuf);
      }
      if (!m_piMemInit)
      {
        m_piMemInit = (Ipp8u*)ippMalloc (sizeInit);
      }

      m_status[IFFTWrapperStatus::IFFTinitStatus] = ippsFFTInit_C_32f (
                                                                      &m_IFFTSpec, fftOrder, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, m_piSpecMem,
                                                                      m_piMemInit);  // initialise IFFT
      m_interleavedFreqData = new float[m_fftSize + 2];
      m_cmplxFreqData       = new IPPcmplxType[m_fftSize];
    }
  }
  
  template <>
  inline void IFFTWrapper<float>::calculateIFFT (const float* reSource,
                                                const float* imSource,
                                                float*       destinationRe,
                                                float*       destinationIm)
  {
    m_status[IFFTWrapperStatus::IFFTStatus] = ippsFFTInv_CToC_32f (
                                                                  reSource, imSource, destinationRe, destinationIm, m_IFFTSpec, m_piBuffer);
  }
  
#pragma mark -  double implementation
  template <>
  inline void IFFTWrapper<double>::prepFFT (int fftLength)
  {
    if (fftLength != m_fftSize)
    {
      m_fftSize = fftLength;
      // prep ipp
      unsigned int fftOrder = static_cast<unsigned int> (ceil (log2 (m_fftSize)));
      int          sizeSpec = 0, sizeInit = 0, sizeBuf = 0;
      m_status[IFFTWrapperStatus::GetSizeStatus] =
		    ippsFFTGetSize_C_64f (fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone,
                              &sizeSpec, &sizeInit, &sizeBuf);
      
      if (!m_piSpecMem)
      {
        m_piSpecMem = (Ipp8u*)ippMalloc (sizeSpec);
      }
      if (!m_piBuffer)
      {
        m_piBuffer = (Ipp8u*)ippMalloc (sizeBuf);
      }
      if (!m_piMemInit)
      {
        m_piMemInit = (Ipp8u*)ippMalloc (sizeInit);
      }
      
      m_status[IFFTWrapperStatus::IFFTinitStatus] = ippsFFTInit_C_64f (
                                                                      &m_IFFTSpec, fftOrder, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, m_piSpecMem,
                                                                      m_piMemInit);  // initialise FFT
      
      m_interleavedFreqData = new double[m_fftSize + 2];
      m_cmplxFreqData       = new IPPcmplxType[m_fftSize];
    }
  }
 
  template <>
  inline void IFFTWrapper<double>::calculateIFFT (const double* reSource,
                                                 const double* imSource,
                                                 double*       destinationRe,
                                                 double*       destinationIm)
  {
    m_status[IFFTWrapperStatus::IFFTStatus] = ippsFFTInv_CToC_64f (
                                                                  reSource, imSource, destinationRe, destinationIm, m_IFFTSpec, m_piBuffer);
  }
  
}  // namespace dino
