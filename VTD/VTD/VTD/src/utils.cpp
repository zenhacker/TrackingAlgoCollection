////////////////////////////////////////////////////////////////////////////
//  utils.c
//
//  This file contains definitions of miscellaneous utility functions.
////////////////////////////////////////////////////////////////////////////


#include "../include/utils.h"
#include "../include/defs.h"


/***************************** Inline Functions ******************************/

/* returns a pixel value from a 32-bit floating point image */
inline float pixval32f(IplImage* img, int r, int c)
{
	return ( (float*)(img->imageData + img->widthStep*r) )[c];
}


/* sets a pixel value in a 64-bit floating point image */
inline void setpix32f(IplImage* img, int r, int c, float val)
{
	( (float*)(img->imageData + img->widthStep*r) )[c] = val;
}

/*************************** Function Definitions ****************************/

////////////////////////////////////////////////////////////////////////////
//
// fatal_error() prints the error message contained in format 
// then exits with exit code 1.
////////////////////////////////////////////////////////////////////////////

void fatal_error(char* format, ...)
{
	va_list ap;  // format argument list

	fprintf( stderr, "Error: ");

	// assemble argument list and print format using it
	va_start( ap, format );
		vfprintf( stderr, format, ap );
	va_end(ap);
	
	printf("\n");
	exit(1);
}



////////////////////////////////////////////////////////////////////////////
// replace_extension() replaces file's extension with new_extn and returns the result.  
// new_extn should not include a dot unless the new extension should contain two dots.
////////////////////////////////////////////////////////////////////////////

char* replace_extension(const char* file, const char* new_extn)
{
	char* new_file = (char*)malloc( (strlen(file)+strlen(new_extn)+2) * sizeof(char) );
	char* lastdot;   // location of extension

	// copy filename into new_file and find location of extension
	strcpy( new_file, file );
	lastdot = strrchr( new_file, '.' );

	// if file has an extension, cut it off, then append new extension
	if( lastdot != NULL )
		*(lastdot + 1) = '\0';
	else
		strcat( new_file, "." );
	strcat( new_file, new_extn );

	return new_file;
}


////////////////////////////////////////////////////////////////////////////
//
//  prepend_path() creates a full pathname by prepending path to file. 
//  Returns the full pathname.
////////////////////////////////////////////////////////////////////////////

char* prepend_path(const char* path, const char* file)
{
	int name_length = strlen(path) + strlen(file) + 2;
	char* pathname = (char*)malloc( name_length * sizeof(char) );

	_snprintf( pathname, name_length, "%s/%s", path, file );

	return pathname;
}

////////////////////////////////////////////////////////////////////////////
//
//  is_image_file() returns TRUE if file represents an image file and FALSE otherwise.  
//  The decision is made based on the file extension.
////////////////////////////////////////////////////////////////////////////

int is_image_file(char* file)
{
	// find location of file extension
	char* lastdot = strrchr(file, '.');

	if( ! lastdot )
		return FALSE;

	// if file has an image extension, return TRUE
	if( ( strcmp(lastdot, ".png") == 0 ) ||
	  ( strcmp(lastdot, ".jpg") == 0 ) ||
	  ( strcmp(lastdot, ".jpeg") == 0 ) ||
	  ( strcmp(lastdot, ".pbm") == 0 ) ||
	  ( strcmp(lastdot, ".pgm") == 0 ) ||
	  ( strcmp(lastdot, ".ppm") == 0 ) ||
	  ( strcmp(lastdot, ".bmp") == 0 ) ||
	  ( strcmp(lastdot, ".tif") == 0 ) ||
	  ( strcmp(lastdot, ".tiff") == 0 ) )
	{
		return TRUE;
	}

	// otherwise return FASLE
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////
//
// draw_x() draws an X at point pt on image img.  
// The X has radius r, weight w, and color c.
////////////////////////////////////////////////////////////////////////////

void draw_x(IplImage* img, CvPoint pt, int r, int w, CvScalar color)
{
	cvLine( img, pt, cvPoint(pt.x+r, pt.y+r), color, w, 8, 0 );
	cvLine( img, pt, cvPoint(pt.x-r, pt.y+r), color, w, 8, 0 );
	cvLine( img, pt, cvPoint(pt.x+r, pt.y-r), color, w, 8, 0 );
	cvLine( img, pt, cvPoint(pt.x-r, pt.y-r), color, w, 8, 0 );
}

////////////////////////////////////////////////////////////////////////////
// 
//  progress() draws a twirling progress thing (|, /, -, \, |) in the console.
//  If done is TRUE, prints "done", otherwise, increments the progress thing.
////////////////////////////////////////////////////////////////////////////

void progress(int done)
{
	static int state = -1;

	if( state == -1 )
	fprintf( stdout, "  " );

	if( done )
	{
	  fprintf( stdout, "\b\bdone\n");
	  state = -1;
	}
	else
	{
		switch( state )
		{
			case 0:
				fprintf( stdout, "\b\b| ");
				break;

			case 1:
				fprintf( stdout, "\b\b/ ");
				break;

			case 2:
				fprintf( stdout, "\b\b- ");
				break;

			default:
				fprintf( stdout, "\b\b\\ ");
				break;
		}
  
		fflush(stdout);
		state = (state + 1) % 4;
	}
}



////////////////////////////////////////////////////////////////////////////
// 
//  A function to erase a certain number of characters from a stream.
//
//  @param stream the stream from which to erase characters
//  @param n the number of characters to erase
////////////////////////////////////////////////////////////////////////////

void erase_from_stream( FILE* stream, int n )
{
	int j;
	for( j = 0; j < n; j++ )
		fprintf( stream, "\b" );
	for( j = 0; j < n; j++ )
		fprintf( stream, " " );
	for( j = 0; j < n; j++ )
		fprintf( stream, "\b" );
}


