//
// MATLAB Compiler: 4.16 (R2011b)
// Date: Thu Apr 05 04:07:18 2012
// Arguments: "-B" "macro_default" "-W" "cpplib:phase" "-T" "link:lib"
// "phasecong2.m" "phasecong.m" "lowpassfilter.m" "-v" 
//

#ifndef __phase_h
#define __phase_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_phase
#define PUBLIC_phase_C_API __global
#else
#define PUBLIC_phase_C_API /* No import statement needed. */
#endif

#define LIB_phase_C_API PUBLIC_phase_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_phase
#define PUBLIC_phase_C_API __declspec(dllexport)
#else
#define PUBLIC_phase_C_API __declspec(dllimport)
#endif

#define LIB_phase_C_API PUBLIC_phase_C_API


#else

#define LIB_phase_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_phase_C_API 
#define LIB_phase_C_API /* No special import/export declaration */
#endif

extern LIB_phase_C_API 
bool MW_CALL_CONV phaseInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_phase_C_API 
bool MW_CALL_CONV phaseInitialize(void);

extern LIB_phase_C_API 
void MW_CALL_CONV phaseTerminate(void);



extern LIB_phase_C_API 
void MW_CALL_CONV phasePrintStackTrace(void);

extern LIB_phase_C_API 
bool MW_CALL_CONV mlxPhasecong2(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_phase_C_API 
bool MW_CALL_CONV mlxPhasecong(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_phase_C_API 
bool MW_CALL_CONV mlxLowpassfilter(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_phase_C_API 
long MW_CALL_CONV phaseGetMcrID();


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_phase
#define PUBLIC_phase_CPP_API __declspec(dllexport)
#else
#define PUBLIC_phase_CPP_API __declspec(dllimport)
#endif

#define LIB_phase_CPP_API PUBLIC_phase_CPP_API

#else

#if !defined(LIB_phase_CPP_API)
#if defined(LIB_phase_C_API)
#define LIB_phase_CPP_API LIB_phase_C_API
#else
#define LIB_phase_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_phase_CPP_API void MW_CALL_CONV phasecong2(int nargout, mwArray& M, mwArray& m, mwArray& or0, mwArray& featType, mwArray& PC, mwArray& EO, const mwArray& varargin);

extern LIB_phase_CPP_API void MW_CALL_CONV phasecong2(int nargout, mwArray& M, mwArray& m, mwArray& or0, mwArray& featType, mwArray& PC, mwArray& EO);

extern LIB_phase_CPP_API void MW_CALL_CONV phasecong(int nargout, mwArray& phaseCongruency, mwArray& orientation, mwArray& featType, const mwArray& im, const mwArray& nscale, const mwArray& norient, const mwArray& minWaveLength, const mwArray& mult, const mwArray& sigmaOnf, const mwArray& dThetaOnSigma, const mwArray& k, const mwArray& cutOff);

extern LIB_phase_CPP_API void MW_CALL_CONV lowpassfilter(int nargout, mwArray& f, const mwArray& sze, const mwArray& cutoff, const mwArray& n);

#endif
#endif
