#include "../include/defs.h"
#include "../include/utils.h"
#include "../include/particles.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function definition 
/////////////////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************************************
   Creates an initial distribution of particles by sampling from a Gaussian window 
   around each of a set of specified locations
   
   
   @param regions   ; an array of regions describing player locations around which particles are to be sampled
   @param histos    ; array of histograms describing regions in a regions
   @param particles ; particle array
   @param n			; the region number
   @param p			; the total number of particles to be assigned
   
   @return Returns an array of a p particles sampled from around regions in a regions
*****************************************************************************************************/
void init_distribution( CvBox2D region, particle* particles, int n, int p )
{
	float x, y, a;
	int j, width, height;

	/* create particles at the centers of each of n regions */
	width	= (int)region.size.width;
	height	= (int)region.size.height;
	
	x		= region.center.x;
	y		= region.center.y;
	a		= region.angle;

	for( j = 0; j < p; j++ )
	{
		particles[j].x0[n]			= particles[j].xp[n] = particles[j].x[n] = x;
		particles[j].y0[n]			= particles[j].yp[n] = particles[j].y[n] = y;
		particles[j].sp[n]			= particles[j].s[n]  = 1.0f;
		particles[j].width[n]		= width;
		particles[j].height[n]		= height;

		particles[j].w[n]			= 0.0f;
	}
}



/******************************************************************************************************
   Samples a transition model for a given particle

   @param p		; a particle to be transitioned
   @param n		; number of object
   @param w		; video frame width
   @param h		; video frame height
 
   @return Returns a new particle sampled based on <EM>p</EM>'s transition model
*****************************************************************************************************/
particle transition( particle p, int n, int w, int h, int index, float nVar, float sMin, float sMax )
{
	int i;
	float x, y, s, param;
	particle pn;

	if ( index == 0 )
		param = 1.0f * nVar; 
	else 
		param = 2.0f * nVar; 

	for ( i = 0; i < n; i++ )
	{
		/* sample new position using second-order autoregressive dynamics */
		x		= (float)(A1 * ( p.x[i] - p.x0[i] ) + A2 * ( p.xp[i] - p.x0[i] ) + B0 * param * TRANS_X_STD * ran_gaussian() + p.x0[i]);
		pn.x[i]	= (float)MAX( 10.0f, MIN( (float)w - 1.0f, x ) );

		y		= (float)(A1 * ( p.y[i] - p.y0[i] ) + A2 * ( p.yp[i] - p.y0[i] ) + B0 * param * TRANS_Y_STD * ran_gaussian() + p.y0[i]);
		pn.y[i]	= (float)MAX( 10.0f, MIN( (float)h - 1.0f, y ) );

		s		= (float)(A1 * ( p.s[i] - 1.0f ) + A2 * ( p.sp[i] - 1.0f ) 	 + B0 * param * TRANS_S_STD * ran_gaussian() + 1.0f);
		pn.s[i]	= (float)MAX( sMin, MIN(sMax, s) ); 

		pn.xp[i]	= p.x[i];
		pn.yp[i]	= p.y[i];
		pn.sp[i]	= p.s[i];
		
		pn.x0[i]	= p.x0[i];
		pn.y0[i]	= p.y0[i];

		pn.width[i]	= p.width[i];
		pn.height[i]= p.height[i];  
		
		pn.w[i]		= 0.0f;
	}

	
	return pn;
}

/******************************************************************************************************
   Compare two particles based on depth.  For use in qsort.

   @param p1 ; pointer to a particle
   @param p2 ; pointer to a particle

   @return Returns  1 if the a p1 has lower depth than a p2, 
                   -1 if a p1 has higher depth than \a p2, and 0 if their depth are equal.
*****************************************************************************************************/
int particle_cmp( const void* p1, const void* p2 )
{
	float* _p1 = (float*)p1;
	float* _p2 = (float*)p2;

	if( *_p1 > *_p2 )
		return 1;
	if( *_p1 < *_p2 )
		return -1;
	
	return 0;
}

/******************************************************************************************************

   Displays a particle on an image as a rectangle around the region specified by the particle

   @param img	; the image on which to display the particle
   @param p		; the particle to be displayed
   @param color ; the color in which a p is to be displayed
   @param type  ; 0 : rectangle, 1 : ellipse
*****************************************************************************************************/
void display_particle( IplImage* img, particle p, int n, CvScalar* color , FILE* file, int type)
{
	int i;
	CvBox2D box;
	CvPoint2D32f boxPoints[4];

	for ( i = 0; i < n; i++ )
	{
		box.center.x	= p.x[i];
		box.center.y	= img->height-p.y[i];
		
		box.size.width	= p.s[i] * p.width[i];
		box.size.height	= p.s[i] * p.height[i];

		box.angle		= (float)CV_PI/2;

		cvBoxPoints(box, boxPoints);

		fprintf(file,"%d %d %d %d\n",(int)boxPoints[0].x, (int)img->height-(int)boxPoints[0].y, 
									 (int)boxPoints[2].x, (int)img->height-(int)boxPoints[2].y);
		if ( type == 0)
		{
			cvLine(img,
					cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
					cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
					color[i], 5);
			cvLine(img,
					cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
					cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
					color[i], 5);
			cvLine(img,
					cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
					cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
					color[i], 5);
			cvLine(img,
					cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
					cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
					color[i], 5);
		}
		else
		{
			cvEllipse (img, cvPoint (int(box.center.x+0.5f), int(box.center.y+0.5f)), 
				       cvSize (int(box.size.width/2+0.5f), int(box.size.height/2+0.5f)), 
					   0, 0, 360, color[i], 2); 
		}
	}
}

/******************************************************************************************************

   Gaussian random number generator from which to sample

   @param NoiseStdDev ; noise deviation

   @return Returns noise value

*****************************************************************************************************/
double ran_gaussian()
{
	static double V1, V2, S;
	static int phase = 0;
	double X;

	if(phase == 0) 
	{
		do 
		{
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while(S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	} 
	else
		X = V2 * sqrt(-2 * log(S) / S);

	phase = 1 - phase;

	return X/3; 
}

/******************************************************************************************************

   MCMC Sampling step

*****************************************************************************************************/

/******************************************************************************************************

   Samples a proposal density for a given particle

   @param p ; the sample to be transitioned 
   @param m ; the m-th object of sample 
   @param w ; the width of image 
   @param h ; the height of image

   @return Returns new sample sampled based on proposal density

*****************************************************************************************************/
particle sample_from_Proposal( particle p, int m, int w, int h, int index, float nVar, float sMin, float sMax )
{
	particle pn;
	float x, y, s, param;

	if ( index == 0 ) 
		param = 1.0f * nVar; 
	else 
		param = 2.0f * nVar; 

	/* simply perturb the target sate according to a zero-mean & standard deviation */
	x		= (float)(B0 * param * SAMPLE_X_STD * ran_gaussian() + p.x[m]);
	pn.x[m]	= (float)MAX( p.width[m]*p.s[m]/2, MIN( (float)w - p.width[m]*p.s[m]/2, x ) );

	y		= (float)(B0 * param * SAMPLE_Y_STD * ran_gaussian() + p.y[m]);
	pn.y[m]	= (float)MAX( p.width[m]*p.s[m]/2, MIN( (float)h - p.width[m]*p.s[m]/2, y ) );
  
	s		= (float)(B0 * param * SAMPLE_S_STD * ran_gaussian() + p.s[m]); 
	pn.s[m]	= (float)MAX( sMin, MIN(sMax, s) ); 

	pn.xp[m]		= p.x[m];
	pn.yp[m]		= p.y[m];
	pn.sp[m]		= p.s[m];

	pn.x0[m]		= p.x0[m];
	pn.y0[m]		= p.y0[m];

	pn.width[m]		= p.width[m];
	pn.height[m]	= p.height[m];

	return pn;
}

/******************************************************************************************************

   Calculate acceptance ratio

   @param pn ; the proposed sample 
   @param po ; the current sample 
   @param m ; the m-th object of sample 

   @return Returns acceptance ratio
*****************************************************************************************************/
float acceptance_ratio(particle* pn, particle* po, int m)
{
	float ratio;

	////////////////////////////////
	/* calculate acceptance ratio */
	///////////////////////////////
	ratio = ( 1.0f * (float)log(pn->w[m]) ) - ( 1.0f * (float)log(po->w[m]) ); 

	return (float)exp(ratio); 
}

/******************************************************************************************************

   Update the sample

   @param pn ; the proposed sample 
   @param po ; the current sample 

*****************************************************************************************************/
void update_sample(particle* po, particle* pn, int m)
{
	po->xp[m]	= po->x[m];
	po->yp[m]	= po->y[m];
	po->sp[m]	= po->s[m];

	po->x[m]	= pn->x[m];
	po->y[m]	= pn->y[m];
	po->s[m]	= pn->s[m];

	po->w[m]	= pn->w[m];
}

/******************************************************************************************************
   Display functions
*****************************************************************************************************/
void display_object( IplImage* img, CvBox2D region, CvScalar color )
{
	CvBox2D roi;
	CvPoint2D32f boxPoints[4];
	IplImage* orig_img	= (IplImage*)cvClone( img );
	
	roi			 = region;
	roi.center.y = orig_img->height - region.center.y;


	cvBoxPoints(roi, boxPoints);

	cvLine(orig_img,
			cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
			cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
			color);
	cvLine(orig_img,
			cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
			cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
			color);
	cvLine(orig_img,
			cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
			cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
			color);
	cvLine(orig_img,
			cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
			cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
			color);

	cvNamedWindow( "detect object", 1 );
	cvShowImage( "detect object", orig_img );

	cvWaitKey( 0 );

	cvDestroyWindow( "detect object" );
}

void display_dyn_object( IplImage* img, char* name )
{
	cvNamedWindow( name, 1 );
	cvShowImage( name, img );		
}
