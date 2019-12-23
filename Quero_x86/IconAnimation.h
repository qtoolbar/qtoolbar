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
// IconAnimation.h

#ifndef __ICONANIMATION_H
#define __ICONANIMATION_H

#define ROTATING_DISK_ANIMATION_CX 16
#define ROTATING_DISK_ANIMATION_CY 16
#define ROTATING_DISK_ANIMATION_STEPS_IE8 18
#define ROTATING_DISK_ANIMATION_STEPS_IE9 12
#define ROTATING_DISK_ANIMATION_STEPS_MAX 18
#define ROTATING_DISK_ANIMATION_RESID 624

#define ICON_ANIMATION_NONE 0x0
#define ICON_ANIMATION_ALL 0xFF
#define ICON_ANIMATION_SEARCHING 0x1
#define ICON_ANIMATION_LOADING 0x2

class CQToolbar;

class CIconAnimation  
{
public:
	CIconAnimation(CQToolbar *pToolbar);
	~CIconAnimation();

	static DWORD WINAPI IconAnimationThread(LPVOID lpParameter);

	void Start(BYTE Animation);
	void Stop(BYTE Animation,bool wait=false);
	void AbortThread();
	inline bool IsAnimating() { return bAnimating; }
	inline BYTE GetRotatingDiskAnimationStep() { return RotatingDiskAnimationStep; }
	inline BYTE GetActiveAnimations() { return ActiveAnimations; }
	HDC GetDC_RotatingDisk(HDC hDC);

private:
	CQToolbar* m_pToolbar;
	HANDLE hIconAnimationMutex;
	HANDLE hIconAnimationEvent;
	HANDLE hIconAnimationThread;
	bool bAbortThread;
	bool bAnimating;
	BYTE ActiveAnimations;

	HBITMAP hBitmap_RotatingDisk;
	HDC hDC_RotatingDisk;
	HBITMAP hBitmap_Old;
	BYTE RotatingDiskAnimationStep;
};

#endif
