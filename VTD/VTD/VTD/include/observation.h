#if !defined(AFX_TRACK_H__E7EFF2ED_BD45_4C22_A733_A1AA3E3CE4ED__INCLUDED_)
#define AFX_TRACK_H__E7EFF2ED_BD45_4C22_A733_A1AA3E3CE4ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/DSPCA.h"
#include "../include/phase.h"
#include "../include/defs.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/* number of bins of HSV in histogram */
#define NH 10
#define NS 10
#define NV 10

/* max HSV values */
#define H_MAX 360.0
#define S_MAX 1.0
#define V_MAX 1.0

/* low thresholds on saturation and value for histogramming */
#define S_THRESH 0.1
#define V_THRESH 0.2

/* distribution parameter */
#define LAMBDA 5 

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Structures 
/////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct histogram 
{
	float histo[NH*NS + NV];   /**< histogram array */
	int n;                     /**< length of histogram array */
} histogram;


typedef struct chain 
{
	int num;
	int pc_f[MAX_OSBS*MAX_FEATURES];
	int pc_i[MAX_OSBS*MAX_FEATURES];

	float pc_w[MAX_OSBS*MAX_FEATURES];

} chain;

typedef struct model 
{
	chain chains[MAX_CHAINS];

	IplImage* feature[MAX_FEATURES][MAX_OSBS];

} model;

/////////////////////////////////////////////////////////////////////////////////////////////////
// Function Definitions 
/////////////////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************************
   Converts a BGR image to HSV color space

   @param img image to be converted

   @return Returns img converted to a 3-channel, 
   32-bit HSV image with S and V values in the range [0,1] and H value in the range [0,360]
***********************************************************************************************/
IplImage* bgr2hsv( IplImage* img );


/***********************************************************************************************
   Calculates the histogram bin into which an HSV entry falls
   
   @param h Hue
   @param s Saturation
   @param v Value
   
   @return Returns the bin index corresponding to the HSV color defined by \a h, \a s, and \a v.
***********************************************************************************************/
int histo_bin( float h, float s, float v );


/***********************************************************************************************
   Calculates a cumulative histogram as defined above for a given array of images
   
   @param imgs an array of images over which to compute a cumulative histogram;
               each must have been converted to HSV colorspace using bgr2hsv()
   @param n the number of images in imgs
   
   @return Returns an un-normalized HSV histogram for \a imgs
***********************************************************************************************/
histogram* calc_histogram( IplImage** imgs, int n );


/***********************************************************************************************
   Normalizes a histogram so all bins sum to 1.0
   
   @param histo a histogram
***********************************************************************************************/
void normalize_histogram( histogram* histo );


/***********************************************************************************************************
   Computes squared distance metric based on the Battacharyya similarity coefficient between histograms.

   @param h1 first histogram; should be normalized
   @param h2 second histogram; should be normalized
   
   @return Rerns a squared distance based on the Battacharyya similarity coefficient between \a h1 and \a h2
************************************************************************************************************/
float histo_dist_sq( histogram* h1, histogram* h2 );


/***********************************************************************************************************
   Computes the likelihood of there being a player at a given location in an image

   @param img image that has been converted to HSV color space using bgr2hsv()
   @param r row location of center of window around which to compute likelihood
   @param c col location of center of window around which to compute likelihood
   @param w width of region over which to compute likelihood
   @param h height of region over which to compute likelihood
   @param ref_histo reference histogram for a player; must have been normalized with normalize_histogram()

   @return Returns the likelihood of there being a player at location (\a r, \a c) in \a img
************************************************************************************************************/
float likelihood_color( IplImage* img, int r, int c, int w, int h, histogram** ref_histo);

float likelihood_decomposed( IplImage** features, int r, int c, int w, int h, model& observation, int chain_index, int nFeature, FILE* info );

void extract_feature(IplImage* image, IplImage** f, int update, int nFeature);

void make_observation_model(IplImage** features, int r, int c, int w, int h, int queue, int update, int nFeature, model& observation);

#endif // !defined(AFX_TRACK_H__E7EFF2ED_BD45_4C22_A733_A1AA3E3CE4ED__INCLUDED_)
