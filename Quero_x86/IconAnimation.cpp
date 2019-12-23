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
// IconAnimation.cpp: implementation of the CIconAnimation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IconAnimation.h"
#include "QToolbar.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIconAnimation::CIconAnimation(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
	bAbortThread=false;
	bAnimating=false;
	ActiveAnimations=ICON_ANIMATION_NONE;

	hBitmap_RotatingDisk=NULL;
	hDC_RotatingDisk=NULL;
	RotatingDiskAnimationStep=0;

	// Create search animation mutex and event object

	hIconAnimationMutex=CreateMutex(NULL,FALSE,NULL);
	hIconAnimationEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	hIconAnimationThread=NULL;
}

CIconAnimation::~CIconAnimation()
{
	if(hIconAnimationThread) AbortThread();
	if(hIconAnimationMutex) CloseHandle(hIconAnimationMutex);
	if(hIconAnimationEvent) CloseHandle(hIconAnimationEvent);

	if(hDC_RotatingDisk)
	{
		SelectObject(hDC_RotatingDisk,hBitmap_Old);
		DeleteDC(hDC_RotatingDisk);
	}
	if(hBitmap_RotatingDisk) DeleteObject(hBitmap_RotatingDisk);
}

DWORD WINAPI CIconAnimation::IconAnimationThread(LPVOID lpParameter)
{
	CIconAnimation *pIconAnimation=(CIconAnimation*)lpParameter;
	int offset,dir,times;
	BYTE steps=(g_IE_MajorVersion>=9)?ROTATING_DISK_ANIMATION_STEPS_IE9:ROTATING_DISK_ANIMATION_STEPS_IE8;

	while(true)
	{
		if(WaitForSingleObject(pIconAnimation->hIconAnimationEvent,INFINITE)==WAIT_OBJECT_0)
		{
			if(pIconAnimation->bAbortThread) break;

			pIconAnimation->bAnimating=true; // Protects the icon area from redrawing in WM_PAINT

			while(pIconAnimation->ActiveAnimations)
			{
				if(WaitForSingleObject(pIconAnimation->hIconAnimationMutex,SEARCH_MUTEX_TIMEOUT)==WAIT_OBJECT_0)
				{
					times=0;
					offset=0;
					if(pIconAnimation->ActiveAnimations&ICON_ANIMATION_SEARCHING)
					{
						// Search animation
						dir=+1;
						while((times<2 || (pIconAnimation->ActiveAnimations&ICON_ANIMATION_SEARCHING)) && 
							pIconAnimation->m_pToolbar->GetCurrentType()==TYPE_SEARCH && pIconAnimation->m_pToolbar->GetPhraseNotFound()==false)
						{
							offset+=dir;
							if(offset==0) times++;
							else if(offset>1) dir=-1;
							else if(offset<-1) dir=+1;
							pIconAnimation->m_pToolbar->SetIconOffset(offset);
							Sleep(60);
						}
						pIconAnimation->m_pToolbar->SetIconOffset(0);
					}
					if(times==0 && offset==0)
					{
						// Page loading animation
						while((pIconAnimation->ActiveAnimations&ICON_ANIMATION_LOADING) && pIconAnimation->m_pToolbar->GetCurrentType()==TYPE_ADDRESS)
						{
							pIconAnimation->m_pToolbar->DrawComboQueroIcon();
							pIconAnimation->RotatingDiskAnimationStep = (pIconAnimation->RotatingDiskAnimationStep+1)%steps;
							times=1;
							Sleep(60);
						}
						// No animation, draw icon
						if(times==0)
						{
							pIconAnimation->m_pToolbar->DrawComboQueroIcon();
							Sleep(60);
						}
					}
					ReleaseMutex(pIconAnimation->hIconAnimationMutex);
				} // End animation mutex
				QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"IconAnimationThread");
			} // End while ActiveAnimations
			pIconAnimation->bAnimating=false;
			pIconAnimation->m_pToolbar->m_ComboQuero.Invalidate(FALSE);
		} // End Animation event
	} // End infinite loop

	ExitThread(0);
}

HDC CIconAnimation::GetDC_RotatingDisk(HDC hDC)
{
	if(hDC_RotatingDisk==NULL)
	{
		if(WaitForSingleObject(hIconAnimationMutex,SEARCH_MUTEX_TIMEOUT)==WAIT_OBJECT_0)
		{
			if(hDC_RotatingDisk==NULL && hBitmap_RotatingDisk==NULL)
			{
				HMODULE hModule_IEFrame;
				BITMAPINFO bmi;
				BYTE vBits[ROTATING_DISK_ANIMATION_CX*ROTATING_DISK_ANIMATION_STEPS_MAX*ROTATING_DISK_ANIMATION_CY*4];
				int i;
				BYTE steps=(g_IE_MajorVersion>=9)?ROTATING_DISK_ANIMATION_STEPS_IE9:ROTATING_DISK_ANIMATION_STEPS_IE8;

				hModule_IEFrame=GetModuleHandle(L"ieframe.dll");
				hBitmap_RotatingDisk=(HBITMAP)LoadImage(hModule_IEFrame,MAKEINTRESOURCE(ROTATING_DISK_ANIMATION_RESID),IMAGE_BITMAP,ROTATING_DISK_ANIMATION_CX*steps,ROTATING_DISK_ANIMATION_CY,LR_CREATEDIBSECTION);
				if(hBitmap_RotatingDisk)
				{
					hDC_RotatingDisk=CreateCompatibleDC(hDC);
					if(hDC_RotatingDisk)
					{
						SelectObject(hDC_RotatingDisk,hBitmap_RotatingDisk);

						ZeroMemory(&bmi, sizeof bmi);
						bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
						bmi.bmiHeader.biWidth = ROTATING_DISK_ANIMATION_CX*steps;
						bmi.bmiHeader.biHeight = ROTATING_DISK_ANIMATION_CX;
						bmi.bmiHeader.biPlanes = 1;
						bmi.bmiHeader.biBitCount = 32;
						bmi.bmiHeader.biCompression = BI_RGB;
						bmi.bmiHeader.biSizeImage = ROTATING_DISK_ANIMATION_CX*ROTATING_DISK_ANIMATION_STEPS_MAX*ROTATING_DISK_ANIMATION_CY * 4;

						GetDIBits(hDC_RotatingDisk,hBitmap_RotatingDisk,0,ROTATING_DISK_ANIMATION_CX,&vBits,&bmi,DIB_RGB_COLORS);

						// Pre-multiply with alpha channel
						i=0;
						while(i<ROTATING_DISK_ANIMATION_CX*steps*ROTATING_DISK_ANIMATION_CY * 4)
						{
							vBits[i]=(BYTE)(((UINT)vBits[i]*vBits[i+3])/255);
							vBits[i+1]=(BYTE)(((UINT)vBits[i+1]*vBits[i+3])/255);
							vBits[i+2]=(BYTE)(((UINT)vBits[i+2]*vBits[i+3])/255);
							i+=4;
						}
						
						SetDIBits(hDC_RotatingDisk,hBitmap_RotatingDisk,0,ROTATING_DISK_ANIMATION_CY,&vBits,&bmi,DIB_RGB_COLORS);
					}
				}
			}
			ReleaseMutex(hIconAnimationMutex);
		}
		QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"GetDC_RotatingDisk");
	}

	return hDC_RotatingDisk;
}

void CIconAnimation::Start(BYTE Animation)
{
	bool bSignalEvent;

	// Create search animation thread
	if(hIconAnimationThread==NULL && bAbortThread==false)
		hIconAnimationThread=CreateThread(NULL,0,IconAnimationThread,(LPVOID)this,0,NULL);

	bSignalEvent = (ActiveAnimations==ICON_ANIMATION_NONE);
	ActiveAnimations |= Animation;

	// Signal search animation event
	if(bSignalEvent) SetEvent(hIconAnimationEvent);
}

void CIconAnimation::Stop(BYTE Animation,bool wait)
{
	ActiveAnimations &= ~Animation;
	
	if(wait)
	{
		if(WaitForSingleObject(hIconAnimationMutex,SEARCH_MUTEX_TIMEOUT)==WAIT_OBJECT_0) ReleaseMutex(hIconAnimationMutex);
		QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"StopIconAnimation");
	}
}

void CIconAnimation::AbortThread()
{
	if(hIconAnimationThread)
	{
		ActiveAnimations=ICON_ANIMATION_NONE;
		bAbortThread=true;

		SetEvent(hIconAnimationEvent);

		if(WaitForSingleObject(hIconAnimationThread,SEARCH_MUTEX_TIMEOUT)!=WAIT_OBJECT_0)
		{
			TerminateThread(hIconAnimationThread,0);
			QDEBUG_PRINT(L"SyncError",L"AbortThread");
		}

		CloseHandle(hIconAnimationThread);
		hIconAnimationThread=NULL;
	}
}