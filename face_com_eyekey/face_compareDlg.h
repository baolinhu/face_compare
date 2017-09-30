
// face_compareDlg.h : 头文件
//

#pragma once

#include "cJSON.h"
#include"CvvImage.h"
#include"cv.h"
// Cface_compareDlg 对话框
class Cface_compareDlg : public CDialogEx
{
// 构造
public:
	Cface_compareDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FACE_COMPARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	BOOL ParseResult(CString &strRes, cJSON*& pJson);

// 实现
protected:
	HICON m_hIcon;
	CString m_strCfgPath;
	IplImage *image1 = nullptr; //图像 1
	IplImage *image2 = nullptr; //
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void WriteConfig();

	void LoadConfig();
	void DrawPicToHDC(IplImage *img, UINT ID);
public:
	//CString m_strAppID;
	//CString m_strAppKey;
	afx_msg void OnBnClickedButtonImg1();
	afx_msg void OnBnClickedButtonImg2();
	afx_msg void OnBnClickedMfcbuttonDetect();
	afx_msg void OnBnClickedMfcbuttonFeature();
	afx_msg void OnBnClickedMfcbuttonCompare();
	CString m_strAppID;
	CString m_strAppKey;
	afx_msg void OnBnClickedMfcbuttonExit();
	CString m_filepath1;
	CString m_filepath2;
};
