
// UsbHidDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UsbHidDemo.h"
#include "UsbHidDemoDlg.h"
#include "afxdialogex.h"
#include "UsbHidDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DWORD WINAPI TreadProc(LPVOID lpThreadParameter)
{
    // 开启读的线程
    CUsbHidDemoDlg* dlg = (CUsbHidDemoDlg*)lpThreadParameter;
    while(1)
    {
		dlg->TestReadProc();
    }
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUsbHidDemoDlg 对话框




CUsbHidDemoDlg::CUsbHidDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUsbHidDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    handle = NULL;
}

void CUsbHidDemoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_wndWriteEdit);
    DDX_Control(pDX, IDC_RADIO1, m_wndReport);
    DDX_Control(pDX, IDC_STATIC_DEVICE, m_wndDeviceName);
    DDX_Control(pDX, IDC_STATIC_READ, m_wndRead);
    DDX_Control(pDX, IDC_EDIT_FILEPATH, m_wndFilePath);
    DDX_Control(pDX, IDC_STATIC_STATUS, m_wndFileStatus);
    DDX_Control(pDX, IDC_STATIC_LOG, m_openlog);
}

BEGIN_MESSAGE_MAP(CUsbHidDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON2, &CUsbHidDemoDlg::OnBnClickedWriteByte)
    ON_BN_CLICKED(IDC_BUTTON1, &CUsbHidDemoDlg::OnBnClickedReFresh)
    ON_BN_CLICKED(IDC_BUTTON4, &CUsbHidDemoDlg::OnBnClickedOpen)
    ON_BN_CLICKED(IDC_RADIO1, &CUsbHidDemoDlg::OnBnClickedSetReport)
    ON_BN_CLICKED(IDC_RADIO2, &CUsbHidDemoDlg::OnBnClickedSetFeature)
    ON_BN_CLICKED(IDC_BUTTON_OPENFILE, &CUsbHidDemoDlg::OnBnClickedButtonOpenfile)
    ON_BN_CLICKED(IDC_BUTTON_WRITEDATA, &CUsbHidDemoDlg::OnBnClickedButtonWritedata)
END_MESSAGE_MAP()


// CUsbHidDemoDlg 消息处理程序

BOOL CUsbHidDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    m_wndReport.SetCheck(BST_CHECKED);

    if (USBHIDFindUSBHIDDevice())
    {
        CString deviceName = USBHIDGetDeviceName();
        m_wndDeviceName.SetWindowText(deviceName);
    }

	isWrite = false;

    fileData = NULL;

    handle = NULL;
    hThread = CreateThread(NULL, 0, TreadProc, this, 0, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUsbHidDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUsbHidDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUsbHidDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUsbHidDemoDlg::OnBnClickedWriteByte()
{
    CString tmp;
    m_wndWriteEdit.GetWindowTextA(tmp);
    if (tmp.IsEmpty()) return ; // 提示输入内容
    int len = tmp.GetLength();

    BYTE* pBuffer = (BYTE*)tmp.GetBuffer(len);
    BYTE* buffer = new BYTE[len];

    // 转换到16进制
	int index = 0; 
	char* token = strtok(tmp.GetBuffer(0), " ");
	char* token2;
	while (token)
	{
		buffer[index] = strtol(token, &token2, 16);
		token = strtok(NULL, " ");
		index++;
	}

	isWrite = true;

    BYTE* tmpBuf = buffer;

    // 循环写入
    while (index > caps.OutputReportByteLength)
    {
        // 根据设备能力写
        USBHIDWriteByte(handle, tmpBuf, caps.OutputReportByteLength);
        
        // 剩余的内容
        index -= caps.OutputReportByteLength;
        
        // 跳过已经写的
        tmpBuf += caps.OutputReportByteLength;
    }

    if (index > 0)
    {
        USBHIDWriteByte(handle, tmpBuf, index);
    }
   
	isWrite = false;

    delete [] buffer;
}


void CUsbHidDemoDlg::OnBnClickedReFresh()
{
    // TODO: 在此添加额外的初始化代码
    if (USBHIDFindUSBHIDDevice())
    {
        CString deviceName = USBHIDGetDeviceName();
        m_wndDeviceName.SetWindowText(deviceName);
    }
}

// 00 fd 01 00 a0 c0 01 02 00 00 00 00 66 fe
void CUsbHidDemoDlg::OnBnClickedOpen()
{
    // 如果已经打开，则关闭
    if (handle)
    {
        USBHIDCloseHandle(handle);
        handle = NULL;
        ((CButton*)GetDlgItem(IDC_BUTTON4))->SetWindowTextA("Open");
    }
    else
    {
        handle = USBHIDCreateUsbHid();
        if (NULL == handle)
        {
            ((CButton*)GetDlgItem(IDC_BUTTON4))->SetWindowTextA("Open");
        }
        else
        {
            ((CButton*)GetDlgItem(IDC_BUTTON4))->SetWindowTextA("Close");
            HIDD_ATTRIBUTES attributes;
            // 获取设备能力
            USBHIDGetDeviceCapabilities(handle, &attributes, &caps);
        }
    }
}


void CUsbHidDemoDlg::OnBnClickedSetReport()
{
    // TODO: 在此添加控件通知处理程序代码
    USBHIDSetType(T_Report);
}


void CUsbHidDemoDlg::OnBnClickedSetFeature()
{
    // TODO: 在此添加控件通知处理程序代码
    USBHIDSetType(T_Feature);
}


void CUsbHidDemoDlg::OnBnClickedButtonOpenfile()
{
    BYTE file1Data[2] ={1, 0};
    FILE* fp = fopen("D:\\test.bin", "wb");
    fwrite(file1Data,1,2,fp);
    fclose(fp);
   CString filePathName;
   CFileDialog dlg(TRUE, 
       NULL, 
       NULL,
       OFN_HIDEREADONLY |  OFN_OVERWRITEPROMPT,
       _T("bin File (*.bin)|*.bin|All Files(*.*)|*.*||"),
       NULL);

   if (dlg.DoModal() == IDOK)
   {
       filePathName = dlg.GetPathName();
       m_wndFilePath.SetWindowText(filePathName);
       FILE* fp = fopen(filePathName.GetBuffer(0), "rb+");

       if (fp == NULL)
       {
           MessageBox("Open file error.");
           return ;
       }

       fseek(fp, 0, SEEK_END);

       // 获取文件大小
       int len = ftell(fp);

       // 删除上一次的空间
       if (fileData != NULL)
       {
           delete [] fileData;
       }
       // 分配空间大小
       fileData = new BYTE[len];

       // 读取数据
       fseek(fp, 0, SEEK_SET);
       fileLen = 0;
       fileLen = fread(fileData, 1, len, fp);

       CString fstatus;
       // 显示大小
       fstatus.Format("文件大小:%d",fileLen);
       m_wndFileStatus.SetWindowText(fstatus);

       fclose(fp);
       fp = NULL;
   }
}


void CUsbHidDemoDlg::OnBnClickedButtonWritedata()
{
    if (handle)
    {
        BYTE* tmpbuf = fileData;
        // 循环写入
        while (fileLen > caps.OutputReportByteLength)
        {
            // 根据设备能力写
            USBHIDWriteByte(handle, tmpbuf, caps.OutputReportByteLength);

            // 剩余的内容
            fileLen -= caps.OutputReportByteLength;

            // 跳过已经写的
            tmpbuf += caps.OutputReportByteLength;
        }

        if (fileLen > 0)
        {
            USBHIDWriteByte(handle, tmpbuf, fileLen);
        }
    }
    delete [] fileData;
}

void CUsbHidDemoDlg::TestReadProc()
{
    if (handle)
    {
		if (isWrite)
		{
			Sleep(50);
		}
		else
		{
			BYTE tmpData[30];
			memset(tmpData, '\0', 30);
			// BYTE tmpData[10] = {1, 2, 3, 4, 5};
			int len = USBHIDReadByte(handle, tmpData, 30);

			int kk = 0;
			char tmp[1024];
			memset(tmp, '\0', 1024);

			if (len <= 0)
			{
				Sleep(50);
				return;
			}
			CString text;
			for (int i = 0; i < len; i++)
			{
				sprintf(tmp, "%02x ", tmpData[i]);  // 以16进制显示
				text += tmp;
			}
			m_wndRead.SetWindowText(text);
		}
    }
}

BOOL CUsbHidDemoDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    USBHIDCloseHandle(handle);
    if (hThread)
    {
        CloseHandle(hThread);
    }
    return CDialogEx::DestroyWindow();
}
