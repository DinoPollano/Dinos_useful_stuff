#pragma once
/*
        wrapper of the Intel Primitives Protocol FFT functions
 */
#include <stdio.h>
#include "ipp.h"
#include "common.hpp"

namespace dino
{
  struct FFTWrapperStatus
  {
    enum Status
    {
      GetSizeStatus,         //
      FFTinitStatus,         //
      FFTStatus,             //
      FormartConvertStatus,  //
      MagnitudeStatus,
      IFFTinitStatus,  //
      IFFTStatus,      //
      totalNumStatus
    };
  };

  template <class T>
  class FFTWrapper
  {
   public:
    using IPPcmplxType = typename std::conditional<std::is_same<T, float>::value,
                                                   Ipp32fc, Ipp64fc>::type;
    using IPPfftSpecType =
        typename std::conditional<std::is_same<T, float>::value,
                                  IppsFFTSpec_R_32f, IppsFFTSpec_R_64f>::type;

    using IPPifftSpecType =
        typename std::conditional<std::is_same<T, float>::value,
                                  IppsFFTSpec_C_32f, IppsFFTSpec_C_64f>::type;

    FFTWrapper () : m_fftSize (0)
    {
      static_assert (std::is_floating_point<T> (),
                     "must be floating point or double");
    }

    ~FFTWrapper ()
    {
      if (m_pMemInit)
      {
        ippFree (m_pMemInit);
      }
      if (m_FFTSpec)
      {
        ippFree (m_FFTSpec);
      }
      if (m_pBuffer)
      {
        ippFree (m_pBuffer);
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
    void calculateMagnitude (const T* source, T* magnitude){};
    // in-line hanning function
    void performHanningWindow (T* source, int lengthInsamples){};
    void calculateFFT  (const T* source, T* reDestination, T* imDestination){};
    void calculateIFFT (const T* reSource, const T* imSource, T* destinationRe, T* destinationIm){};

   private:
    size_t m_fftSize;
    Ipp8u *m_pSpecMem = nullptr, *m_piSpecMem =  nullptr, *m_pMemInit = nullptr, *m_pBuffer = nullptr;
    T*     m_interleavedFreqData     = nullptr;
    IPPcmplxType*    m_cmplxFreqData = nullptr;
    IPPfftSpecType*  m_FFTSpec       = nullptr;
    IPPifftSpecType* m_IFFTSpec      = nullptr;
    IppStatus        m_status[FFTWrapperStatus::totalNumStatus];
  };

  #pragma mark -  float implementation
  template <>
  inline void FFTWrapper<float>::prepFFT (int fftLength)
  {
    if (fftLength != m_fftSize)
    {
      m_fftSize = fftLength;
      // prep ipp
      unsigned int fftOrder = static_cast<unsigned int> (ceil (log2 (m_fftSize)));
      int          sizeSpec = 0, sizeInit = 0, sizeBuf = 0;
      m_status[FFTWrapperStatus::GetSizeStatus] =
          ippsFFTGetSize_R_32f (fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuf);

      if (!m_pSpecMem)
      {
        m_pSpecMem = (Ipp8u*)ippMalloc (sizeSpec);
      }
      if (!m_pBuffer)
      {
        m_pBuffer = (Ipp8u*)ippMalloc (sizeBuf);
      }
      if (!m_pMemInit)
      {
        m_pMemInit = (Ipp8u*)ippMalloc (sizeInit);
      }
      m_status[FFTWrapperStatus::IFFTinitStatus] = ippsFFTInit_R_32f (&m_FFTSpec, fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, m_pSpecMem, m_pMemInit);  // initialise FFT
      
      m_status[FFTWrapperStatus::IFFTinitStatus] = ippsFFTInit_C_32f (&m_IFFTSpec, fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, m_piSpecMem, m_pMemInit);  // initialise IFFT
      m_interleavedFreqData = new float[m_fftSize + 2];
      m_cmplxFreqData       = new IPPcmplxType[m_fftSize];
    }
  }

  template <>
  inline void FFTWrapper<float>::calculateMagnitude (const float* source,
                                                     float*       magnitude)
  {
    // perform fft, this will create an interleaved pointer to pointer of
    // complex data
    m_status[FFTWrapperStatus::FFTStatus] = ippsFFTFwd_RToCCS_32f (source, m_interleavedFreqData, m_FFTSpec, m_pBuffer);

    // move the interleaved data into a complex pointer to pointer
    m_status[FFTWrapperStatus::FormartConvertStatus] = ippsConjCcs_32fc (m_interleavedFreqData, m_cmplxFreqData, static_cast<int> (m_fftSize));

    // calculates magnitude (needs the frequency data in a complex pointer to
    // pointer)
    m_status[FFTWrapperStatus::MagnitudeStatus] = ippsMagnitude_32fc (m_cmplxFreqData, magnitude, static_cast<int> ((m_fftSize / 2)));
  }

  template <>
  inline void FFTWrapper<float>::performHanningWindow (float* source,
                                                       int    lengthInsamples)
  {
    ippsWinHann_32f_I (source, lengthInsamples);  // uses IPP's windowing function
  }

  template <>
  inline void FFTWrapper<float>::calculateFFT (const float* source,
                                               float*       reDestination,
                                               float*       imDestination)
  {
    // perform fft, this will create an interleaved pointer to pointer of
    // complex data
    m_status[FFTWrapperStatus::FFTStatus] = ippsFFTFwd_RToCCS_32f ( source, m_interleavedFreqData, m_FFTSpec, m_pBuffer);

    // move the interleaved data into a complex pointer to pointer
    m_status[FFTWrapperStatus::FormartConvertStatus] = ippsConjCcs_32fc ( m_interleavedFreqData, m_cmplxFreqData, static_cast<int> (m_fftSize));
    for (int i = 0; i < m_fftSize; i++)
    {
      reDestination[i] = m_cmplxFreqData[i].re;
      imDestination[i] = m_cmplxFreqData[i].im;
    }
  }

  template <>
  inline void FFTWrapper<float>::calculateIFFT (const float* reSource,
                                                const float* imSource,
                                                float*       destinationRe,
                                                float*       destinationIm)
  {
    m_status[FFTWrapperStatus::IFFTStatus] = ippsFFTInv_CToC_32f ( reSource, imSource, destinationRe, destinationIm, m_IFFTSpec, m_pBuffer);
  }

  #pragma mark -  double implementation
  template <>
  inline void FFTWrapper<double>::prepFFT (int fftLength)
  {
    if (fftLength != m_fftSize)
    {
      m_fftSize = fftLength;
      // prep ipp
      unsigned int fftOrder = static_cast<unsigned int> (ceil (log2 (m_fftSize)));
      int          sizeSpec = 0, sizeInit = 0, sizeBuf = 0;
      m_status[FFTWrapperStatus::GetSizeStatus] = ippsFFTGetSize_R_64f (fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuf);

      if (!m_pSpecMem)
      {
        m_pSpecMem = (Ipp8u*)ippMalloc (sizeSpec);
      }
      if (!m_pBuffer)
      {
        m_pBuffer = (Ipp8u*)ippMalloc (sizeBuf);
      }
      if (!m_pMemInit)
      {
        m_pMemInit = (Ipp8u*)ippMalloc (sizeInit);
      }
      m_status[FFTWrapperStatus::FFTinitStatus] = ippsFFTInit_R_64f (&m_FFTSpec, fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, m_pSpecMem, m_pMemInit);  // initialise FFT
      
      m_status[FFTWrapperStatus::IFFTinitStatus] = ippsFFTInit_C_64f (&m_IFFTSpec, fftOrder, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, m_piSpecMem, m_pMemInit);  // initialise FFT
      
      m_interleavedFreqData = new double[m_fftSize + 2];
      m_cmplxFreqData       = new IPPcmplxType[m_fftSize];
    }
  }

  template <>
  inline void FFTWrapper<double>::calculateMagnitude (const double* source,
                                                      double*       magnitude)
  {
    // perform fft, this will create an interleaved pointer to pointer of
    // complex data
    m_status[FFTWrapperStatus::FFTStatus] = ippsFFTFwd_RToCCS_64f (source, m_interleavedFreqData, m_FFTSpec, m_pBuffer);

    // move the interleaved data into a complex pointer to pointer
    m_status[FFTWrapperStatus::FormartConvertStatus] = ippsConjCcs_64fc (m_interleavedFreqData, m_cmplxFreqData, static_cast<int> (m_fftSize));

    // calculates magnitude (needs the frequency data in a complex pointer to
    // pointer)
    m_status[FFTWrapperStatus::MagnitudeStatus] = ippsMagnitude_64fc (m_cmplxFreqData, magnitude, static_cast<int> ((m_fftSize / 2)));
  }

  template <>
  inline void FFTWrapper<double>::performHanningWindow (double* source,int     lengthInsamples)
  {
    ippsWinHann_64f_I (source, lengthInsamples);  // uses IPP's windowing function
  }

  template <>
  inline void FFTWrapper<double>::calculateFFT (const double* source,
                                                double*       reDestination,
                                                double*       imDestination)
  {
    // perform fft, this will create an interleaved pointer to pointer of
    // complex data
    m_status[FFTWrapperStatus::FFTStatus] = ippsFFTFwd_RToCCS_64f (source, m_interleavedFreqData, m_FFTSpec, m_pBuffer);

    // move the interleaved data into a complex pointer to pointer
    m_status[FFTWrapperStatus::FormartConvertStatus] = ippsConjCcs_64fc (m_interleavedFreqData, m_cmplxFreqData, static_cast<int> (m_fftSize));
    for (int i = 0; i < m_fftSize; i++)
    {
      reDestination[i] = m_cmplxFreqData[i].re;
      imDestination[i] = m_cmplxFreqData[i].im;
    }
  }
  template <>
  inline void FFTWrapper<double>::calculateIFFT (const double* reSource,
                                                 const double* imSource,
                                                 double*       destinationRe,
                                                 double*       destinationIm)
  {
    m_status[FFTWrapperStatus::IFFTStatus] = ippsFFTInv_CToC_64f (reSource, imSource, destinationRe, destinationIm, m_IFFTSpec, m_pBuffer);
  }
  
}  // namespace dino
