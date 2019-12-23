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

#pragma once

#define MAX_DOWNLOAD_THREADS 32

class CDownloader
{
public:
	CDownloader(void);
	~CDownloader(void);

	void CreateDownloadThread(LPTHREAD_START_ROUTINE lpStartAddress);
	void AbortThreads();

protected:
	HANDLE hDownloadMutex;
	bool bAbortThreads;

	HANDLE DownloadThreads[MAX_DOWNLOAD_THREADS];
	int DownloadThreadIndex;
};
