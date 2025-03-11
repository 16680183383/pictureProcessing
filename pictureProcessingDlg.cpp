
// pictureProcessingDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "pictureProcessing.h"
#include "pictureProcessingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>

std::vector<OperationData*> threads;

// CpictureProcessingDlg 对话框
CpictureProcessingDlg::CpictureProcessingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PICTUREPROCESSING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CpictureProcessingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
	DDX_Control(pDX, IDC_COMBO1, m_ComBoxText);
}

BEGIN_MESSAGE_MAP(CpictureProcessingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UPDATE_PROGRESS, &CpictureProcessingDlg::OnUpdateProgress) 
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CpictureProcessingDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CpictureProcessingDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CpictureProcessingDlg::OnBnClickedButtonProcess)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CpictureProcessingDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_ButtonCancel, &CpictureProcessingDlg::OnBnClickedButtoncancel)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_RESULT, &CpictureProcessingDlg::OnBnClickedButtonShowResult)
END_MESSAGE_MAP()


// CpictureProcessingDlg 消息处理程序

BOOL CpictureProcessingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);		
	SetIcon(m_hIcon, FALSE);

	return TRUE; 
}


void CpictureProcessingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CpictureProcessingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CpictureProcessingDlg::OnUpdateProgress(WPARAM wParam, LPARAM lParam)
{
	CString* pFileName = (CString*)lParam; 
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST1); 

	if (pListBox == NULL)
		return 0;

	int nItemCount = pListBox->GetCount(); 
	for (int i = 0; i < nItemCount; ++i)
	{
		CString itemText;
		pListBox->GetText(i, itemText);

		if (itemText.Left(pFileName->GetLength()) == *pFileName)
		{
			CString status;
			if (wParam == 0) 
			{
				status = _T("正在处理");
			}
			else if (wParam == 1) 
			{
				status = _T("处理成功");
			}

			itemText = *pFileName + _T(" - ") + status;
			pListBox->DeleteString(i); 
			pListBox->InsertString(i, itemText); 
			break;
		}
	}
	pListBox->Invalidate();
	return 0; 
}

void CpictureProcessingDlg::OnBnClickedButtonAdd()
{
	TCHAR szFilter[] = _T("JPEG文件(*.jpg)|*.jpg|GIF文件(*.gif)|*.gif|bmp文件(*.bmp)|*.bmp||");
	CFileDialog fileDlg(TRUE, _T("jpg"), NULL, 0, szFilter, this);

	if (fileDlg.DoModal() != IDOK)
		return;

	fileName = fileDlg.GetPathName();

	CString status = _T("未处理");

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST1);
	if (pListBox == NULL) return;

	CString itemText = fileName + _T(" - ") + status;

	pListBox->AddString(itemText);

	int nItemCount = pListBox->GetCount();
	if (nItemCount > 0)
	{
		pListBox->SetCurSel(nItemCount - 1);
	}
}


void CpictureProcessingDlg::OnBnClickedButtonDelete()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST1);
	if (pListBox == NULL) return;

	int nCurSel = pListBox->GetCurSel();
	if (nCurSel == LB_ERR)
	{
		AfxMessageBox(_T("请先选择一个项目！"));
		return;
	}
	pListBox->DeleteString(nCurSel);
}


void CpictureProcessingDlg::OnBnClickedButtonProcess()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST1);
	if (pListBox == NULL) return;

	int nCurSel = pListBox->GetCurSel();
	if (nCurSel == LB_ERR)
	{
		AfxMessageBox(_T("请先选择一个图片！"));
		return;
	}

	CString fileName;
	pListBox->GetText(nCurSel, fileName);

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO1);
	if (pComboBox == NULL) return;

	int nSelectedIndex = pComboBox->GetCurSel();
	CString operation;
	pComboBox->GetLBText(nSelectedIndex, operation);

	OperationData* pData = new OperationData(fileName, operation);
	threads.push_back(pData);
	AfxBeginThread(ProcessImageThread, pData);
}

//UINT CpictureProcessingDlg::ProcessImageThread(LPVOID pParam)
//{
//
//	// 将传递的 LPVOID 指针转换为 CString 指针
//	CpictureProcessingDlg* pFilePath = (CpictureProcessingDlg*)pParam;
//	CString fileNameth = pFilePath->fileName;
//	// 获取图片文件路径
//	//CString fileName1 = *pFilePath;  // 获取文件路径（ANSI字符）
//	CStringA ansiFileName(fileNameth);  // 使用 CStringA 将 CString 转换为 ANSI 字符串
//	std::string filePath = fileNameth;//pFilePath->GetString();
//	// 读取图像
//	Mat src = imread(std::string(ansiFileName), 1);
//	if (src.empty())
//	{
//		AfxMessageBox(_T("无法加载图片"));
//		return 0;
//	}
//	// 直接使用CStringA对象
//	//CStringA fileNameA = *pFilePath;  // 这里直接从指针中获取CStringA对象
//	//CString fileName(fileNameA);
//
//	// 通过消息通知主线程，更新该文件为“正在处理”
//	CWnd* pWnd = AfxGetMainWnd();
//	pWnd->PostMessage(WM_UPDATE_PROGRESS, 0, (LPARAM)&fileNameth);
//
//	// 转换为灰度图像
//	Mat grayImage;
//	cvtColor(src, grayImage, COLOR_BGR2GRAY);
//
//	// 生成保存路径（如果需要保存处理后的图像）
//	std::string outputFilePath = filePath.substr(0, filePath.length() - 4) + "_gray.jpg";  // 生成灰度图的文件名
//	imwrite(outputFilePath, grayImage);  // 保存灰度图
//
//	// 处理完毕后，通过消息更新UI，标记为“处理成功”
//	pWnd->PostMessage(WM_UPDATE_PROGRESS, 1, (LPARAM)new CString(fileNameth));
//
//
//
//	return 0;
//}

Mat CpictureProcessingDlg::detectFaces(Mat inputImage)
{
	CascadeClassifier cascade;
	const string path = "D:\\Users\\psh\\source\\repos\\pictureProcessing\\xml\\haarcascade_frontalface_alt.xml";
	if (!cascade.load(path))
	{
		AfxMessageBox(TEXT("cascade load failed!"));
	}

	//计时e
	double t = 0;
	t = (double)getTickCount();
	vector<Rect> faces(0);
	cascade.detectMultiScale(inputImage, faces, 1.1, 2, 0, Size(30, 30));

	CString str;
	str.Format(TEXT("detect face number is : %d"), faces.size());
	AfxMessageBox(str);

	if (faces.size() > 0)
	{
		for (size_t i = 0; i < faces.size(); i++)
		{
			rectangle(inputImage, faces[i], Scalar(150, 0, 0), 3, 8, 0);
		}
	}
	else AfxMessageBox(TEXT("未检测到人脸"));

	return inputImage;
}

Mat CpictureProcessingDlg::thresholdSegmentation(Mat inputImage, int blockSize, int subtractValue)
{
	Mat grayImage;
	cvtColor(inputImage, grayImage, COLOR_BGR2GRAY);
	Mat segmentedAdaptive;
	adaptiveThreshold(grayImage, segmentedAdaptive, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
		THRESH_BINARY, blockSize, subtractValue);

	return segmentedAdaptive;
}

Mat CpictureProcessingDlg::regionGrowingSegmentation(Mat inputImage, int seedX, int seedY, int threshold)
{
	Mat grayImage;
	cvtColor(inputImage, grayImage, COLOR_BGR2GRAY);

	Mat segmentedImage = Mat::zeros(grayImage.size(), CV_8U);

	int seedValue = static_cast<int>(grayImage.at<uchar>(seedY, seedX));

	std::queue<std::pair<int, int>> pixelQueue;
	pixelQueue.push(std::make_pair(seedX, seedY));

	while (!pixelQueue.empty()) {
		int x = pixelQueue.front().first;
		int y = pixelQueue.front().second;
		pixelQueue.pop();

		if (segmentedImage.at<uchar>(y, x) == 0) {
			int pixelValue = static_cast<int>(grayImage.at<uchar>(y, x));
			int diff = std::abs(seedValue - pixelValue);

			if (diff <= threshold) {
				segmentedImage.at<uchar>(y, x) = 255;

				for (int dy = -1; dy <= 1; ++dy) {
					for (int dx = -1; dx <= 1; ++dx) {
						int newX = x + dx;
						int newY = y + dy;
						if (newX >= 0 && newX < grayImage.cols && newY >= 0 && newY < grayImage.rows) {
							pixelQueue.push(std::make_pair(newX, newY));
						}
					}
				}
			}
		}
	}
	return segmentedImage;
}

UINT CpictureProcessingDlg::ProcessImageThread(LPVOID pParam)
{
	OperationData* pData = (OperationData*)pParam;
	CString fileNameth = pData->fileName;
	CString operation = pData->operation;

	int pos = fileNameth.Find(_T(" - "));
	if (pos != -1) {
		fileNameth = fileNameth.Left(pos);
	}

	CStringA ansiFileName(fileNameth);
	std::string filePath = fileNameth;

	Mat src = imread(std::string(ansiFileName), 1);
	if (src.empty())
	{
		AfxMessageBox(_T("无法加载图片"));
		delete pData;
		return 0;
	}
	CWnd* pWnd = AfxGetMainWnd();
	pWnd->PostMessage(WM_UPDATE_PROGRESS, 0, (LPARAM)&fileNameth);

	Mat processedImage;
	while (true) {
		if (pData->cancelFlag) {
			AfxMessageBox(_T("线程已终止"));
			delete pData;
			return 0;
		}
		if (operation.CompareNoCase(_T("灰度处理")) == 0)
		{
			cvtColor(src, processedImage, COLOR_BGR2GRAY);
			break;
		}
		else if (operation.CompareNoCase(_T("放大至200%")) == 0)
		{
			resize(src, processedImage, Size(src.cols * 2, src.rows * 2));
			break;
		}
		else if (operation.CompareNoCase(_T("缩小至50%")) == 0)
		{
			resize(src, processedImage, Size(src.cols * 0.5, src.rows * 0.5));
			break;
		}
		else if (operation.CompareNoCase(_T("顺时针旋转90°")) == 0)
		{
			rotate(src, processedImage, ROTATE_90_CLOCKWISE);
			break;
		}
		else if (operation.CompareNoCase(_T("逆时针旋转90°")) == 0)
		{
			rotate(src, processedImage, ROTATE_90_COUNTERCLOCKWISE);
			break;
		}
		else if (operation.CompareNoCase(_T("人像捕捉")) == 0)
		{
			processedImage = detectFaces(src);
			break;
		}
		else if (operation.CompareNoCase(_T("阈值分割")) == 0)
		{
			int blockSize = 11;
			int subtractValue = 10;
			processedImage = thresholdSegmentation(src, blockSize, subtractValue);
			break;
		}
		else if (operation.CompareNoCase(_T("区域生长")) == 0)
		{
			int seedX = 100;
			int seedY = 100;
			int threshold = 50;
			processedImage = regionGrowingSegmentation(src, seedX, seedY, threshold);
			break;
		}
		else
		{
			AfxMessageBox(_T("未知操作"));
			delete pData;
			return 0;
		}
		Sleep(50);
	}
	std::string outputFilePath = filePath.substr(0, filePath.length() - 4) + "_processed.jpg";
	imwrite(outputFilePath, processedImage); 

	pWnd->PostMessage(WM_UPDATE_PROGRESS, 1, (LPARAM)new CString(fileNameth));

	delete pData;
	return 0;
}


void CpictureProcessingDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CpictureProcessingDlg::OnBnClickedButtoncancel()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST1);
	if (pListBox == NULL) return;

	int nCurSel = pListBox->GetCurSel();
	if (nCurSel == LB_ERR) 
	{
		AfxMessageBox(_T("请先选择一个图片！"));
		return;
	}

	CString fileName;
	pListBox->GetText(nCurSel, fileName);

	auto it = std::find_if(threads.begin(), threads.end(),
		[&](const OperationData* pData) { return pData->fileName == fileName; });

	if (it != threads.end()) {
		(*it)->cancel();
		AfxMessageBox(_T("取消处理"));
	}
}


void CpictureProcessingDlg::OnBnClickedButtonShowResult()
{
	// TODO: 在此添加控件通知处理程序代码
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST1);
	if (pListBox == NULL) return;

	int nCurSel = pListBox->GetCurSel();
	if (nCurSel == LB_ERR)
	{
		AfxMessageBox(_T("请先选择一个图片！"));
		return;
	}

	CString fileName;
	pListBox->GetText(nCurSel, fileName);

	int pos = fileName.Find(_T("."));
	if (pos != -1) {
		fileName = fileName.Left(pos);
	}

	CString processedFileName = fileName + _T("_processed.jpg");

	CStringA ansiFileName(processedFileName);

	Mat src = imread(std::string(ansiFileName), 1);

	//imshow("histImage", src);
	if (src.empty()) {
		AfxMessageBox(_T("无法加载图像，请检查文件路径！"));
		return;
	}

	const int fixedWidth = 800; 
	const int fixedHeight = 600;

	Mat resizedImg;
	double aspectRatio = (double)src.cols / (double)src.rows;
	int newWidth, newHeight;

	if (aspectRatio > 1.0) {
		newWidth = fixedWidth;
		newHeight = static_cast<int>(fixedWidth / aspectRatio);
	}
	else {
		newHeight = fixedHeight;
		newWidth = static_cast<int>(fixedHeight * aspectRatio);
	}
	resize(src, resizedImg, Size(newWidth, newHeight));

	namedWindow("ProcessedImage", WINDOW_AUTOSIZE);
	imshow("ProcessedImage", resizedImg);
}