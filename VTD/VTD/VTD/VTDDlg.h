
// VTDDlg.h : header file
//

#pragma once

#include "Track.h"

// CVTDDlg dialog
class CVTDDlg : public CDialogEx
{
// Construction
public:
	CVTDDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VTD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CString strFilePath;
	CString strFileName;

	CTrack cTrack;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpenVideo();
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonOk();
};
