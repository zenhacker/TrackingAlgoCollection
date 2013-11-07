#ifndef PARTICLES_H
#define PARTICLES_H

#include "../include/observation.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions 
/////////////////////////////////////////////////////////////////////////////////////////////////////

/* standard deviations for Gaussian sampling in transition model */
#define TRANS_X_STD sqrt((double)8)
#define TRANS_Y_STD sqrt((double)4)
#define TRANS_S_STD 0.025

#define SAMPLE_X_STD sqrt((double)4)
#define SAMPLE_Y_STD sqrt((double)2)
#define SAMPLE_S_STD 0.0165

/* autoregressive dynamics parameters for transition model */
#define A1  2.0
#define A2 -1.0
#define B0  1.0000

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Structures 
/////////////////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************************************
   A particle is an instantiation of the state variables of the system being monitored.  
   A collection of particles is essentially a discretization of the posterior probability of the system.
*****************************************************************************************************/

typedef struct particle 
{
	float x[MAX_OBJECTS];				/**< current x coordinate */
	float y[MAX_OBJECTS];				/**< current y coordinate */
	float s[MAX_OBJECTS];				/**< scale */
	float xp[MAX_OBJECTS];				/**< previous x coordinate */
	float yp[MAX_OBJECTS];				/**< previous y coordinate */
	float sp[MAX_OBJECTS];				/**< previous scale */
	float x0[MAX_OBJECTS];				/**< original x coordinate */
	float y0[MAX_OBJECTS];				/**< original y coordinate */
	int width[MAX_OBJECTS];				/**< original width of region described by particle */
	int height[MAX_OBJECTS];			/**< original height of region described by particle */
	
	float w[MAX_OBJECTS];				/**< weight */
} particle;


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes 
/////////////////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************************************
   Creates an initial distribution of particles by sampling from a Gaussian window 
   around each of a set of specified locations
   
   
   @param regions   ; region describing player locations around which particles are to be sampled
   @param histos    ; histograms describing regions 
   @param particles ; particle array	
   @param n			; the region number
   @param p			; the total number of particles to be assigned
   
   @return Returns an array of a p particles sampled from around region
*****************************************************************************************************/

void init_distribution(CvBox2D region, particle* particels, int n, int p );


/******************************************************************************************************
   Samples a transition model for a given particle

   @param p		; a particle to be transitioned
   @param n		; number of object
   @param w		; video frame width
   @param h		; video frame height

   @return Returns a new particle sampled based on <EM>p</EM>'s transition model
*****************************************************************************************************/

particle transition( particle p, int n, int w, int h, int index, float nVar, float sMin, float sMax );

/******************************************************************************************************
   Compare two particles based on depth.  For use in qsort.

   @param p1 ; pointer to a particle
   @param p2 ; pointer to a particle

   @return Returns -1 if the a p1 has lower depth than a p2, 
                    1 if a p1 has higher depth than \a p2, and 0 if their depth are equal.
*****************************************************************************************************/

int particle_cmp( const void* p1, const void* p2 );

/******************************************************************************************************

   Displays a particle on an image as a rectangle around the region specified by the particle

   @param img	; the image on which to display the particle
   @param p		; the particle to be displayed
   @param color ; the color in which a p is to be displayed
   @param type  ; 0 : rectangle, 1 : ellipse
*****************************************************************************************************/

void display_particle( IplImage* img, particle p, int num_objects, CvScalar* color, FILE* file, int type );

/******************************************************************************************************

   Gaussian random number generator from which to sample

*****************************************************************************************************/
double ran_gaussian();


/******************************************************************************************************

   Samples a proposal density for a given particle

   @param p ; the sample to be transitioned 
   @param m ; the m-th object of sample 
   @param w ; the width of image 
   @param h ; the height of image
   
   @return Returns new sample sampled based on proposal density

*****************************************************************************************************/
particle sample_from_Proposal( particle p, int m, int w, int h, int index, float nVar, float sMin, float sMax );


/******************************************************************************************************

   Calculate acceptance ratio

   @param pn ; the proposed sample 
   @param po ; the current sample 
   @param m ; the m-th object of sample 

   @return Returns acceptance ratio
*****************************************************************************************************/
float acceptance_ratio(particle* pn, particle* po, int m);

/******************************************************************************************************

   Update the sample

   @param pn ; the proposed sample 
   @param po ; the current sample 
   @param m ; the m-th object of sample 

*****************************************************************************************************/
void update_sample( particle* po, particle* pn, int m );

/******************************************************************************************************
   Display functions
*****************************************************************************************************/
void display_object( IplImage* img, CvBox2D region, CvScalar color );

void display_dyn_object( IplImage* img, char* name );

#endif
