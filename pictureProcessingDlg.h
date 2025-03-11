
// pictureProcessingDlg.h: 头文件
//
#pragma once
#define WM_UPDATE_PROGRESS (WM_USER + 1)  // 定义自定义消息

class OperationData
{
public:
	CString fileName;
	CString operation;
	bool cancelFlag;

	OperationData(const CString& file, const CString& op)
		: fileName(file), operation(op), cancelFlag(false) {}

	void cancel() { cancelFlag = true; }
};


// CpictureProcessingDlg 对话框
class CpictureProcessingDlg : public CDialogEx
{
// 构造
public:
	CpictureProcessingDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PICTUREPROCESSING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LRESULT OnUpdateProgress(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CEdit picture_select;
	CListBox m_listBox;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();

	CString fileName;
	afx_msg void OnBnClickedButtonProcess();
	static UINT ProcessImageThread(LPVOID pParam);
	CComboBox m_ComBoxText;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButtoncancel();
	afx_msg void OnBnClickedButtonShowResult();
	static Mat detectFaces(Mat inputImage);
	static Mat thresholdSegmentation(Mat inputImage, int blockSize, int subtractValue);
	static Mat CpictureProcessingDlg::regionGrowingSegmentation(Mat inputImage, int seedX, int seedY, int threshold);
};
