//
// MATLAB Compiler: 4.16 (R2011b)
// Date: Thu Apr 05 04:05:24 2012
// Arguments: "-B" "macro_default" "-W" "cpplib:DSPCA" "-T" "link:lib"
// "DSPCA.m" "sparse_rank_one_mex.mexw32" "-v" 
//

#ifndef __DSPCA_h
#define __DSPCA_h 1

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

#ifdef EXPORTING_DSPCA
#define PUBLIC_DSPCA_C_API __global
#else
#define PUBLIC_DSPCA_C_API /* No import statement needed. */
#endif

#define LIB_DSPCA_C_API PUBLIC_DSPCA_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_DSPCA
#define PUBLIC_DSPCA_C_API __declspec(dllexport)
#else
#define PUBLIC_DSPCA_C_API __declspec(dllimport)
#endif

#define LIB_DSPCA_C_API PUBLIC_DSPCA_C_API


#else

#define LIB_DSPCA_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_DSPCA_C_API 
#define LIB_DSPCA_C_API /* No special import/export declaration */
#endif

extern LIB_DSPCA_C_API 
bool MW_CALL_CONV DSPCAInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_DSPCA_C_API 
bool MW_CALL_CONV DSPCAInitialize(void);

extern LIB_DSPCA_C_API 
void MW_CALL_CONV DSPCATerminate(void);



extern LIB_DSPCA_C_API 
void MW_CALL_CONV DSPCAPrintStackTrace(void);

extern LIB_DSPCA_C_API 
bool MW_CALL_CONV mlxDSPCA(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_DSPCA_C_API 
bool MW_CALL_CONV mlxSparse_rank_one_mex(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                         *prhs[]);

extern LIB_DSPCA_C_API 
long MW_CALL_CONV DSPCAGetMcrID();


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_DSPCA
#define PUBLIC_DSPCA_CPP_API __declspec(dllexport)
#else
#define PUBLIC_DSPCA_CPP_API __declspec(dllimport)
#endif

#define LIB_DSPCA_CPP_API PUBLIC_DSPCA_CPP_API

#else

#if !defined(LIB_DSPCA_CPP_API)
#if defined(LIB_DSPCA_C_API)
#define LIB_DSPCA_CPP_API LIB_DSPCA_C_API
#else
#define LIB_DSPCA_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_DSPCA_CPP_API void MW_CALL_CONV DSPCA(int nargout, mwArray& U, mwArray& X, mwArray& x, mwArray& F, mwArray& k, mwArray& dualitygap, mwArray& cputime, mwArray& perceigs, const mwArray& A, const mwArray& rho, const mwArray& gapchange, const mwArray& maxiter, const mwArray& info, const mwArray& algo);

extern LIB_DSPCA_CPP_API void MW_CALL_CONV sparse_rank_one_mex(int nargout, mwArray& varargout, const mwArray& varargin);

extern LIB_DSPCA_CPP_API void MW_CALL_CONV sparse_rank_one_mex(int nargout, mwArray& varargout);

extern LIB_DSPCA_CPP_API void MW_CALL_CONV sparse_rank_one_mex(const mwArray& varargin);

extern LIB_DSPCA_CPP_API void MW_CALL_CONV sparse_rank_one_mex();

#endif
#endif
