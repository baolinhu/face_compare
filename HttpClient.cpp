#include "StdAfx.h"
#include "HttpClient.h"
#include "Comon_Function.h"

CHttpClient::CHttpClient(void)
{
	m_pSession = new CInternetSession();
	m_pConnection = NULL;
	m_pFile = NULL;
}

CHttpClient::~CHttpClient(void)
{
	Clear();
	if(NULL != m_pSession)
	{
		m_pSession->Close();
		delete m_pSession;
		m_pSession = NULL;
	}
}

void CHttpClient::Clear()
{
	if(NULL != m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
	}

	if(NULL != m_pConnection)
	{
		m_pConnection->Close();	
		delete m_pConnection;
		m_pConnection = NULL;
	}
}



int CHttpClient::ExecuteRequest(CString strMethod, CString strUrl, CString strPostData, CString &strResponse)
{
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	strResponse = "";

	const int nTimeOut = 2000;
	m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut); //重试之间的等待延时
	m_pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);   //重试次数

	AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);

	if(AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{	
		return FAILURE;

	}

	try
	{
		m_pConnection = m_pSession->GetHttpConnection(strServer, 
			dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT, nPort);
		m_pFile = m_pConnection->OpenRequest(strMethod, strObject, 
			NULL, 1, NULL, NULL, (dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));


		////忽略无效证书验证
		DWORD dwFlags;
		m_pFile->QueryOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
		dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
		//set web server option
		m_pFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);

		//webservice访问加上访问内网异常
		//m_pFile->AddRequestHeaders(_T("Accept: *,*/*"));
		//m_pFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));
		//m_pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded"));	
		//m_pFile->AddRequestHeaders(_T("Accept-Encoding: gzip, deflate"));

        CString strHeader = _T("Content-Type: application/x-www-form-urlencoded");
		USES_CONVERSION;
		m_pFile->SendRequest(strHeader, strHeader.GetLength(), (LPVOID)T2A(strPostData), strPostData.IsEmpty() ? 0 : strPostData.GetLength());
		//m_pFile->SendRequest(NULL, 0, (LPVOID)(LPCTSTR)strPostData, strPostData.IsEmpty() ? 0 : strPostData.GetLength());

        /*CString strInfo;
        m_pFile->QueryInfo(HTTP_QUERY_ECHO_REPLY,strInfo);*/
        DWORD dw;
        BOOL bResult = m_pFile->QueryInfoStatusCode(dw);
        if (dw != HTTP_STATUS_OK)
        {
            return dw;
        }
		char szChars[BUFFER_SIZE + 1] = {0};
// 		CString strRawResponse;
// 		strRawResponse.Empty();
        string  strRawResponse;
		UINT nReaded = 0;
		while ((nReaded = m_pFile->Read((void*)szChars, BUFFER_SIZE)) > 0)
		{
			szChars[nReaded] = '\0';
			strRawResponse += szChars;
			memset(szChars, 0, BUFFER_SIZE + 1);
		}
		//strResponse = Utf82UnicodeW(strRawResponse.GetBuffer(0)).c_str();
        strResponse = string2wstring(Utf82Unicode(strRawResponse)).c_str();
		Clear();
	}	
	catch (CInternetException* e)
	{
		Clear();
		DWORD dwErrorCode = e->m_dwError;
		e->Delete();
		DWORD dwError = GetLastError();

		if (ERROR_INTERNET_TIMEOUT == dwErrorCode)			
		{
			return OUTTIME;
		}
		else	
		{
			return FAILURE;
		}
	}
	return SUCCESS;
}

int CHttpClient::HttpGet(CString strUrl, CString strPostData, CString &strResponse)
{
	return ExecuteRequest(_T("GET"), strUrl, strPostData, strResponse);
}

int CHttpClient::HttpPost(CString strUrl, CString strPostData, CString &strResponse)
{
	return ExecuteRequest(_T("POST"), strUrl, strPostData, strResponse);
}
