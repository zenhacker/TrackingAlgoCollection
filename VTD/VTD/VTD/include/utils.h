//////////////////////////////////////////////////////////////////////////////////////////////
// This file contains prototypes for miscellaneous utility functions.
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_H
#define UTILS_H

#include "../include/defs.h"

/**************************** Function Prototypes ****************************/



//////////////////////////////////////////////////////////////////////////////////////////////
//
// A function to get a pixel value from a 32-bit floating-point image.
//   
// @param img an image
// @param r row
// @param c column
// @return Returns the value of the pixel at (\a r, \a c) in \a img
//////////////////////////////////////////////////////////////////////////////////////////////
extern inline float pixval32f( IplImage* img, int r, int c );


//////////////////////////////////////////////////////////////////////////////////////////////
//
// A function to set a pixel value in a 32-bit floating-point image.
//   
// @param img an image
// @param r row
// @param c column
// @param val pixel value
//////////////////////////////////////////////////////////////////////////////////////////////

extern inline void setpix32f( IplImage* img, int r, int c, float val );


//////////////////////////////////////////////////////////////////////////////////////////////
//
//  A function to print an error message then exit the program.
// 
// @param format an error message format string (as with \c printf(3)).
//////////////////////////////////////////////////////////////////////////////////////////////

extern void fatal_error( char* format, ... );


//////////////////////////////////////////////////////////////////////////////////////////////
//
//   A function to replace a file's extension.  
//
//   @param file the name of a file
//   @param new_extn a new file extension for \a file.  
//   a new_extn should not include a dot (i.e. \c "jpg", not \c ".jpg") 
//   unless the new file extension should contain two dots.
//   
//   @return Returns a file with its extension replaced.  
//   If a file does not have an extension, this function simply adds one.
//////////////////////////////////////////////////////////////////////////////////////////////

extern char* replace_extension( const char* file, const char* new_extn );


//////////////////////////////////////////////////////////////////////////////////////////////
//
//   A function to prepend a path to a filename.
//
//   @param path a path
//   @param file a file name
//   @return Returns a full path name with a path prepended to a file.
//////////////////////////////////////////////////////////////////////////////////////////////

extern char* prepend_path( const char* path, const char* file );


//////////////////////////////////////////////////////////////////////////////////////////////
//
//   A function to determine whether a file is an image file.  
//   The determination is made based solely on the file's extension.  
//   Files with the following extensions are considered image files: 
//   .\c png, .\c jpg, .\c jpeg, .\c pbm, .\c pgm, .\c ppm, .\c bmp, .\c tif, .\c tiff.
//
//   @param file a file name
//   @return Returns TRUE if a file has one of the extensions listed above, and FALSE otherwise.
//////////////////////////////////////////////////////////////////////////////////////////////

extern int is_image_file( char* file );


//////////////////////////////////////////////////////////////////////////////////////////////
//   A function to draw an x on an image.
//
//   @param img an image
//   @param pt the center point of the x
//   @param r the x's radius
//   @param w the x's line weight
//   @param color the color of the x
//////////////////////////////////////////////////////////////////////////////////////////////

extern void draw_x( IplImage* img, CvPoint pt, int r, int w, CvScalar color );


//////////////////////////////////////////////////////////////////////////////////////////////
//
//   A function to denote progress in the console with a spinning pinwheel.
//   Every time this function is called, the state of the pinwheel is incremented.  
//   The pinwheel has four states that loop indefinitely: | / - \.
//   
//   @param done if FALSE, this function simply increments the state of the pinwheel; 
//               if TRUE, prints "done" rather than incrementing the pinwheel.
//////////////////////////////////////////////////////////////////////////////////////////////

extern void progress( int done );


//////////////////////////////////////////////////////////////////////////////////////////////
//
//   A function to erase a certain number of characters from a stream.
//   
//   @param stream the stream from which to erase characters
//   @param n the number of characters to erase
//////////////////////////////////////////////////////////////////////////////////////////////

extern void erase_from_stream( FILE* stream, int n );


//////////////////////////////////////////////////////////////////////////////////////////////
//
//   A function that doubles the size of an array with error checking
//
//   @param array pointer to an array whose size is to be doubled
//   @param n number of elements allocated for a array
//   @param size size of elements in a array
//
//   @return Returns the new number of elements allocated for \a array.  
//   If no memory was available, returns 0 and sets \c errno to ENOMEM.
//////////////////////////////////////////////////////////////////////////////////////////////

extern int array_double( void** array, int n, int size );

#endif
