//*****************************************************
//CDesktop.cpp
/*****************************************************/

#include "CDesktop.h"

//构造函数 
CDesktop::CDesktop():mode_changed(false)
{
	SaveMode();
}

//析构函数
CDesktop::~CDesktop()
{
	ResetMode();
}

// 存储现在的显示模式
void CDesktop::SaveMode()
{
	HDC	dc = GetDC(0);
	devmode_saved.dmSize = sizeof(devmode_saved);
	devmode_saved.dmDriverExtra = 0;
	devmode_saved.dmPelsWidth = GetDeviceCaps(dc, HORZRES);
	devmode_saved.dmPelsHeight = GetDeviceCaps(dc, VERTRES);
	devmode_saved.dmBitsPerPel = GetDeviceCaps(dc, BITSPIXEL);
	devmode_saved.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	//判断Windows版本是否为Windows NT/2000 
	if ((GetVersion() & 0x80000000) == 0)
	{		
		devmode_saved.dmFields |= DM_DISPLAYFREQUENCY;
		devmode_saved.dmDisplayFrequency = GetDeviceCaps(dc, VREFRESH);
	}
	ReleaseDC(0, dc);
}

// 还原成之前存储的显示模式
//
void CDesktop::ResetMode()
{
	if (mode_changed) 
	{
		ChangeDisplaySettings(&devmode_saved, 0);
		mode_changed = false;
	}
}

// 更改显示模式
//
bool CDesktop::ChangeMode(int width, int height)
{
	DEVMODE	devmode;

	devmode = devmode_saved;
	devmode.dmPelsWidth = width;
	devmode.dmPelsHeight = height;
	devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	if (ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) 
		== DISP_CHANGE_SUCCESSFUL) 
	{
		mode_changed = true;
		return true;
	}
	return false;
}


