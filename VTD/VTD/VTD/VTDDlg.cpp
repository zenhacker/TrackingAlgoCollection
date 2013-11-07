
// VTDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VTD.h"
#include "VTDDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVTDDlg dialog




CVTDDlg::CVTDDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVTDDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVTDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVTDDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_VIDEO, &CVTDDlg::OnBnClickedButtonOpenVideo)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CVTDDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CVTDDlg::OnBnClickedButtonOk)
END_MESSAGE_MAP()


// CVTDDlg message handlers

BOOL CVTDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVTDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVTDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVTDDlg::OnBnClickedButtonOpenVideo()
{
	CFileDialog dFileDlg(TRUE);
	int nRet = dFileDlg.DoModal();

	if ( nRet == IDOK )
	{
		strFilePath = dFileDlg.GetPathName();
		strFileName = dFileDlg.GetFileName();
	}
	else
	{
		strFilePath = "";
		strFileName = "";
	}
}


void CVTDDlg::OnBnClickedButtonRun()
{
	if ( strFilePath !="" )
	{
		cTrack.RunTracking(strFileName, strFilePath);
	}
	else
	{
		AfxMessageBox(_T("open the video file fisrt or choose mode!!"));
	}
}


void CVTDDlg::OnBnClickedButtonOk()
{
	CDialog::OnOK();
}
