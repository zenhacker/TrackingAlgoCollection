/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING

By downloading, copying, installing or using the code, 
you agree with the following license claims:

COPYRIGHT (C) 2006, 2007, HAIBIN LING AND KAZUNORI OKADA, ALL RIGHTS RESERVED.

REDISTRIBUTION AND USE IN SOURCE AND BINARY FORMS, WITH OR WITHOUT MODIFICATION,
ARE PERMITTED FOR NON-PROFIT RESEARCH USE ONLY, PROVIDED THAT THE FOLLOWING 
CONDITIONS ARE MET:

REDISTRIBUTION'S OF SOURCE CODE MUST RETAIN THE ABOVE COPYRIGHT NOTICE,
THIS LIST OF CONDITIONS AND THE FOLLOWING DISCLAIMER.

REDISTRIBUTION'S IN BINARY FORM MUST REPRODUCE THE ABOVE COPYRIGHT NOTICE,
THIS LIST OF CONDITIONS AND THE FOLLOWING DISCLAIMER IN THE DOCUMENTATION
AND/OR OTHER MATERIALS PROVIDED WITH THE DISTRIBUTION.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

If you do not agree to this license agreement, 
do not download, install, copy or use the software. 

------------------------------------------------------------------------------------------------------
This code implements the diffusion-based distance algorithm published in the following paper

	H. Ling and K. Okada, 
	Diffusion Distance for Histogram Comparison,
	IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 
	Vol. I, pp. 246-253, 2006. 

This code is a RE-IMPLEMENTATION of the diffusion-based distance according to the above paper. 
	There is no guarantee of exact reproduction of the experimental results reported in the paper. 
	In addition, the method is licensed by 
		
	Siemens Corporate Research, Inc.
	755 College Road East
	Princeton, NJ 08540-6632
	Telephone: (609) 734-6500
	Fax: (609) 734-6565

	The usage of this code is restricted for non-profit research usage only and
	using of the code is at the user's risk.

------------------------------------------------------------------------------------------------------
NOTES:
1. Histogram matrices are assumed to be arranged in the "matlab" style, i.e, the [i,j,k]-th element is 
	located in the position
		i*(n2*n3) + j*(n3) + k

2. The example usage of the code is demonstrated in 
		dd_test.cpp

Author Contact Information:
	Haibin Ling (hbling at cs dot ucla dot edu)
	Kazunori Okada (kazokada at sfsu dot edu)
	09/23/07

*/

#include "stdio.h"
#include "stdlib.h"
#include "../include/dd_head.h" 


///////////////////////////////////////////////////////////////////////////////////////////
// Construction, deconstruction 
///////////////////////////////////////////////////////////////////////////////////////////
dd_dist::dd_dist(int nSize1, int nSize2, int nSize3)
{
	SetDimension(nSize1,nSize2,nSize3);
}

dd_dist::~dd_dist()
{
}


///////////////////////////////////////////////////////////////////////////////////////////
//			Diffusion-based distance
///////////////////////////////////////////////////////////////////////////////////////////

// diffision-based distance between two 2d hstograms of size n1 X n2
double dd_dist::dd2D(double* h1, double* h2, int n1, int n2)
{
	if(n1!=m_nSize[0] || n2!=m_nSize[1])	
		SetDimension(n1,n2);

	//- compute the difference 
	double	*p1	= h1;
	double	*p2	= h2;
	int		x,y;
	for(y=0; y<n1; ++y)	
		for(x=0; x<n2; ++x)	
			m_dDiff2D[y][x]	= *(p1++)-*(p2++);

	//- compute the distance recursively
	double	dDist	= L1_dist(n1,n2);
	while(n1>1 && n2>1)	{
		SmoothDiff2D(n1,n2);			// smooth
		DownsampleDiff2D(n1,n2);		// downsampling
		dDist	+= L1_dist(n1,n2);		// distance
	}
	return dDist;
}


// diffision-based distance between two 3d hstograms of size n1 X n2 X n3
double dd_dist::dd3D(double* h1, double* h2, int n1, int n2, int n3)
{
	if(n1!=m_nSize[0] || n2!=m_nSize[1] || n3!=m_nSize[2])
		SetDimension(n1,n2,n3);

	//- compute the difference 
	double	*p1	= h1;
	double	*p2	= h2;
	int		x,y,z;
	for(z=0; z<n1; ++z)
		for(y=0; y<n2; ++y)	
			for(x=0; x<n3; ++x)	
				m_dDiff3D[z][y][x]	= *(p1++)-*(p2++);

	//- compute the distance recursively
	double	dDist	= L1_dist(n1,n2,n3);
	while(n1>1 && n2>1 && n3>1)
	{
		SmoothDiff3D(n1,n2,n3);			// smooth
		DownsampleDiff3D(n1,n2,n3);		// downsampling
		dDist	+= L1_dist(n1,n2,n3);	// distance
	}
	return dDist;
}


////////////////////////////////////////////////////////////////////////////////////////////
// n1,n2,n3 are the dimensions for histogram, a histogam matrix is assume to be arranged in the 
// "matlab" style, that is, the [i,j,k]-th element is located in the position
//		i*(n2*n3) + j*(n3) + k
//		
// n1,n2 are dimensions for y,x if n3==0
// or 
// n1,n2,n3 are dimensions for z,y,x if n3!=0
//
bool dd_dist::SetDimension(int n1, int n2, int n3/*=0*/)
{	
	m_nSize[0]=n1; m_nSize[1]=n2; m_nSize[2]=n3;
	m_nDim	= n3>0 ? 3 : 2; 

	if(m_nDim==2) {
		m_dDiff2D.resize(n1);
		m_dTmp2D.resize(n1);
		for(int y=0; y<n1; ++y)	{
			m_dDiff2D[y].resize(n2);
			m_dTmp2D[y].resize(n2);
		}
	}
	else if(m_nDim==3) {
		m_dDiff3D.resize(n1);
		m_dTmp3D.resize(n1);
		for(int z=0; z<n1; ++z)	{
			m_dDiff3D[z].resize(n2);
			m_dTmp3D[z].resize(n2);
			for(int y=0; y<n2; ++y)	{
				m_dDiff3D[z][y].resize(n3);
				m_dTmp3D[z][y].resize(n3);
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
void dd_dist::DownsampleDiff2D(int& n1, int &n2)
{
	int		x,y;
	n1	= n1/2;
	for(y=0;y<n1;++y) 
	for(x=0;x<n2;++x)
		m_dDiff2D[y][x]	= m_dDiff2D[2*y][x]+m_dDiff2D[2*y+1][x];

	n2	= n2/2;
	for(y=0;y<n1;++y) 
	for(x=0;x<n2;++x)
		m_dDiff2D[y][x]	= m_dDiff2D[y][2*x]+m_dDiff2D[y][2*x+1];
}


void dd_dist::DownsampleDiff3D(int &n1, int &n2, int &n3)
{
	int	x,y,z;
	n1	= n1/2;
	for(z=0;z<n1;++z)
	for(y=0;y<n2;++y) 
	for(x=0;x<n3;++x)
		m_dDiff3D[z][y][x]	= m_dDiff3D[2*z][y][x]+m_dDiff3D[2*z+1][y][x];

	n2	= n2/2;
	for(z=0;z<n1;++z)
	for(y=0;y<n2;++y) 
	for(x=0;x<n3;++x)
		m_dDiff3D[z][y][x]	= m_dDiff3D[z][2*y][x]+m_dDiff3D[z][2*y+1][x];

	n3	= n3/2;
	for(z=0;z<n1;++z)
	for(y=0;y<n2;++y) 
	for(x=0;x<n3;++x)
		m_dDiff3D[z][y][x]	= m_dDiff3D[z][y][2*x]+m_dDiff3D[z][y][2*x+1];
}

/*-----------------------------------------------------------------------*/
void dd_dist::SmoothDiff2D(int n1, int n2)
{
	const double F00= 0.0113;	const double F01= 0.0838;	const double F02= 0.0113;
	const double F10= 0.0838;	const double F11= 0.6193;	const double F12= 0.0838;
	const double F20= 0.0113;	const double F21= 0.0838;	const double F22= 0.0113;

	double	d4	= 1/(F00+F01+F10+F11);
	double	d6	= 1/(F00+F01+F10+F11+F20+F21);
	int		x,y;
	for(x=1; x<n2-1; ++x) {
		for(y=1; y<n1-1; ++y) {
			m_dTmp2D[y][x]= F00*m_dDiff2D[y-1][x-1] + F01*m_dDiff2D[y-1][x] + F02*m_dDiff2D[y-1][x+1] + 
							F10*m_dDiff2D[y][x-1]   + F11*m_dDiff2D[y][x]   + F12*m_dDiff2D[y][x+1] + 
							F20*m_dDiff2D[y+1][x-1] + F21*m_dDiff2D[y+1][x] + F22*m_dDiff2D[y+1][x+1];
			
			// column boundaries
			m_dTmp2D[y][0]	  = (F01*m_dDiff2D[y-1][0] + F02*m_dDiff2D[y-1][1] + 
								 F11*m_dDiff2D[y][0]   + F12*m_dDiff2D[y][1] + 
								 F21*m_dDiff2D[y+1][0] + F22*m_dDiff2D[y+1][1]) * d6;
			m_dTmp2D[y][n2-1] = (F00*m_dDiff2D[y-1][n2-2] + F01*m_dDiff2D[y-1][n2-1] + 
						 		 F10*m_dDiff2D[y][n2-2]   + F11*m_dDiff2D[y][n2-1] + 
								 F20*m_dDiff2D[y+1][n2-2] + F21*m_dDiff2D[y+1][n2-1]) * d6;
		}

		// row boundaries
		m_dTmp2D[0][x]	  = (F10*m_dDiff2D[0][x-1] + F11*m_dDiff2D[0][x] + F12*m_dDiff2D[0][x+1] + 
							 F20*m_dDiff2D[1][x-1] + F21*m_dDiff2D[1][x] + F22*m_dDiff2D[1][x+1]) * d6;
		m_dTmp2D[n1-1][x] = (F00*m_dDiff2D[n1-2][x-1] + F01*m_dDiff2D[n1-2][x] + F02*m_dDiff2D[n1-2][x+1] + 
							 F10*m_dDiff2D[n1-1][x-1] + F11*m_dDiff2D[n1-1][x] + F12*m_dDiff2D[n1-1][x+1]) * d6;
	}

	// corners
	m_dTmp2D[0][0]= (F11*m_dDiff2D[0][0] + F12*m_dDiff2D[0][1] +			
					 F21*m_dDiff2D[1][0] + F22*m_dDiff2D[1][1])*d4;						// left-bottom
	m_dTmp2D[0][n2-1] = (F10*m_dDiff2D[0][n2-2] + F11*m_dDiff2D[0][n2-1] + 
						 F20*m_dDiff2D[1][n2-2] + F21*m_dDiff2D[1][n2-1])*d4;			// right-bottom
	m_dTmp2D[n1-1][n2-1] = (F00*m_dDiff2D[n1-2][n2-2] + F01*m_dDiff2D[n1-2][n2-1] + 
							F10*m_dDiff2D[n1-1][n2-2] + F11*m_dDiff2D[n1-1][n2-1])*d4;	// right-top
	m_dTmp2D[n1-1][0] = (F01*m_dDiff2D[n1-2][0] + F02*m_dDiff2D[n1-2][1] + 
						 F11*m_dDiff2D[n1-1][0] + F12*m_dDiff2D[n1-1][1])*d4;			// left-top

	// copy result
	for(y=0; y<n1; ++y)	
	for(x=0; x<n2; ++x)	
		m_dDiff2D[y][x]	= m_dTmp2D[y][x];
}


/*-----------------------------------------------------------------------*/
void dd_dist::SmoothDiff3D(int n1, int n2, int n3)
{
	// prepare 3D gaussian filter
	const double G[3][3][3] = 	
	{  {{0.0012, 0.0089, 0.0012}, 
		{0.0089, 0.0660, 0.0089},
		{0.0012, 0.0089, 0.0012}},
	   {{0.0089, 0.0660, 0.0089}, 
   		{0.0660, 0.4874, 0.0660},
   		{0.0089, 0.0660, 0.0089}},
	   {{0.0012, 0.0089, 0.0012}, 
 		{0.0089, 0.0660, 0.0089},
		{0.0012, 0.0089, 0.0012}}
	};   

	int x,y,z, x1,y1,z1;
	for(z=0;z<n1;++z)
	for(y=0;y<n2;++y)
	for(x=0;x<n3;++x)
	{
		m_dTmp3D[z][y][x]	= 0;
		for(z1=-1;z1<2;++z1)
		for(x1=-1;x1<2;++x1)
		for(y1=-1;y1<2;++y1)
			if(	z+z1>=0 && z+z1<n1 && y+y1>=0 && y+y1<n2 && x+x1>=0 && x+x1<n3 )
				m_dTmp3D[z][y][x] += G[z1+1][y1+1][x1+1]*m_dDiff3D[z+z1][y+y1][x+x1];
	}

	// copy result
	for(z=0;z<n1;++z)
	for(y=0;y<n2;++y)
	for(x=0;x<n3;++x)
		m_dDiff3D[z][y][x]	= m_dTmp3D[z][y][x];
}

