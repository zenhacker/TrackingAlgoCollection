// Track.h: interface for the CTrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACK_H__E7EFF2ED_BD45_4C22_A733_A1AA3E3CE4EC__INCLUDED_)
#define AFX_TRACK_H__E7EFF2ED_BD45_4C22_A733_A1AA3E3CE4EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "include/defs.h"
#include "include/utils.h"
#include "include/particles.h"
#include "include/observation.h"

class CTrack  
{
public:
	void RunTracking(CString filename, CString filepath);

	CTrack();
	virtual ~CTrack();

protected:
	char* pname;			/* program name */
	char* vid_file;			/* input video file name */
	int show_all;			/* TRUE to display all particles */
	int export_img;			/* TRUE to exported tracking sequence */
	int export_vid;		
	
	int interaction;
	BOOL PeekAndPump();
};

#endif // !defined(AFX_TRACK_H__E7EFF2ED_BD45_4C22_A733_A1AA3E3CE4EC__INCLUDED_)
