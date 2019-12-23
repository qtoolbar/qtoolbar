/*
   Quero Toolbar
   http://www.quero.at/
   Copyright 2013 Viktor Krammer

   This file is part of Quero Toolbar.

   Quero Toolbar is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Quero Toolbar is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Quero Toolbar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "CertificateDownloader.h"
#include "QToolbar.h"

#include <winhttp.h>
#include <wincrypt.h>

CCertificateDownloader::CCertificateDownloader(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
	HostName[0]=0;
	Certificate_Organization[0]=0;
	LTime_HostName=0;
}

void CCertificateDownloader::Download(TCHAR *pURL,int HostStartIndex,int HostEndIndex)
{
	if(WaitForSingleObject(hDownloadMutex,DOWNLOAD_MUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		StringCchCopyN(HostName,MAXURLLENGTH,pURL+HostStartIndex,HostEndIndex-HostStartIndex);
		Certificate_Organization[0]=0;
		LTime_HostName++;

		if(g_Options2&OPTION2_DisplayCertificateInformation) CreateDownloadThread(CertificateDownloadThread);

		ReleaseMutex(hDownloadMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"CCertificateDownloader::Download");
}

DWORD WINAPI CCertificateDownloader::CertificateDownloadThread(LPVOID lpParameter)
{
	CCertificateDownloader *pCertificateDownloader=(CCertificateDownloader*)lpParameter;
	HANDLE hMutex;
	HWND hWnd;
	TCHAR HostName[MAXURLLENGTH];
	UINT LTime_HostName;

	HINTERNET hInternet_Open;
	HINTERNET hInternet_Connect;
	HINTERNET hInternet_Request;
	CERT_CONTEXT *pCertContext;
	DWORD size;

	// Save mutex and hwnd handle in the case that pCertificateDownloader instance terminates while this thread is still running

	hMutex=pCertificateDownloader->hDownloadMutex;
	hWnd=pCertificateDownloader->m_pToolbar->m_hWnd;

	if(WaitForSingleObject(hMutex,DOWNLOAD_MUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		StringCbCopy(HostName,sizeof HostName,pCertificateDownloader->HostName);
		LTime_HostName=pCertificateDownloader->LTime_HostName;
		
		ReleaseMutex(hMutex);

		// Download Certificate
		hInternet_Open=WinHttpOpen(NULL,WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,NULL,NULL,0);
		if(hInternet_Open)
		{
			hInternet_Connect=WinHttpConnect(hInternet_Open,HostName,INTERNET_DEFAULT_HTTPS_PORT,0);
			if(hInternet_Connect)
			{
				hInternet_Request=WinHttpOpenRequest(hInternet_Connect,L"HEAD",L"/",NULL,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE);
				if(hInternet_Request)
				{
					if(WinHttpSendRequest(hInternet_Request, WINHTTP_NO_ADDITIONAL_HEADERS,0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
					{
						size=sizeof pCertContext;
						if(WinHttpQueryOption(hInternet_Request,WINHTTP_OPTION_SERVER_CERT_CONTEXT,&pCertContext,&size))
						{
							if(WaitForSingleObject(hMutex,DOWNLOAD_MUTEX_TIMEOUT)==WAIT_OBJECT_0)
							{
								if(LTime_HostName==pCertificateDownloader->LTime_HostName && pCertificateDownloader->bAbortThreads==false)
								{
									if(CertGetNameString(pCertContext,CERT_NAME_ATTR_TYPE,0,szOID_ORGANIZATION_NAME,pCertificateDownloader->Certificate_Organization,MAX_CERT_NAME_LENGTH)>1)
									{
										QDEBUG_PRINT(L"CERTIFICATE ORGANIZATION",pCertificateDownloader->Certificate_Organization);
										::PostMessage(hWnd,WM_QUERO_TOOLBAR_COMMAND,QUERO_COMMAND_CERTIFICATE_DOWNLOAD_COMPLETE,0);
									}
								}
								ReleaseMutex(hMutex);
							}
							QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"CertificateDownloadThread 2");
							CertFreeCertificateContext(pCertContext);
						}
					}
					WinHttpCloseHandle(hInternet_Request);
				}	
				WinHttpCloseHandle(hInternet_Connect);
			}
			WinHttpCloseHandle(hInternet_Open);
		}
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"CertificateDownloadThread 1");

	ExitThread(0);
}
