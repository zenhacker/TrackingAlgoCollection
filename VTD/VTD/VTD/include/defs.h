	#ifndef DEFS_H
#define DEFS_H

/********************************* Includes **********************************/
#include "../stdAfx.h"

/* From OpenCV library */
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"


/******************************* Defs and macros *****************************/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MIN
#define MIN(x,y) ( ( x < y )? x : y )
#endif
#ifndef MAX
#define MAX(x,y) ( ( x > y )? x : y )
#endif
#ifndef ABS
#define ABS(x) ( ( x < 0 )? -x : x )
#endif

/********************************** Structures *******************************/

/* maximum number of objects to be tracked */
#define MAX_OBJECTS 10

/* maximum number of features*/
#define MAX_FEATURES 4

/* maximum number of chains */
#define MAX_MOTIONS 2 

/* maximum number of chains */
#define MAX_CHAINS 4 * MAX_MOTIONS

/* maximum number of observations */
#define MAX_OSBS 5

/* maximum number of observations for updating */
#define MAX_UPDATE 2 

/* Penalty factor controlling sparsity (large rho => sparse sol) */
#define RHO 90

/* Target precision  */ 
#define  GAP 0.3e-2

#define THRES 0.1

#define MW 20 
#define MH 60

#endif
