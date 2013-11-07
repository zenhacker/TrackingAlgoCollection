/*
  Functions for visual tracking decomposition 
  
  @author Junseok Kwon and Kyoung Mu Lee
  @version 7.0.0-20120405
*/


#include "stdafx.h"
#include "Track.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

/* start frame */
#define START_FRAME 0

/* maximum number of frames for exporting */
#define MAX_FRAMES 5000

/* default base name and extension of exported frames */
#define EXPORT_BASE "./frame/frame_"
#define EXPORT_EXTN ".jpg"
#define EXPORT_VIDEO "./frame/output.avi"

//////////////////////////////////////////////////////////////////////
// Structures 
//////////////////////////////////////////////////////////////////////

typedef struct params 
{
	CvPoint		center[MAX_OBJECTS];
	
	int			width[MAX_OBJECTS];
	int			height[MAX_OBJECTS];

	int			ang[MAX_OBJECTS];
	int			frame[MAX_OBJECTS];

	IplImage*	objects[MAX_OBJECTS];
	
	char* win_name;
	
	IplImage* orig_img;
	IplImage* cur_img;
	
	int n;
} params;

//////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////
histogram** compute_ref_histos( IplImage*, CvBox2D);
histogram** compute_dyn_histos( IplImage** , IplImage**);

int export_frame( IplImage*, int );
int get_regions_from_file( char* , int**, CvBox2D** );
void get_object_image( IplImage* img, IplImage** tmp, int r, int c, int w, int h);

void get_param_from_file();

int nChannel;		
int nFeature;			/* number of features */
int nIter;				/* number of samples */
float nVar;
float sMin;
float sMax;

int num_particles;		/* number of particles */

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrack::CTrack()
{	
	show_all		= FALSE;           
	export_img		= TRUE;            
	export_vid		= FALSE;

	interaction		= TRUE; 
}

CTrack::~CTrack()
{

}

char* StringToChar(CString& str)   
{   
    long len =  str.GetLength();   
    len = len*2;   
    char *szTemp = new char[len+1];    
    memset(szTemp, 0, len+1);   
    USES_CONVERSION;   
    strcpy(szTemp, T2A(str));   
  
    return szTemp;   
} 



void CTrack::RunTracking(CString filename, CString filepath)
{
	// parameter setting
	get_param_from_file();	

	CvCapture* video;
	histogram** ref_histos, ** dyn_histos;
	IplImage* frame, * hsv_frame, * frames[MAX_FRAMES], *cur_features[MAX_FEATURES], *prev_features[MAX_FEATURES];

	particle* particles = NULL;
	particle* new_samples, * old_samples; 
	particle spca_sample; 
	float ratio;

	model observation[MAX_OBJECTS];

	CvScalar* color;
	CvBox2D* regions;
	int* detectfr;

	int num_objects   = 0;
	int total_objects = 0;
	
	int i, j, k, l, w, h, n, o, p;

	i = 0;
	n = 0;
	p = 0;

	// open a video file
	vid_file	= StringToChar(filepath);
	video		= cvCaptureFromFile( vid_file );

	if( ! video )
		AfxMessageBox(_T("couldn't open video file"));
	  
	// open a info file 
	total_objects = get_regions_from_file( (LPSTR)(LPCSTR)(filename+".txt"), &detectfr, &regions );
	color = (CvScalar*)malloc(total_objects*sizeof(CvScalar));

	// create particles
	particles	= (particle*)malloc( num_particles * sizeof( particle ) );
	old_samples = (particle*)malloc( MAX_CHAINS * sizeof( particle ) );
	new_samples = (particle*)malloc( MAX_CHAINS * sizeof( particle ) );

	float max_weight;
	int select_sample = 0;
	IplImage* ref_img[MAX_OBJECTS][2];
	IplImage* dyn_img[MAX_OBJECTS][2];
	int img_queue[MAX_OBJECTS];
	int img_update[MAX_OBJECTS];

	// save the best configuration at each frame 
	FILE *info1;
	info1 = fopen (".\\frame\\best_configuration.txt", "w");

	// matlab library initialization 
	if ( !mclInitializeApplication(NULL,0) || !mclmcrInitialize() )
	{
		AfxMessageBox(_T("could not initialize application properly"));
		return;
	}
	if ( !phaseInitialize() )
	{
		AfxMessageBox(_T("could not initialize application properly"));
		return;
	}
	if ( !DSPCAInitialize())
	{
		AfxMessageBox(_T("could not initialize application properly"));
		return;
	}

	// do tracking
	img_update[0] = 0;
	while( frame = cvQueryFrame( video ) ) 
	{
		// if(!PeekAndPump()){ break; }
		
		// seek the start frame
		if ( p < START_FRAME )
		{
			p++;
			continue;
		}

		// covert RGB into HSV
		hsv_frame = bgr2hsv( frame );	
		frames[i] = (IplImage *)cvClone( frame );

		// extract feature
		extract_feature(hsv_frame, cur_features, img_update[0], nFeature); 
		
		w = frame->width;
		h = frame->height;

		// detect the object 
		while ( n < total_objects && detectfr[n] == i )
		{
			/* compute reference histograms and distribute particles */
			ref_histos = compute_ref_histos( hsv_frame, regions[n]);

			// get initial image 
			get_object_image( hsv_frame,
							  ref_img[n],
							  cvRound( regions[n].center.y ), cvRound( regions[n].center.x ),
							  cvRound( regions[n].size.width ), cvRound( regions[n].size.height ));

			dyn_img[n][0] = ref_img[n][0];
			dyn_img[n][1] = ref_img[n][1];

			// initialize image queue 
			img_update[n]		= 1;
			img_queue[n]		= 0;

			for( j = 0; j < nFeature; j++ )
				for( l = 0; l < MAX_OSBS; l++ )
					observation[n].feature[j][l] = NULL;
		
			for( j = 0; j < nFeature; j++ )
				prev_features[j] = (IplImage *)cvClone( cur_features[j] );

			// initialize particles
			init_distribution( regions[n], particles, n, num_particles );
			
			// display bounding box
			color[n] = CV_RGB(255,255,255);
			display_object( frame, regions[n], CV_RGB(255,0,0));

			n++;
			num_objects = n;
		} 

		int accepted_num	  = 0;
		float acceptance_rate = 0.0f;

		if ( num_objects > 0 )
		{
			//////////////////////
			/* initialize state */ 
			//////////////////////

			// select_sample = rand() % PARTICLES; // randomly choose one sample 
			
			/////////////////
			/////////////////
			/* IMC tracker */
			/////////////////
			/////////////////

			// make observation models
			for( j = 0; j < num_objects; j++ )
			{
				float s = particles[select_sample].s[j];
				make_observation_model(prev_features, 
									   int( particles[select_sample].y[j] + 0.5f ), int( particles[select_sample].x[j] + 0.5f ),
									   int( float(particles[select_sample].width[j]) * s + 0.5f ), int( float(particles[select_sample].height[j]) * s + 0.5f ),
									   img_queue[j], img_update[j], nFeature, observation[j]);
			}

			//////////////
			/* sampling */
			//////////////
			
			// move all targets according to the second order autoregressive motion model
			for ( j = 0; j < MAX_CHAINS; j++ )
			{
				int motion_index	= j%MAX_MOTIONS;
				new_samples[j]		= transition( particles[select_sample], num_objects, w, h, motion_index, nVar, sMin, sMax);
			}

			for( j = 0; j < num_objects; j++ )
			{
				for ( l = 0; l < MAX_CHAINS; l++ )
				{
					int observation_index	= l/MAX_MOTIONS;
					float s = new_samples[l].s[j];
					new_samples[l].w[j]	= likelihood_decomposed( cur_features, 
														int( new_samples[l].y[j] + 0.5f ), int( new_samples[l].x[j] + 0.5f  ),
														int( float(new_samples[l].width[j]) * s + 0.5f ), int( float(new_samples[l].height[j]) * s + 0.5f ), 
														observation[j], observation_index, nFeature, NULL);
					old_samples[l] = new_samples[l];
				}
			}

			k = 0;
			max_weight = 0;

			float rnd, prob;
			float factor	= 0.5f / float(nIter);
			float alpha		= 1.0f;

			for( j = 0; j < nIter; j++ ) 
			{
				int m = rand() % num_objects; // randomly choose one target 
				// int m = j % num_objects;
				
				for ( l = 0; l < MAX_CHAINS; l++ )
				{
					int motion_index		= l%MAX_MOTIONS;
					int observation_index	= l/MAX_MOTIONS;

					// proposal step
					new_samples[l] = sample_from_Proposal(old_samples[l], m, w, h, motion_index, nVar, sMin, sMax); 
				
					// acceptance step
					float s = new_samples[l].s[m];
					new_samples[l].w[m]	= likelihood_decomposed( cur_features, 
																int( new_samples[l].y[m] + 0.5f ), int( new_samples[l].x[m] + 0.5f  ),
																int( float(new_samples[l].width[m]) * s + 0.5f ), int( float(new_samples[l].height[m]) * s + 0.5f ), 
																observation[m], observation_index, nFeature, NULL);

					ratio = acceptance_ratio(&new_samples[l], &old_samples[l], m); 

					// update a sample
					if ( ratio >= 1 )
					{
						update_sample(&old_samples[l], &new_samples[l], m); 
						accepted_num++;
					}
					else 
					{	
						if (  float(rand()) / float(RAND_MAX) < ratio )
						{						
							update_sample(&old_samples[l], &new_samples[l], m);
							accepted_num++;
						}
					}

					// save the particle and do MAP estimation 
					if ( k < num_particles )
					{
						particles[k++] = old_samples[l];

						float weight = 0;
						for ( o = 0; o < num_objects; o++ )
							weight += particles[k-1].w[o];
	
						if ( weight > max_weight )
						{
							select_sample = k-1;
							max_weight = weight;
						}
					}
					else
					{
						l = MAX_CHAINS;
						j = nIter;
					}
					
				}

				if ( interaction )
				{
					// interaction 
					rnd = float(rand()) / float(RAND_MAX); 
					if ( rnd <= alpha )
					{
						// decrease an alpha value
						alpha = alpha - factor;

						for ( l = 0; l < MAX_CHAINS; l++ )
						{
							float temp = 0.0f;
							for ( o = 0; o < MAX_CHAINS; o++ )
								temp += old_samples[o].w[m];

							prob = 0.0f;
							rnd = float(rand()) / float(RAND_MAX);	
							for ( o = 0; o < MAX_CHAINS; o++ )
							{
								prob += old_samples[o].w[m] / temp;
								if ( rnd < prob )
								{
									new_samples[l] = old_samples[o];
									o = MAX_CHAINS;
								}
							}
						}

						for ( l = 0; l < MAX_CHAINS; l++ )
							old_samples[l] = new_samples[l];
					} 
				}
			}
			spca_sample = particles[select_sample];
		}
	    
		// get dynamic image 
		for ( int l = 0; l < num_objects; l++ )
		{
			img_update[l]++;
			img_queue[l]++;
			if ( img_queue[l] == MAX_OSBS )
				img_queue[l] = 1;

			get_object_image( hsv_frame, 
								dyn_img[l],
								particles[select_sample].y[l], 
								particles[select_sample].x[l] ,
								particles[select_sample].width[l] * particles[select_sample].s[l], 
								particles[select_sample].height[l]* particles[select_sample].s[l]);
			
			//if ( dyn_histos )
			//	free(dyn_histos);
			dyn_histos = compute_dyn_histos(dyn_img[l], ref_img[l]);
		}
		
		for ( j = 0; j < nFeature; j++ )
			cvReleaseImage(&prev_features[j]);
		for( j = 0; j < nFeature; j++ )
				prev_features[j] = (IplImage *)cvClone( cur_features[j] );
		for ( j = 0; j < nFeature; j++ )
			cvReleaseImage(&cur_features[j]);

		/* display most likely particle */
		// select_sample = rand() % PARTICLES; // randomly choose one sample
		display_particle( frames[i], spca_sample, num_objects, color, info1, 0 ); 
	
		/* display all particle */ 
		if( show_all )
			for( j = 1; j < num_particles; j++ ) 
				display_particle( frames[i], particles[j], num_objects, color, info1, 0 );
		
		cvNamedWindow( "Video", 1 );
		cvShowImage( "Video", frames[i] );

		cvWaitKey( 5 );

		cvReleaseImage( &hsv_frame ); 

		i++;

		int c = cvWaitKey(10);

		if ( i == MAX_FRAMES || c == 27)
			break;
	}
	
	fclose(info1);

	if ( particles )
		free( particles ); 
	if ( old_samples )
		free( old_samples ); 

	/* export video frames, if export requested */
	CvVideoWriter* videoWriter;	
	double dblFrameRate	= cvGetCaptureProperty(video, CV_CAP_PROP_FPS);
	double codec = cvGetCaptureProperty(video, CV_CAP_PROP_FOURCC);
	
	cvReleaseCapture( &video ); 
	
	if( export_vid )
		videoWriter	= cvCreateVideoWriter(EXPORT_VIDEO, (int)codec, 20, cvSize(frames[0]->width, frames[0]->height),1);
	
	/* export image frames, if export requested */
	for( j = 0; j < i; j++ )
	{
		if( export_img )
			export_frame( frames[j], j+1 );

		if( export_vid )
			int nret = cvWriteFrame( videoWriter, frames[j]);
		
		cvReleaseImage( &frames[j] );
	}

	if( export_vid )
		cvReleaseVideoWriter(&videoWriter);

	AfxMessageBox(_T("finished"));
	cvDestroyWindow("Video");
}

BOOL CTrack::PeekAndPump()
{
	MSG msg;
	
	while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetApp ()->PumpMessage ()) 
		{
			::PostQuitMessage (0);
			return FALSE;
		}
	}
	
	LONG lIdle = 0;
	while (AfxGetApp ()->OnIdle (lIdle++));
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//  Computes a reference histogram for each of the object regions defined by the user
//
//  @param frame; video frame in which to compute histograms
//                should have been converted to hsv using bgr2hsv in observation.h
//  @param regions; regions of a frame over which histograms should be computed
//  @param export if TRUE, object region images are exported
//
//  @return Returns a normalized histograms 
//                  corresponding to regions of a frame specified in a regions.
////////////////////////////////////////////////////////////////////////////////////////////////

histogram** compute_ref_histos( IplImage* frame, CvBox2D region )
{
	IplImage* tmp;
	histogram** histos = (histogram**)malloc( sizeof( histogram ) * 2 );

	/* extract region from frame and compute its histogram */
	for ( int i = 0; i < 2; i++ )
	{
		cvSetImageROI( frame, cvRect( (int)(region.center.x - region.size.width/2.0f + 0.5f), 
									  (int)(region.center.y - region.size.height/2.0f * ( 1 - i ) + 0.5f), 
									   region.size.width, region.size.height /2 ));

		tmp = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 3 );
		cvCopy( frame, tmp, NULL );
		cvResetImageROI( frame );

		histos[i] = calc_histogram( &tmp, 1 );

		normalize_histogram( histos[i] );

		cvReleaseImage( &tmp );
	}
	

	return histos; 
}

////////////////////////////////////////////////////////////////////////////////////////////////
//  Mix a reference histogram with a dynamic histogram
//
//  @return Returns a normalized histograms 
////////////////////////////////////////////////////////////////////////////////////////////////

histogram** compute_dyn_histos( IplImage** dyn_img, IplImage** ref_img)
{
	IplImage* tmp[2];
	histogram** histos = (histogram**)malloc( sizeof( histogram ) * 2 );

	/* extract region from frame and compute its histogram */
	for ( int i = 0; i < 2; i++ )
	{
		tmp[0]		= dyn_img[i];
		tmp[1]		= ref_img[i];

		histos[i]	= calc_histogram( tmp, 2 );

		normalize_histogram( histos[i] );
	}

	return histos;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//  Exports a frame whose name and format are determined by EXPORT_BASE and EXPORT_EXTN, defined above.
//
//  @param frame frame to be exported
//  @param i frame number
////////////////////////////////////////////////////////////////////////////////////////////////
int export_frame( IplImage* frame, int i )
{
	char* name;
	char num[5];

	name = (char*)malloc(strlen(EXPORT_BASE)+strlen(EXPORT_EXTN)+4);

	_snprintf( num, 5, "%04d", i );
	strcpy( name, EXPORT_BASE );
	strcat( name, num );
	strcat( name, EXPORT_EXTN );
	
	int nRet = cvSaveImage( name, frame ); 
	
	return nRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Get object regions from file 
////////////////////////////////////////////////////////////////////////////////////////////////
int get_regions_from_file( char* filename, int** detectfr, CvBox2D** regions )
{
	params p;
	int * f;
	CvBox2D* r;

	int i;

	FILE *file;
	file = fopen (filename, "rb");

	p.n	= 0;

	while ( TRUE )
	{
		if (fscanf(file, "%d %d %d %d %d %d", 
			       &p.center[p.n].x, &p.center[p.n].y, &p.width[p.n], &p.height[p.n], &p.ang[p.n], &p.frame[p.n]) != 6)
		{
			break;
		}

		p.n++;
	}

	/* extract regions defined by user; store as an array of rectangles */
	if( p.n == 0 )
	{
		*regions = NULL;
		return 0;
	}

	r = (CvBox2D*)malloc( p.n * sizeof( CvBox2D ) );
	f = (int*)malloc( p.n * sizeof( int ) );

	for( i = 0; i < p.n; i++ )
	{
		r[i].center.x	= (float)p.center[i].x;
		r[i].center.y	= (float)p.center[i].y;
		
		r[i].size.width	= (float)p.width[i];
		r[i].size.height= (float)p.height[i];

		r[i].angle		= (float)(p.ang[i]*CV_PI/180);
		
		f[i]			= p.frame[i];
	}
	
	*regions	= r;
	*detectfr	= f;

	return p.n;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Get the image of the object regions defined by the user
////////////////////////////////////////////////////////////////////////////////////////////////
void get_object_image( IplImage* img, IplImage** tmp, int r, int c, int w, int h)
{
	/* extract region around (r,c) */
	for ( int i = 0; i < 2; i++ )
	{
		cvSetImageROI( img, cvRect( c-w/2, r-h/2*(1-i), w, h/2 ) );

		tmp[i] = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 3 );
		cvCopy( img, tmp[i], NULL );
		cvResetImageROI( img );
	}
}

void get_param_from_file()
{
	FILE *file;
	file = fopen ("param.txt", "rb");

	fscanf(file, "%d", &nChannel);
	fscanf(file, "%d", &nIter);
	fscanf(file, "%f", &nVar);
	fscanf(file, "%f", &sMin);
	fscanf(file, "%f", &sMax);
	fclose(file);

	if ( nChannel == 0 )
		nFeature = 2;
	else
		nFeature = 4;

	num_particles	= nIter * MAX_CHAINS;
}