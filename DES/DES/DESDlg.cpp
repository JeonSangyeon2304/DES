
// DESDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "DES.h"
#include "DESDlg.h"
#include "afxdialogex.h"

#include <string.h>
#include <openssl/des.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
BOOL fs_encrypt_des(char *infile, char *ckey,char *outfile);
BOOL fs_decrypt_des(char *infile, char *ckey, char *outfile);
BOOL fs_passwd(char *passwd);


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDESDlg 대화 상자



CDESDlg::CDESDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DES_DIALOG, pParent)
	, cPasswd(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDESDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, cPath);
	//  DDX_Control(pDX, IDC_EDIT2, cPasswd);
	DDX_Text(pDX, IDC_EDIT2, cPasswd);
}

BEGIN_MESSAGE_MAP(CDESDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CDESDlg::OnOpenFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CDESDlg::OnEncrypt)
	ON_BN_CLICKED(IDC_BUTTON3, &CDESDlg::OnDecrypt)
END_MESSAGE_MAP()


// CDESDlg 메시지 처리기

BOOL CDESDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDESDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDESDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDESDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDESDlg::OnOpenFile() {
	CString szFilter = _T("All Files(*.*)|*.*||");
	CString strPathName;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal()) {
		strPathName = _T(dlg.GetPathName());
		CFile fp;
		CFileException e;
		if (!fp.Open(strPathName, CFile::modeRead, &e)) {
			e.ReportError();
			return;
		}
		else {
			in_file = strPathName;
			cPath.SetWindowTextA(strPathName);
			fp.Close();
		}
	}
}


void CDESDlg::OnEncrypt() {
	UpdateData(TRUE);
	char *passwd = (char *)malloc(9*sizeof(char));
	strcpy(passwd, (LPTSTR)(LPCTSTR)cPasswd);
	if(!(fs_passwd(passwd)))
		AfxMessageBox("암호화에 성공하였습니다.");

	char *infile = (char *)malloc(in_file.GetLength());
	strcpy(infile, (LPTSTR)(LPCTSTR)in_file);

	char *out_file = (char *)malloc(in_file.GetLength() + 4);
	strcpy(out_file, (LPTSTR)(LPCTSTR)in_file);
	strcat(out_file, ".aes");

	if (!(fs_encrypt_des(infile, passwd, out_file)))
		AfxMessageBox("암호화에 성공하였습니다.");
	UpdateData(FALSE);
}


void CDESDlg::OnDecrypt() {
	UpdateData(TRUE);
	char *passwd = (char *)malloc(cPasswd.GetLength()+1);
	strcpy(passwd, (LPTSTR)(LPCTSTR)cPasswd);
	if (!(fs_passwd(passwd)))
		AfxMessageBox("암호화에 성공하였습니다.");

	char *infile = (char *)malloc(in_file.GetLength());
	strcpy(infile, (LPTSTR)(LPCTSTR)in_file);

	char *out_file = (char *)malloc(in_file.GetLength() + 4);
	strcpy(out_file, (LPTSTR)(LPCTSTR)in_file);
	strcat(out_file, ".des");

	if (!(fs_decrypt_des(infile, passwd, out_file)))
		AfxMessageBox("암호화에 성공하였습니다.");
	UpdateData(FALSE);
}

BOOL fs_encrypt_des(char *infile, char *ckey, char *outfile) {
	int file_size;
	static char* Res;
	int n = 0;
	DES_cblock key;
	DES_key_schedule schedule;
	int len = 0;


	FILE *fp = fopen(infile, "rb");
	if (fp == NULL) {
		CString err = "[ERROR] can not fopen in_file";
		AfxMessageBox(err);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *buf = (char *)malloc(file_size+2);
	Res = (char *)malloc(file_size);

	memcpy(key, ckey, 8);
	DES_set_odd_parity(&key);
	DES_set_key_checked(&key, &schedule);

	FILE *wfp = fopen(outfile, "wb");
	if (wfp == NULL) {
		CString err = "[ERROR] can not fopen out_file";
		AfxMessageBox(err);
		return 0;
	}
	while (len = fread(buf, 1, file_size, fp)) {
		if (file_size != len) {
			break;
		}
		DES_cfb64_encrypt((unsigned char *)buf, (unsigned char *)Res,
			file_size, &schedule, &key, &n, DES_ENCRYPT);
		fwrite(Res, 1, len, wfp);

		fclose(wfp);
		fclose(fp);

		return 1;
	}
}


BOOL fs_decrypt_des(char *infile, char *ckey, char *outfile) {
	int file_size;
	static char* Res;
	int n = 0;
	DES_cblock key;
	DES_key_schedule schedule;
	int len = 0;


	FILE *fp = fopen(infile, "rb");
	if (fp == NULL) {
		CString err = "[ERROR] can not fopen in_file";
		AfxMessageBox(err);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *buf = (char *)malloc(file_size + 2);
	Res = (char *)malloc(file_size);

	memcpy(key, ckey, 8);
	DES_set_odd_parity(&key);
	DES_set_key_checked(&key, &schedule);

	FILE *wfp = fopen(outfile, "wb");
	if (wfp == NULL) {
		CString err = "[ERROR] can not fopen out_file";
		AfxMessageBox(err);
		return 0;
	}
	while (len = fread(buf, 1, file_size, fp)) {
		if (file_size != len) {
			break;
		}
		DES_cfb64_encrypt((unsigned char *)buf, (unsigned char *)Res,
			file_size, &schedule, &key, &n, DES_DECRYPT);
		fwrite(Res, 1, len, wfp);

		fclose(wfp);
		fclose(fp);

		return 1;
	}
}


BOOL fs_passwd(char *passwd) {
	int padding_len = 8 - strlen(passwd);
	char padding[8];
	int padding_num = passwd[strlen(passwd)-1] - 1;
	memset(padding, padding_num, padding_len);
	padding[padding_len] = '\0';
	strcat(passwd, padding);
	return 1;
}