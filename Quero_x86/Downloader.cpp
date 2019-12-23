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
#include "Downloader.h"
#include "QToolbar.h"

CDownloader::CDownloader(void)
{
	bAbortThreads=false;

	hDownloadMutex=CreateMutex(NULL,FALSE,NULL);

	ZeroMemory(&DownloadThreads,sizeof DownloadThreads);
	DownloadThreadIndex=0;
}

CDownloader::~CDownloader(void)
{
	if(!bAbortThreads) AbortThreads();
	if(hDownloadMutex) CloseHandle(hDownloadMutex);
}

void CDownloader::AbortThreads()
{
	int i=0;
	QDEBUG_CODE DWORD result;

	bAbortThreads=true;

	// Block creation of new download threads
	WaitForSingleObject(hDownloadMutex,DOWNLOAD_MUTEX_TIMEOUT);

	while(i<MAX_DOWNLOAD_THREADS)
	{
		if(DownloadThreads[i])
		{
			// Wait for download thread to terminate
			QDEBUG_CODE result=
			WaitForSingleObject(DownloadThreads[i],DOWNLOAD_MUTEX_TIMEOUT);

			QDEBUG_CODE if(result!=WAIT_OBJECT_0) QDEBUG_PRINT(L"CDownloader::AbortThreads",L"DownloadThread still running");

			// Close handle anyway
			CloseHandle(DownloadThreads[i]);
			DownloadThreads[i]=NULL;
		}

		i++;
	}
}

void CDownloader::CreateDownloadThread(LPTHREAD_START_ROUTINE lpStartAddress)
{
	int i;

	i=DownloadThreadIndex;
	do
	{
		i++;
		if(i>=MAX_DOWNLOAD_THREADS) i=0;

		if(DownloadThreads[i])
		{
			if(WaitForSingleObject(DownloadThreads[i],0)==WAIT_OBJECT_0)
			{
				CloseHandle(DownloadThreads[i]);
				DownloadThreads[i]=NULL;
			}
			QDEBUG_CODE else QDEBUG_PRINT(L"CDownloader::Download",L"DownloadThread still running");
		}

	} while(DownloadThreads[i] && i!=DownloadThreadIndex);

	if(DownloadThreads[i]==NULL)
	{
		DownloadThreads[i]=CreateThread(NULL,0,lpStartAddress,(LPVOID)this,0,NULL);
		DownloadThreadIndex=i;
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"CDownloader::Download",L"all DownloadThreads busy");
}
