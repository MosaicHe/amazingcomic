#include "stdafx.h"
#include <iostream>
//#include <string.h>
#include "Warper.h"
#include "WarperUtil.h"
#include <afxwin.h>
#include <GdiPlus.h>
#include <vector>  


using namespace ImageWarper; 
int m_iWarperType;  
int m_iRadius=40;
//vector::array m_pointArray;
Point m_Point[75];
Warper* m_warper;
ImageData imgData;
Gdiplus::Bitmap* m_bmpImage; 
Gdiplus::GdiplusStartupInput m_gdiplusStartupInput; 
ULONG_PTR m_gdiplusToken;
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}   
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}

void SaveImage(Gdiplus::Bitmap* bmpDest, WarpedImage* imgData)
{
	Gdiplus::Graphics* grp = Gdiplus::Graphics::FromImage(bmpDest);
	Gdiplus::Bitmap* bmp =new Gdiplus::Bitmap(imgData->Image.Width, imgData->Image.Height, grp);
	Gdiplus::BitmapData* bmpData =new Gdiplus::BitmapData();
	std::cout<<"width...."<<imgData->Image.Width<<"height  is  "<<imgData->Image.Height<<"\n";
	Gdiplus::Rect rect(0, 0, imgData->Image.Width, imgData->Image.Height);
	bmp->LockBits(
		&rect,
		Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite,
		PixelFormat24bppRGB,bmpData);
	//System::Diagnostics::Debug::Assert(bmpData->Stride == imgData->Image.ScanWidth);
	memcpy((char*)(void*)bmpData->Scan0, imgData->Image.Data, bmpData->Stride*bmpData->Height);
	bmp->UnlockBits(bmpData);
	CLSID aclsid;
	GetEncoderClsid(L"image/bmp", &aclsid);
	bmp->Save(L"D:\\abc.bmp",&aclsid,NULL);
	//grp->DrawImage(bmp, System::Drawing::Point(imgData->Position.X, imgData->Position.Y));

}
void InitialImage(const WCHAR *filename)
{ 
	std::cout<<"step 3 cout......\n";
	std::string sfilestr="D:\\3.bmp";
	m_bmpImage=new Gdiplus::Bitmap(filename);
	std::cout<<"step 6 is running ....\n";
	Gdiplus::BitmapData bmpData ;
	std::cout<<"step 10 is running ....\n";
	Gdiplus::Rect arect(0, 0,m_bmpImage->GetWidth(), m_bmpImage->GetHeight());   
	std::cout<<"step 7 is running ....\n";
	m_bmpImage->LockBits(&arect,Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite,PixelFormat24bppRGB,&bmpData);
	ImageData imgData; 
	std::cout<<"step 8 is running ....\n";
	std::cout<<"before width is "<<bmpData.Width<<"before height is "<<bmpData.Height<<"\n";
	imgData.Resize(bmpData.Width, bmpData.Height, 3, bmpData.Stride);
	memcpy(imgData.Data, (char*)(void*)bmpData.Scan0, bmpData.Stride*bmpData.Height);
	std::cout<<"step 9 is running ....\n";
	m_bmpImage->UnlockBits(&bmpData);
	m_warper = new Warper(imgData); 
} 

void ProcessImage()
{ 
	/*
	if (m_bmpImage &&
	pt.X >= 0 && pt.X < m_bmpImage->Width &&
	pt.Y >= 0 && pt.Y < m_bmpImage->Height)
	{
	WarpedImage* warpedImg = m_warper->UpdateWarp(pt);
	DrawImage(m_bmpImage, warpedImg, false);
	Invalidate();
	}
	*/
	///////////////////方式一
	 Point pt=Point(15,19);
	 m_iWarperType=WARPER_TRANSLATE;
	m_warper->BeginWarp(pt, m_iRadius, m_iWarperType);
	//m_bMouseDown = true;
	if (m_iWarperType != WARPER_TRANSLATE)
	{
	//m_timerGrowShrink->Enabled = true;
	}
	Point pt2=Point(50,50);
	WarpedImage* warpedImg = m_warper->UpdateWarp(pt2);
	if(warpedImg)
	{
		std::cout<<"step b is running ....\n";
		SaveImage(m_bmpImage,warpedImg);
	}
	

	///方式二。
	/*m_iWarperType=WARPER_GROW;
	ImageWarper::Point pt=ImageWarper::Point(10,10);
	m_warper->BeginWarp(pt, m_iRadius, m_iWarperType);
	WarpedImage* warpedImg;
		std::cout<<"step a is running ....\n";
	for (int i=0;i<5;i++)
	{
		if (m_bmpImage &&
			pt.X >= 0 && pt.X < m_bmpImage->GetWidth() &&
			pt.Y >= 0 && pt.Y < m_bmpImage->GetHeight())
		{
			warpedImg = m_warper->UpdateWarp(pt);
			std::cout<<"step 15 save image cout......\n";
		}
	} 
		std::cout<<"step b is running ....\n";
		SaveImage(m_bmpImage,warpedImg);*/
} 

void initialservice()
{
	std::cout<<"step 2 cout......\n";
	//InitialImage(); 
}
// 初始化GDI+库
void InitGuiPlus()
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
}

int _tmain(int argc, _TCHAR* argv[])
{  

	InitGuiPlus();
	std::cout<<"step 1 cout......\n"; 
	const WCHAR *name=argv[0];
	if (argc==1)
	{
		name=L"D:\\3.bmp";
	}
	std::cout<<"名字:   "<<name;
	InitialImage(name); 
	std::cout<<"step 5 is coming .....";
	ProcessImage();
	return 0; 
} 
