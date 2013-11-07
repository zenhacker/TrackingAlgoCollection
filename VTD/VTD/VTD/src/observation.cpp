#include "../stdafx.h" 
#include "../include/defs.h"
#include "../include/utils.h"
#include "../include/observation.h"
#include "../include/dd_head.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//  Converts a BGR image to HSV color space
// 
//  @param bgr image to be converted
//  
//  @return Returns bgr converted to a 3-channel, 32-bit HSV image 
//  with S and V values in the range [0,1] and H value in the range [0,360]
//////////////////////////////////////////////////////////////////////////////////////////////
IplImage* bgr2hsv( IplImage* bgr )
{
	IplImage* bgr_flip, * bgr32f, * hsv;

	bgr32f	= cvCreateImage( cvGetSize(bgr), IPL_DEPTH_32F, 3 );
	hsv		= cvCreateImage( cvGetSize(bgr), IPL_DEPTH_32F, 3 );

	bgr_flip = (IplImage*)cvClone(bgr);
	cvConvertImage(bgr,bgr_flip,1);

	cvConvertScale( bgr_flip, bgr32f, 1.0 / 255.0, 0 );
	cvCvtColor( bgr32f, hsv, CV_BGR2HSV );
	
	cvReleaseImage( &bgr_flip );
	cvReleaseImage( &bgr32f );

	return hsv;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//  Calculates the histogram bin into which an HSV entry falls
//  
//  @param h Hue
//  @param s Saturation
//  @param v Value
//  
//  @return Returns the bin index corresponding to the HSV color defined by
//    \a h, \a s, and \a v.
//////////////////////////////////////////////////////////////////////////////////////////////
int histo_bin( float h, float s, float v )
{
	int hd, sd, vd;

	/* if S or V is less than its threshold, return a "colorless" bin */
	vd = MIN( (int)(v * NV / V_MAX), NV-1 );
	if( s < S_THRESH  ||  v < V_THRESH )
		return NH * NS + vd;

	/* otherwise determine "colorful" bin */
	hd = MIN( (int)(h * NH / H_MAX), NH-1 );
	sd = MIN( (int)(s * NS / S_MAX), NS-1 );
	
	return sd * NH + hd;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Calculates a cumulative histogram as defined above for a given array of images
//  
//  @param img an array of images over which to compute a cumulative histogram;
//             each must have been converted to HSV colorspace using bgr2hsv()
//  @param n the number of images in imgs
//    
//  @return Returns an un-normalized HSV histogram for \a imgs
//////////////////////////////////////////////////////////////////////////////////////////////
histogram* calc_histogram( IplImage** imgs, int n )
{
	IplImage* img;
	IplImage* h, * s, * v;
		
	histogram* histo;

	float* hist;
	int i, r, c, bin;

	histo = (histogram*)malloc( sizeof(histogram) );
	histo->n = NH*NS + NV;
	hist = histo->histo;
	memset( hist, 0, histo->n * sizeof(float) );

	for( i = 0; i < n; i++ )
	{
		/* extract individual HSV planes from image */
		img = imgs[i];

		h = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
		s = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
		v = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
		cvCvtPixToPlane( img, h, s, v, NULL );

		/* increment appropriate histogram bin for each pixel */
		for( r = 0; r < img->height; r++ )
			for( c = 0; c < img->width; c++ )
			{
				bin = histo_bin( pixval32f( h, r, c ),
								 pixval32f( s, r, c ),
								 pixval32f( v, r, c ) );
				hist[bin] += 1;
			}

		cvReleaseImage( &h );
		cvReleaseImage( &s );
		cvReleaseImage( &v );
	}

	return histo;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Normalizes a histogram so all bins sum to 1.0
// 
//  @param histo a histogram
//////////////////////////////////////////////////////////////////////////////////////////////
void normalize_histogram( histogram* histo )
{
	float* hist;
	float sum = 0, inv_sum;
	
	int i, n;

	hist	= histo->histo;
	n		= histo->n;

	/* compute sum of all bins and multiply each bin by the sum's inverse */
	for( i = 0; i < n; i++ )
		sum += hist[i];
	
	inv_sum = (float)(1.0 / sum);
	
	for( i = 0; i < n; i++ )
		hist[i] *= inv_sum;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Computes squared distance metric 
//	based on the Battacharyya similarity coefficient between histograms.
//  
//  @param h1 first  histogram; should be normalized
//  @param h2 second histogram; should be normalized
//  
//  @return Returns a squared distance 
//  based on the Battacharyya similarity coefficient between \a h1 and \a h2
//////////////////////////////////////////////////////////////////////////////////////////////
float histo_dist_sq( histogram* h1, histogram* h2 )
{
	float* hist1, * hist2;
	float sum = 0;
	int i, n;

	n		= h1->n;
	hist1	= h1->histo;
	hist2	= h2->histo;

	/*
	According the the Battacharyya similarity coefficient,

	D = sqrt{ 1 - sum_1^n{ sqrt{ h_1(i) * h_2(i) } } }
	*/

	for( i = 0; i < n; i++ )
		sum += (float)(sqrt( hist1[i]*hist2[i] ));
	
	return (float)(1.0 - sum);
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Computes the likelihood of there being a player at a given location in an image
//  
//  @param img image that has been converted to HSV color space using bgr2hsv()
//  @param r row location of center of window around which to compute likelihood
//  @param c col location of center of window around which to compute likelihood
//  @param w width of region over which to compute likelihood
//  @param h height of region over which to compute likelihood
//  @param ref_histo reference histogram for a player; must have been normalized with normalize_histogram()
//  
//  @return Returns the likelihood of there being a player at location (\a r, \a c) in \a img
//////////////////////////////////////////////////////////////////////////////////////////////
float likelihood_color( IplImage* img, int r, int c, int w, int h, histogram** ref_histo )
{
	IplImage* tmp;
	histogram* histo;
	float d_sq = 0;

	for ( int i = 0; i < 2; i++ )
	{
		/* extract region around (r,c) and compute and normalize its histogram */
		cvSetImageROI( img, cvRect( c - w / 2, r -  ( h / 2 ) * ( 1 - i ), w, h / 2 ) );
		tmp = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 3 );
		cvCopy( img, tmp, NULL );
		cvResetImageROI( img );

		/* compute histogram */
		histo = calc_histogram( &tmp, 1 );
		cvReleaseImage( &tmp );
		normalize_histogram( histo );

		/* compute likelihood as e^{\lambda D^2(h, h^*)} */
		d_sq += histo_dist_sq( histo, ref_histo[i] );
		free( histo );
	}

	return (float)(exp( -LAMBDA * 4 * d_sq / 2 ));
}

float likelihood_decomposed( IplImage** features, int r, int c, int w, int h, model& observation, int chain_index, int nFeature, FILE* info )
{
	int i, j, k;
	float d_sq = 0;
	
	// set ROI
	IplImage* f[MAX_FEATURES], *tmp;
	for ( i = 0; i < nFeature; i++ )
	{
		cvSetImageROI( features[i], cvRect( c - w / 2, r -  h / 2, w, h ) );
		tmp = cvCreateImage( cvGetSize(features[i]), IPL_DEPTH_32F, 1 );
		cvCopy( features[i], tmp, NULL );
		cvResetImageROI( features[i] );

		f[i] = cvCreateImage( cvSize(MW,MH), IPL_DEPTH_32F, 1 );
		cvResize(tmp, f[i] );


		cvReleaseImage(&tmp);
	}

	// matching 
	int width		= MW;
	int height		= MH;

	double val;
	float idx_w;
	int idx_f, idx_i, count;

	int num[MAX_FEATURES];
	double template_flat[MAX_FEATURES][MW*MH], temp_template_flat[MW*MH]; 
	double test_flat[MAX_FEATURES][MW*MH], temp_test_flat[MW*MH];
	
	for ( i = 0; i < nFeature; i++ )
	{
		num[i] = 0;

		for ( j = 0; j < MW*MH; j++ )
		{
			template_flat[i][j] = 0;
			test_flat[i][j]		= 0;
		}
	}

	for ( i = 0; i < observation.chains[chain_index].num; i++ )
	{
		idx_f = observation.chains[chain_index].pc_f[i];
		idx_i = observation.chains[chain_index].pc_i[i];
		idx_w = observation.chains[chain_index].pc_w[i];

		// template features 
		count = 0;
		for ( j = 0; j < height; j++ )
			for ( k = 0; k < width; k++ )
			{
				val = pixval32f(observation.feature[idx_f][idx_i], j, k);	
				temp_template_flat[count++] = val;
			}

		// test features
		count = 0;
		for ( j = 0; j < height; j++ )
			for ( k = 0; k < width; k++ )
			{
				val = pixval32f(f[idx_f], j, k);		
				temp_test_flat[count++] = val;
			}

		num[idx_f]++;
		for ( j = 0; j < width*height; j++ )
		{
			template_flat[idx_f][j] += temp_template_flat[j];
			test_flat[idx_f][j]		+= temp_test_flat[j];
		}
	}

	count = 0;
	for ( i = 0; i < nFeature; i++ )
	{
		if ( num[i] > 0 )
		{
			count++;

			for ( j = 0; j < width*height; j++ )
			{
				temp_template_flat[j] = template_flat[i][j] / float(num[i]);
				temp_test_flat[j]	  = test_flat[i][j] / float(num[i]);
			}

			dd_dist	dd;
			float temp = dd.dd2D(temp_template_flat, temp_test_flat, width, height) / float( width * height );
			d_sq +=  temp;

			if ( info )
				fprintf(info, "%d (%f) ",i,temp);
		}
	}
	
	d_sq =  d_sq / float(count);

	for ( i = 0; i < nFeature; i++ )
		cvReleaseImage(&f[i]);

	return (float)(exp( -LAMBDA * d_sq ));
}

void extract_feature(IplImage* image, IplImage** f, int update, int nFeature)
{
	int j, k;
	int count;
	double val;

	int width		= image->width;
	int height		= image->height;
	int linear_size	= height * width;

	IplImage*h, *s, *v, *e;

	// 1. feature extraction
	h = cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );
	s = cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );
	v = cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );
	e = cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );

	// (1) hue, (2) saturation, (3) value  
	cvCvtPixToPlane( image, h, s, v, NULL );

	// normalization
	cvConvertScale( h, h, 1.0f/360.0f, 0 );

	// (4) edge
	double* i_linear		= (double*)malloc(linear_size * sizeof(double)); // Input
	double* e_linear		= (double*)malloc(linear_size * sizeof(double)); // output
	double* c_linear		= (double*)malloc(linear_size * sizeof(double)); // output

	try
	{
		// input 
		count = 0;
		for ( j = 0; j < width; j++ )
			for ( k = 0; k < height; k++ )
			{
				val =  pixval32f( v, k, j );
				i_linear[count++] = val;
			}

		mwArray im(height, width, mxDOUBLE_CLASS, mxREAL); 
		im.SetData(i_linear, linear_size);

		// output
		mwArray M;			// Maximum moment of phase congruency covariance. This is used as a indicator of edge strength.
		mwArray m;			// Minimum moment of phase congruency covariance. This is used as a indicator of corner strength.
		mwArray or0;		// Orientation image in integer degrees 0-180, positive anticlockwise. 0 corresponds to a vertical edge, 90 is horizontal.
		mwArray featType;	// *Not correctly implemented at this stage*
		mwArray PC;			// Cell array of phase congruency images (values between 0 and 1)  for each orientation
		mwArray EO;			//  A 2D cell array of complex valued convolution results

		// compute edge and corner phase congruency in an image
		phasecong2(6, M, m, or0, featType, PC, EO, im);

		M.GetData(e_linear, linear_size);
	}
	catch (const mwException& e)
	{
		AfxMessageBox(_T("SPCA error"));
		return;
	}

	// edge strength image
	count = 0;
	for ( j = 0; j < width; j++ )
		for ( k = 0; k < height; k++ )
		{
			setpix32f(e, k, j, e_linear[count++]);
			val = pixval32f(e, k, j);
		}

	cvDilate(v,v);
	cvDilate(e,e);
	cvDilate(e,e); 

	free(i_linear);
	free(e_linear);
	free(c_linear);

	// 2. description 
	if ( nFeature == 2 )
	{
		f[0]	= cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 ); 
		f[1]	= cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );

		cvCopy( v, f[0] );
		cvCopy( e, f[1] );
	}
	else
	{	
		f[0]	= cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 ); 
		f[1]	= cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );
		f[2]	= cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );
		f[3]	= cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );

		cvCopy( h, f[0] );
		cvCopy( s, f[1] );
		cvCopy( v, f[2] );
		cvCopy( e, f[3] );
	}

	cvReleaseImage(&h);
	cvReleaseImage(&s);
	cvReleaseImage(&v);
	cvReleaseImage(&e);
}

void make_observation_model(IplImage** features, int r, int c, int w, int h,int queue, int update, int nFeature, model& observation)
{
	int i, j, k, l;
	int count;
	double val;

	// set ROI
	IplImage* f[MAX_FEATURES], *tmp;
	for ( i = 0; i < nFeature; i++ )
	{
		cvSetImageROI( features[i], cvRect( c - w / 2, r -  h / 2, w, h ) );
		tmp = cvCreateImage( cvGetSize(features[i]), IPL_DEPTH_32F, 1 );
		cvCopy( features[i], tmp, NULL );
		cvResetImageROI( features[i] );

		f[i] = cvCreateImage( cvSize(MW,MH), IPL_DEPTH_32F, 1 );
		cvResize(tmp, f[i] );

		cvReleaseImage(&tmp);
	}

	// 3. SPCA
	int width		= MW;
	int height		= MH;
	int linear_size	= height * width;

	for ( l = 0; l < nFeature; l++ )
	{
		if ( observation.feature[l][queue] )
			cvReleaseImage(&observation.feature[l][queue]);
		observation.feature[l][queue] = f[l];
	}

	if ( update == 1 )  // initial
	{
		for ( l = 0; l < MAX_CHAINS; l++ )
		{
			int type = rand() % nFeature;

			observation.chains[l].pc_f[0]	= type; // choose type of feature 
			observation.chains[l].pc_i[0]	= 0;	// choose index in the feature
			observation.chains[l].pc_w[0]	= 1.0f;

			observation.chains[l].num		= 1;
		}
	}
	else if ( update % MAX_UPDATE == 0 ) // update
	{
		int max_index;
		if ( update < MAX_OSBS ) 
			max_index =  queue + 1;
		else
			max_index = MAX_OSBS; 

		// making data matrix 
		CvMat* data	= cvCreateMat(width * height, max_index * nFeature, CV_64FC1);

		int row, col;
		for ( i = 0; i < max_index; i++ )
		{
			for ( l = 0; l < nFeature; l++ )
			{
				for ( j = 0; j < height; j++ )
					for ( k = 0; k < width; k++ )
					{
						row = j * width + k;
						col = i + max_index * l;

						val = pixval32f(observation.feature[l][i], j, k);	
						data->data.db[row * data->width + col ]	= val;
					}
			}
		}

		// normalization
		CvMat* norm = cvCreateMat(data->height, 1, CV_64FC1);
		for ( j = 0; j < data->width; j++ )
		{
			for ( i = 0; i < data->height; i++ )
				norm->data.db[i] = data->data.db[i * data->width + j];

			float val = cvNorm(norm);
			for ( i = 0; i < data->height; i++ )
				data->data.db[i * data->width + j] = data->data.db[i * data->width + j] / val;
		}
		cvReleaseMat(&norm);

		/*float val = cvNorm(data);
		for ( j = 0; j < data->width; j++ )
			for ( i = 0; i < data->height; i++ )
				data->data.db[i * data->width + j] = data->data.db[i * data->width + j] / val;*/

		// make covariance matrix 
		CvMat* S = cvCreateMat(data->width, data->width, CV_64FC1);
		cvMulTransposed(data,S,1);

		count = 0;
		double* S_linear = (double*)malloc(data->width*data->width*sizeof(double)); // Input
		for ( i = 0; i < data->width; i++ )
			for ( j = 0; j < data->width; j++ )
				S_linear[count++] = S->data.db[j * data->width + i];

		// SPCA
		int maxiter			= 3000;		// Maximum Number of Iterations
		int info			= 100;		// Control amount of output

		double* X_linear = (double*)malloc(data->width*data->width*sizeof(double));
		double* x_linear = (double*)malloc(data->width*1*sizeof(double));
		int* k_linear = (int*)malloc(1*sizeof(int));

		try
		{
			// input
			mwArray covmtx(data->width, data->width, mxDOUBLE_CLASS, mxREAL); 
			covmtx.SetData(S_linear, data->width * data->width);
			
			// Penalty factor controlling sparsity (large rho => sparse sol)
			double n_rho[1];
			n_rho[0] = RHO;		
			mwArray rho(1, 1, mxDOUBLE_CLASS, mxREAL); 
			rho.SetData(n_rho, 1*1);

			// Target precision 
			double n_gapchange[1];
			n_gapchange[0] = GAP;
			mwArray gapchange(1, 1, mxDOUBLE_CLASS, mxREAL); 
			gapchange.SetData(n_gapchange, 1*1);

			// Maximum Number of Iterations
			int n_maxiter[1];
			n_maxiter[0] = maxiter;	
			mwArray maxiter(1, 1, mxINT16_CLASS, mxREAL); 
			maxiter.SetData(n_maxiter, 1*1);
		
			 // Control amount of output
			int n_info[1];
			n_info[0] = info;     
			mwArray info(1, 1, mxINT16_CLASS, mxREAL); 
			info.SetData(n_info, 1*1);

			int n_algo[1];
			n_algo[0] = 3;     
			mwArray algo(1, 1, mxINT16_CLASS, mxREAL); 
			algo.SetData(n_algo, 1*1);

			// output
			mwArray U;			// symmetric matrix that solves the above SDP 
			mwArray X;			// dual variable, solves the dual SDP 
			mwArray x;			// largest eigenvector of U 
			mwArray F;			// Average gradient
			mwArray k;			// number of iterations run
			mwArray dualitygap; // vector of duality gaps at designated iterations
			mwArray cputime;    // vector of cumulative cpu times at designated iterations
			mwArray perceigs;   // vector of percentage of eigenvalues used (in partial eigenvalue decomposition) at designated iterations

			// run SPCA
			DSPCA(8, U, X, x, F, k, dualitygap, cputime, perceigs, covmtx ,rho ,gapchange ,maxiter, info, algo);

			x.GetData(x_linear, data->width * 1);
			X.GetData(X_linear, data->width*data->width);
			k.GetData(k_linear, 1);

		}
		catch (const mwException& e)
		{
			AfxMessageBox(_T("error"));
			return;
		}

		// output
		CvMat* X = cvCreateMat(data->width, data->width, CV_64FC1);

		count = 0;
		for ( i = 0; i < data->width; i++ )
			for ( j = 0; j < data->width; j++ )
				X->data.db[j * data->width + i] = X_linear[count++];

		CvMat* evects = cvCreateMat(data->width, data->width, CV_64FC1);
		CvMat* evals = cvCreateMat(data->width, 1, CV_64FC1);
		cvEigenVV( X, evects, evals);

		cvTranspose(evects, evects);
	
		int idx, idx_f, idx_i;
		for ( i = 0; i < MAX_CHAINS; i++ )
		{
			idx = 0;

			for ( j = 0; j < data->width; j++ )
			{
				idx_f = j / max_index;
				idx_i = j % max_index;
				val = fabsf(evects->data.db[j * data->width + i]);

				if ( val > THRES || idx_i == 0 )
				//if ( idx_f == i )
				{
					observation.chains[i].pc_f[idx] = idx_f;
					observation.chains[i].pc_i[idx] = idx_i;
					
					observation.chains[i].pc_w[idx]	= val;

					idx++;
				}
			}

			observation.chains[i].num = idx;
		}

		free(S_linear);
		free(x_linear);
		free(X_linear);
		free(k_linear);

		cvReleaseMat(&data);
		cvReleaseMat(&S);
	}
	else
	{
		// nothing
	}
}
