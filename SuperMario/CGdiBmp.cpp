/************************************************************************/
/*																		*/
/* 位图的创建、加载与绘制(CGdiBmp类的实现)    						    */
/* 采用Windows API方式实现												*/
/*																		*/
/* 作者：万立中	(C)2009-09-05											*/
/* www.wanlizhong.com													*/
/*																		*/
/************************************************************************/

#include "CGdiBmp.h"

//-----------------------------------------------------------------
//
// CGdiBmp类默认构造函数
//
//-----------------------------------------------------------------
CGdiBmp::CGdiBmp():hBitmap(NULL)
{
}

#include "stdio.h"

//-----------------------------------------------------------------
//
// CGdiBmp类构造函数，使用指定的文件来创建一个Bitmap对象
//
//-----------------------------------------------------------------
CGdiBmp::CGdiBmp(LPTSTR fileName):hBitmap(NULL)
{
	
	LoadBmp(fileName);
}

//-----------------------------------------------------------------
//
// CGdiBmp类构造函数，创建一个指定宽、高的空Bitmap对象
//
//-----------------------------------------------------------------
CGdiBmp::CGdiBmp(HDC hDC, int ibWidth, int ibHeight, COLORREF crColor)
	: hBitmap(NULL)
{
	CreateBlankBMP(hDC, ibWidth, ibHeight, crColor);
}

//-----------------------------------------------------------------
//
// CGdiBmp类析构函数
//
//-----------------------------------------------------------------
CGdiBmp::~CGdiBmp()
{
	Destroy();
}

//-----------------------------------------------------------------
//
// CGdiBmp类释放资源的方法
//
//-----------------------------------------------------------------
void CGdiBmp::Destroy()
{
	if (hBitmap != NULL)
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}
}

//-----------------------------------------------------------------
//
// CGdiBmp类创建空白位图函数
//
//-----------------------------------------------------------------
HBITMAP CGdiBmp::CreateBlankBMP (HDC hDC, int ibWidth, int ibHeight, COLORREF crColor ) 
{
	//创建一个空的位图
	HBITMAP bitmap = CreateCompatibleBitmap(hDC, ibWidth, ibHeight);
	if (bitmap == NULL) return FALSE;

	//创建一个内存设备用来绘制位图
	HDC memDC = CreateCompatibleDC(hDC);

	//创建一个单色画刷，用于填充所创建的空位图
	HBRUSH hBrush = CreateSolidBrush(crColor);

	//将创建的空位图替换内存设备中的原来对象
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, bitmap);

	//用制定的颜色填充位图
	RECT rcBitmap = { 0, 0, ibWidth, ibHeight };
	FillRect(memDC, &rcBitmap, hBrush);

	//还原：使用原来对象替换内存设备中的位图对象
	SelectObject(memDC, hOldBitmap);
	//删除内存设备
	DeleteDC(memDC);
	//删除笔刷
	DeleteObject(hBrush);
	return bitmap;
}




//-----------------------------------------------------------------
//
// CGdiBmp类加载位图函数
//
//-----------------------------------------------------------------
BOOL CGdiBmp::LoadBmp( LPTSTR path )
{
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(0), path, 
		IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | 
		LR_LOADFROMFILE);
	if (!hBitmap) return FALSE;
	DIBSECTION	dib;
	if (GetObject(hBitmap, sizeof(DIBSECTION), &dib) != sizeof(DIBSECTION)) {
		DeleteObject(hBitmap);
		hBitmap = 0;
		return FALSE;
	}
	bmpWidth = dib.dsBmih.biWidth;
	bmpHeight =dib.dsBmih.biHeight;
	return TRUE;
}

//-----------------------------------------------------------------
//
// CGdiBmp类绘制镜像位图函数
//
//-----------------------------------------------------------------
void CGdiBmp::DrawMirror(HDC hDC, int x, int y, int mirror)
{
	//为位图对象创建一个内存设备
	HDC memDC = CreateCompatibleDC(hDC);

	//将位图替换内存设备中的原来对象
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

	if(mirror==0)
	{
		//原始图像
		StretchBlt(	hDC, x, y, bmpWidth, bmpHeight, 
			memDC, 0, 0, bmpWidth, bmpHeight, 
			SRCCOPY);
	}

	if(mirror==1)
	{
		//水平镜像：源x坐标应等于图像宽度，y坐标为0，图像宽度为负数
		StretchBlt(	hDC, x, y, bmpWidth, bmpHeight, 
			memDC, bmpWidth, 0, -bmpWidth, bmpHeight, 
			SRCCOPY);
	}

	if(mirror == 2)
	{
		//垂直镜像：源y坐标应等于图像高度，x坐标为0，图像高度为负数
		StretchBlt(	hDC, x, y, bmpWidth, bmpHeight, 
			memDC, 0, bmpHeight, bmpWidth, -bmpHeight, 
			SRCCOPY);
	}

	//还原：使用原来对象替换内存设备中的位图对象
	SelectObject(memDC, hOldBitmap);
	//删除内存设备
	DeleteDC(memDC);
	//删除位图对象
	DeleteObject(hOldBitmap);
	hOldBitmap = NULL;
}

//-----------------------------------------------------------------
//
// CGdiBmp类绘制源图像中局部图像函数
//
//-----------------------------------------------------------------
void CGdiBmp::DrawSingleFrame(HDC hDC, int x, int y, 
	int xFrame, int yFrame, 
	int wFrame, int hFrame,
	double ratio, int mirror,
	COLORREF crTransColor)
{

	//创建全图的内存设备
	HDC memDC = CreateCompatibleDC(hDC);
	//将源图选进内存设备中
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

	//创建一个空白的帧图
	HBITMAP BlankFrameBmp = CreateBlankBMP(hDC, wFrame, hFrame, crTransColor);
	//创建帧图的内存设备
	HDC FrameDC = CreateCompatibleDC(hDC);
	//将空白的帧图选进内存设备
	HBITMAP OldFrameBmp = (HBITMAP)SelectObject(FrameDC, BlankFrameBmp);

	//从全图内存设备中将指定行列的帧图拷贝到帧图内存设备
	StretchBlt( FrameDC, 0, 0, wFrame, hFrame, 
		memDC, xFrame, yFrame, wFrame, hFrame, 
		SRCCOPY);

	//使用原来对象替换全图内存设备中的位图对象
	SelectObject(memDC, hOldBitmap);
	//删除全图内存设备
	DeleteDC(memDC);
	//删除位图对象
	DeleteObject(BlankFrameBmp);
	BlankFrameBmp = NULL;
	//删除位图对象
	DeleteObject(hOldBitmap);
	hOldBitmap = NULL;

	//重新建立一个帧图的内存设备
	memDC = CreateCompatibleDC(hDC);
	BlankFrameBmp = CreateBlankBMP(hDC, wFrame, hFrame, crTransColor);
	//将位图替换内存设备中的原来对象
	hOldBitmap = (HBITMAP)SelectObject(memDC, BlankFrameBmp);

	if(mirror==0)  //保持原图
	{
		StretchBlt(memDC, 0, 0, wFrame, hFrame, FrameDC, 0, 0, wFrame, hFrame, SRCCOPY);
	}

	if(mirror==1)//原图水平镜像(即根据Y轴旋转180, 也就是顺时针旋转180)
	{
		//水平镜像：源坐标x应等于图像宽，y坐标为0，图像宽为负数
		StretchBlt(memDC, 0, 0, wFrame, hFrame, FrameDC, wFrame, 0, -wFrame, hFrame, SRCCOPY);
	}

	if(mirror == 2)//原图垂直镜像(即根据X轴旋转180, 也就是顺时针旋转90)
	{
		//垂直镜像：源坐标y应等于图像高，x坐标为0，图像高为负数
		StretchBlt(memDC, 0, 0, wFrame, hFrame, FrameDC, 0, hFrame, wFrame, -hFrame, SRCCOPY);
	}

	if(mirror == 3)//原图先水平镜像,在进行垂直镜像
	{
		//创建一个空白的帧图
		HBITMAP Blank = CreateBlankBMP(hDC, wFrame, hFrame, crTransColor);
		//创建帧图的内存设备
		HDC dc = CreateCompatibleDC(hDC);
		//将空白的帧图选进内存设备
		HBITMAP Old = (HBITMAP)SelectObject(dc, Blank);
		//水平镜像
		StretchBlt(dc, 0, 0, wFrame, hFrame, FrameDC, wFrame, 0, -wFrame, hFrame, SRCCOPY);
		//垂直镜像
		StretchBlt(memDC, 0, 0, wFrame, hFrame, dc, 0, hFrame, wFrame, -hFrame, SRCCOPY);

		SelectObject(dc, Old);
		DeleteDC(dc);
	}

	TransparentBlt( hDC, x, y, (int)(wFrame*ratio+0.5), (int)(hFrame*ratio+0.5), 
		memDC, 0, 0, wFrame, hFrame, 
		crTransColor);

	//还原：使用原来对象替换内存设备中的位图对象
	SelectObject(memDC, hOldBitmap);
	SelectObject(FrameDC, OldFrameBmp);
	//删除内存设备
	DeleteDC(memDC);
	DeleteDC(FrameDC);
	//删除位图对象
	DeleteObject(hOldBitmap);
	hOldBitmap = NULL;
	//删除位图对象
	DeleteObject(OldFrameBmp);
	OldFrameBmp = NULL;
	//删除位图对象
	DeleteObject(BlankFrameBmp);
	BlankFrameBmp = NULL;
}

void CGdiBmp::DrawAnimatedFrames(HDC hDC,int x,int y,int FrameCount,int RowFrames,
	int wFrame,int hFrame,double ratio,int mirror,
	COLORREF crTransColor)
{
	int col=FrameCount%RowFrames;
	int row=(FrameCount-col)/RowFrames;
	DrawSingleFrame(hDC,x,y,col*wFrame,row*hFrame,
		wFrame,hFrame,ratio,mirror,crTransColor);
}

void CGdiBmp::DrawWords(HDC hDC, LPTSTR text, RECT FontRect)
{

}
//--------------------------------------
//CGdiBmp类绘制透明文字的函数（可定义方式）
//---------------------------------------
void CGdiBmp::DrawWords(HDC hDC,LPTSTR text,COLORREF FontColor,
	int FontSize,LPCTSTR FontName, RECT FontRect,
	int Align)
{
	HFONT hFont;
	hFont=CreateFont(
		FontSize,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
		OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,
		VARIABLE_PITCH|PROOF_QUALITY,FF_DONTCARE,FontName);
	SelectObject(hDC,hFont);
	TCHAR szText[512];
	wsprintf(szText,"%s",text);
	SetBkMode(hDC,TRANSPARENT);
	SetTextColor(hDC,FontColor);
	DrawText(hDC,szText,-1,&FontRect,
		DT_SINGLELINE|Align|DT_VCENTER);
	DeleteObject(hFont);

}