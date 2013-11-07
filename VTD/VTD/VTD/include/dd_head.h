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

#ifndef dd_H
#define	dd_H

#include <vector>
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////////
// Class of Diffusiont Distance
/////////////////////////////////////////////////////////////////////////////////////////////
class dd_dist
{
public:
	dd_dist(int nSize1=0, int nSize2=0, int nSize3=0);
	~dd_dist();

	double	dd2D(double* h1, double* h2, int n1, int n2);		// two 2d hstograms of size n1 X n2
	double	dd3D(double* h1, double* h2, int n1, int n2, int n3);	// two 3d hstograms of size n1 X n2 X n3
	bool	SetDimension(int n1, int n2, int n3=-1);

private:
	void	DownsampleDiff2D(int& n1, int &n2);
	void	DownsampleDiff3D(int& n1, int &n2, int& n3);
	void	SmoothDiff2D(int n1, int n2);
	void	SmoothDiff3D(int n1, int n2, int n3);
	inline double	L1_dist(int n1, int n2, int n3=0);

private:
	std::vector<std::vector<double> >	m_dDiff2D;
	std::vector<std::vector<double> >	m_dTmp2D;
	std::vector<std::vector<std::vector<double> > >	m_dDiff3D;
	std::vector<std::vector<std::vector<double> > >	m_dTmp3D;

	int		m_nDim;
	int		m_nSize[3];
};


/////////////////////////////////////////////////////////////////////////////////////////////
// Inline functions
/////////////////////////////////////////////////////////////////////////////////////////////
inline double dd_dist::L1_dist(int n1, int n2, int n3/*=0*/)
{
	int x,y,z;
	double d	= 0;
	if(m_nDim==2) {
		for(y=0;y<n1;++y)
			for(x=0;x<n2;++x)
				d	+= fabs(m_dDiff2D[y][x]);
	}
	else {
		for(z=0;z<n1;++z)
			for(y=0;y<n2;++y)
				for(x=0;x<n3;++x)
					d	+= fabs(m_dDiff3D[z][y][x]);
	}
	return d;
}


#endif
