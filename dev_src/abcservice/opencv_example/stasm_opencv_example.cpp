// $stasm_opencv_example.cpp 3.0 milbo$ locate facial landmarks using the Stasm DLL
//
// To build, do something like (you may have to tweak the flags for your environment):
//
// cl -nologo -O2 -W3 -MT -EHsc -DWIN32 stasm_opencv_example.cpp \
//   -I%OPENCV_HOME%\include\opencv %OPENCV_HOME%\lib\cv210.lib \
//   %OPENCV_HOME%\lib\cxcore210.lib %OPENCV_HOME%\lib\highgui210.lib \
//   ..\data\stasm_dll.lib
/************************************************************************/
/* authour songtzu@126.com
	time 6.2012-3.2013
	thanks for carrot rol
	that is my girl friends!
	without her,I can't made it work
	*/
/************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "Warper.h"
#include "WarperUtil.h"
#include <afxwin.h>
#include <GdiPlus.h>
#include <math.h>
#include "oileffect.h"
#if _MSC_VER==1200 // hack for Visual C 6.0 (standard OpenCV .h files cause compiler errors)
  #include "../stasm/stasm_cv.h"
#else
  #include "cv.h"
  #include "highgui.h"
#endif
#include "../stasm/stasm_dll.hpp"   // for AsmSearchDll

#define RATE_NOISE 1.5f	//定义鼻子的扩宽比例。
#define RATE_MOUTH 1.1f	//定义嘴巴的扩宽比例。
#define RATE_EYE 1.1f	//定义眼睛的扩宽比例。
#define RATE_DEFAULT 1.1f	//默认比例。



using namespace ImageWarper; 
char *image_name;
int m_iWarperType;  
int m_iRadius=40; //此变量，初略估计要赋值为图片宽高的1/10.即width or height * 0.1。此值在initialImage方法计算数值！
Point mPoints[76];
Warper* m_warper;
const static float m_comparerate=1.1f;
ImageData imgData;
Gdiplus::Bitmap* m_bmpImage; 
Gdiplus::GdiplusStartupInput m_gdiplusStartupInput; 
ULONG_PTR m_gdiplusToken;
int mWidth;
int mWuMu;
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


 
	////////////////////////////////////////////////////////////////////////// 
//Here analyzing the nearest pixels of a pixel (X,Y), for this example, X,Y is at the center of the blue rectangle. The nearest pixels of the pixel X,Y is displayed in the right top corner. We will analyse the intensity of these nearest pixels and find out the final R, G, B for pixel (X,Y) which will help create an oil painting effect.
	//////////////////////////////////////////////////////////////////////////
  

/*
this method does not output image to file system!
save related code all included in saveImage method.
注意，此函数不保存图片到文件系统！
保存相关的见saveImage
*/
void updateImage(Gdiplus::Bitmap* bmpDest, WarpedImage* imgData)
{ 
	CLSID aclsid;
	GetEncoderClsid(L"image/bmp", &aclsid); 
	Gdiplus::Graphics* grp = Gdiplus::Graphics::FromImage(bmpDest);
	Gdiplus::Bitmap* bmp =new Gdiplus::Bitmap(imgData->Image.Width, imgData->Image.Height,grp); 
	Gdiplus::BitmapData* bmpData =new Gdiplus::BitmapData();
	std::cout<<"\nwidth is "<<imgData->Image.Width<<";  height  is  "<<imgData->Image.Height<<"\n";
	Gdiplus::Rect rect(0, 0, imgData->Image.Width, imgData->Image.Height);
	bmp->LockBits(
		&rect,
		Gdiplus::ImageLockModeRead|Gdiplus::ImageLockModeWrite,
		PixelFormat24bppRGB,
		bmpData);  
	memcpy((char*)(void*)bmpData->Scan0, imgData->Image.Data, bmpData->Stride*bmpData->Height);
	bmp->UnlockBits(bmpData);
	grp->DrawImage(bmp,Gdiplus::Point(imgData->Position.X,imgData->Position.Y)); 
}


void saveImage(Gdiplus::Bitmap* bmpDest, WarpedImage* imgData)
{ 
	CLSID aclsid;
	GetEncoderClsid(L"image/bmp", &aclsid);  
	Gdiplus::BitmapData* i_bitmapdata=new Gdiplus::BitmapData();
	Gdiplus::Rect i_rect(0,0,bmpDest->GetWidth(),bmpDest->GetHeight());
	bmpDest->LockBits(&i_rect,Gdiplus::ImageLockModeRead|Gdiplus::ImageLockModeWrite,PixelFormat24bppRGB,i_bitmapdata);
	BYTE *i_oil_byte_buff=new BYTE[i_bitmapdata->Stride*i_bitmapdata->Height];//(BYTE*)malloc(i_bitmapdata->Stride*i_bitmapdata->Height);
	BYTE *i_oil_byte_buff_out=new BYTE[i_bitmapdata->Stride*i_bitmapdata->Height];//(BYTE*)malloc(i_bitmapdata->Stride*i_bitmapdata->Height);
	memcpy(i_oil_byte_buff,i_bitmapdata->Scan0,i_bitmapdata->Stride*i_bitmapdata->Height);
	//oil style process!
	//then save image
	oilEffectProcess(i_oil_byte_buff,2,20,i_bitmapdata->Width,i_bitmapdata->Height,i_oil_byte_buff_out);
	memcpy((char*)(void*)i_bitmapdata->Scan0,i_oil_byte_buff_out,i_bitmapdata->Stride*i_bitmapdata->Height);
	bmpDest->Save(L"F:\\RESULT.bmp",&aclsid,NULL);
}

void InitialImage(const WCHAR *filename)
{  
	//std::string sfilestr="D:/test-image.jpg"; 
	m_bmpImage=new Gdiplus::Bitmap(filename); 
	Gdiplus::BitmapData bmpData ; 
	Gdiplus::Rect arect(0, 0,m_bmpImage->GetWidth(), m_bmpImage->GetHeight());  
	m_bmpImage->LockBits(&arect,Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite,PixelFormat24bppRGB,&bmpData);
	ImageData imgData;  
	std::cout<<"before width is "<<bmpData.Width<<"before height is "<<bmpData.Height<<"\n";
	imgData.Resize(bmpData.Width, bmpData.Height, 3, bmpData.Stride);
	memcpy(imgData.Data, (char*)(void*)bmpData.Scan0, bmpData.Stride*bmpData.Height); 
	m_bmpImage->UnlockBits(&bmpData);
	m_warper = new Warper(imgData); 
}
///
///处理方式二。点A到点B的处理。translate
///
void ProcessUnit(Point ptsrc,Point ptdes)
{ 
	m_iWarperType=WARPER_TRANSLATE;
	m_warper->BeginWarp(ptsrc, m_iRadius, m_iWarperType);
	int t_xpixel=ptdes.X-ptsrc.X;
	int xnege=0;
	if (ptsrc.X-ptdes.X>0)
	{
		xnege=-1;
	}else{
		xnege=1;
	}
	int ynege=1;
	if (ptsrc.Y-ptdes.Y>0)
	{
		ynege=-1;
	}
	float t_ypixel=ptdes.Y-ptsrc.Y;
	float t_rate=1;
	if (t_xpixel!=0)
	{
		 t_rate=(float)t_ypixel/(float)t_xpixel; 
		 for (int t_count=1;t_count<abs(t_xpixel);t_count++)
		 {
			 ImageWarper::Point t_updatepoint=Point(ptsrc.X+xnege*t_count,ptsrc.Y+t_count*t_rate*ynege);
			 WarpedImage* warpedImg = m_warper->UpdateWarp(t_updatepoint); 
			 if(warpedImg)
			 { 
				 updateImage(m_bmpImage,warpedImg);
			 }
			 //t_count++;
		 }







	}else{
	 
		 
		for (int t_count=1;t_count<abs(t_ypixel);t_count++)
		{
			ImageWarper::Point t_updatepoint=Point(ptsrc.X+t_count,ptsrc.Y+t_count);
			WarpedImage* warpedImg = m_warper->UpdateWarp(t_updatepoint); 
			if(warpedImg)
			{ 
				updateImage(m_bmpImage,warpedImg);
			}
			//t_count++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////处理方式1，一个点Grow，放大镜效果，shrink效果。
void ProcessUnit(Point pt,bool isGrow)
{ 
	if (isGrow)
	{
		m_iWarperType=WARPER_GROW;
	}
	else
	{
		m_iWarperType=WARPER_SHRINK;
	}
	m_warper->BeginWarp(pt, m_iRadius, m_iWarperType); 
	WarpedImage* warpedImg = m_warper->UpdateWarp(pt);
	if(warpedImg)
	{ 
		warpedImg = m_warper->EndWarp(warpedImg); 
		updateImage(m_bmpImage, warpedImg);
	}  
}
 
// 初始化GDI+库
void InitGuiPlus()
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
}
//计算比例，自动化处理
void AutoMaticProcess()
{
	mWuMu=mPoints[43].X-mPoints[39].X;
	mWidth=abs(mPoints[0].Y-mPoints[14].Y);
    /*算法表述：
	原则：
	一，算法需要慢慢改进，不必过多纠结。
	二，必须要有实际的效果拿出来对比。
	三，初始算法可以只有一种。
	算法：
	三庭为中挺长，则中庭扩大，下庭，上庭均缩小。
	中庭明显小，则缩小中庭，扩大上下两庭。
	三庭差距不明显，则默认处理，下庭扩大，中庭小幅扩大，上庭缩小。
	*/ 
	/************************************************************************/
	int t_middleheight=abs(mPoints[21].Y-mPoints[40].Y);
	int t_bottomheight=abs(mPoints[40].Y-mPoints[7].Y);
	if (t_middleheight>t_bottomheight*m_comparerate)/*中庭过长*/
	{ 
		Point temp=Point(100,100);  
	}
	else if(t_middleheight*m_comparerate<t_bottomheight)/*否则，中庭过短，先不处理。*/
	{
	
	}
	else/*中庭比例合适，下庭放大，中庭适当放大，上庭缩小。通常伴随鼻子放大，嘴巴放大
		 先以此为例，做出初步的运行效果，然后在评估后面的工作安排*/
	{
		//ProcessUnit(m_Point[67],true);
	} 
 
	//////////////////////////////////////////////////////////////////////////右边脸的处理。
	//int t_noisewidth=abs(mPoints[39].X-mPoints[43].X); 
	/////0
	if (mWuMu>50)
	{
		mWuMu=40;
	}
	//std::cout<<"五目 de chang du shi "<<mWuMu<<"\n";
	//std::cout<<"mpoint[0] x "<<mPoints[0].X<<" y "<<mPoints[0].Y;

	//Point pt=Point(mPoints[0].X+mWuMu*0.3,mPoints[0].Y);
	//ProcessUnit(mPoints[0],pt);
	//std::cout<<"mpoint[1] x "<<mPoints[1].X<<" y "<<mPoints[1].Y;
	/////////1
	//pt= Point(mPoints[1].X,mPoints[1].Y+mWuMu*0.1);
	//ProcessUnit(mPoints[1],pt);
	////2
	//pt= Point(mPoints[2].X+mWuMu*0.2,mPoints[2].Y-mWuMu*0.2);
	//ProcessUnit(mPoints[2],pt);
	/////3
	//pt= Point(mPoints[3].X+mWuMu*0.3,mPoints[3].Y-mWuMu*0.5);
	//ProcessUnit(mPoints[3],pt);
	/////4
	//pt= Point(mPoints[4].X+mWuMu*0.6,mPoints[4].Y-mWuMu*0.7);
	//ProcessUnit(mPoints[4],pt);
	/////5
	//pt= Point(mPoints[5].X+mWuMu*0.7,mPoints[5].Y-mWuMu*0.8);
	//ProcessUnit(mPoints[5],pt);
	/////6
	//pt= Point(mPoints[6].X+mWuMu*0.9,mPoints[6].Y-mWuMu*0.9);
	//ProcessUnit(mPoints[6],pt);



	Point pt=Point(mPoints[0].X+mWuMu*0.1,mPoints[0].Y);
	Point otherpoint=Point(mPoints[0].X-mWuMu*0.3,mPoints[0].Y-mWuMu*0.8);
	pt=Point(otherpoint.X+mWuMu*0.5,otherpoint.Y);
	ProcessUnit(otherpoint,pt);
    pt=Point(mPoints[0].X+mWuMu*0.3,mPoints[0].Y);
	ProcessUnit(mPoints[0],pt);
	//Point pt=Point(mPoints[0].X+mWuMu*0.1,mPoints[0].Y);
	//ProcessUnit(mPoints[0],pt);
	///////1
	pt= Point(mPoints[1].X+mWuMu*0.1,mPoints[1].Y);
	ProcessUnit(mPoints[1],pt);
	//2
	pt= Point(mPoints[2].X-mWuMu*0.1,mPoints[2].Y+mWuMu*0.2);
	ProcessUnit(mPoints[2],pt);
	///3
	pt= Point(mPoints[3].X-mWuMu*0.1,mPoints[3].Y+mWuMu*0.3);
	ProcessUnit(mPoints[3],pt);
	///4
	pt= Point(mPoints[4].X-mWuMu*0.4,mPoints[4].Y+mWuMu*0.6);
	ProcessUnit(mPoints[4],pt);
	///5
	pt= Point(mPoints[5].X-mWuMu*0.7,mPoints[5].Y+mWuMu*0.7);
	ProcessUnit(mPoints[5],pt);
	///6
	pt= Point(mPoints[6].X-mWuMu*0.9,mPoints[6].Y+mWuMu*0.9);
	ProcessUnit(mPoints[6],pt);
 //
	////7
	pt=Point(mPoints[7].X,mPoints[7].Y+mWuMu);
	ProcessUnit(mPoints[7],pt);
	////8
	pt=Point(mPoints[8].X+mWuMu*0.5,mPoints[7].Y+mWuMu*0.7);
	ProcessUnit(mPoints[8],pt);
	////9
	pt=Point(mPoints[9].X+mWuMu*0.8,mPoints[9].Y+mWuMu*0.6);
	ProcessUnit(mPoints[9],pt);
	////10
	pt=Point(mPoints[10].X+mWuMu*0.9,mPoints[10].Y+mWuMu*0.4);
	ProcessUnit(mPoints[10],pt);
	////11
	pt=Point(mPoints[11].X+mWuMu*0.9,mPoints[11].Y+mWuMu*0.2);
	ProcessUnit(mPoints[11],pt);

	//处理嘴巴附近的区域。
	pt=Point(mPoints[48].X-mWuMu*0.5,mPoints[48].Y);
	ProcessUnit(mPoints[48],pt);
	saveImage(m_bmpImage,NULL);
}


void DoAction()
{
	//////////////////////////////////////////////////////////////////////////initial GDI+
	InitGuiPlus();
	//////////////////////////////////////////////////////////////////////////initial image
	/*InitialImage(L"D:/test-image.jpg"); */
	InitialImage(L"F:/SONGTZU/abc.jpg"); 
	//wcscpy(temp,)
	//MultiByteToWideChar(CP_ACP,0,image_name,)
	//InitialImage(muti(image_name));
	//////////////////////////////////////////////////////////////////////////process
	AutoMaticProcess();	
	//saveImagetoFile();
}



int main (void)
{
 	image_name = "F:/SONGTZU/abc.jpg";  
    IplImage *img = cvLoadImage(image_name, CV_LOAD_IMAGE_COLOR);
    if(img == NULL) {
        printf("Error: Cannot open %s\n", image_name);
        return -1;
    }
    // sanity checks (AsmSearchDll assumes imageData is vector of b,r,g bytes)
    if(img->nChannels != 3 || img->depth != IPL_DEPTH_8U ||
            img->origin != 0 || img->widthStep != 3 * img->width) {
        printf("Error: %s is an unrecognized image type\n", image_name);
        //return -1;
    }
    // locate the facial landmarks with stasm
    int nlandmarks;
    int landmarks[500]; // space for x,y coords of up to 250 landmarks
    AsmSearchDll(&nlandmarks, landmarks,
                 image_name, img->imageData, img->width, img->height,
                 1 /* is_color */, NULL /* conf_file0 */, NULL /* conf_file1 */);
    if (nlandmarks == 0) {
        printf("\nError: Cannot locate landmarks in %s\n", image_name);
        return -1;
    }
//#if 0 // print the landmarks if you want
//    printf("landmarks:\n");
//    for (int i = 0; i < nlandmarks; i++)
//        printf("%3d: %4d %4d\n", i, landmarks[2 * i], landmarks[2 * i + 1]);
//#endif
	//////////////////////////////////////////////////////////////////////////draw point information
	printf("landmarks:\n");
	for (int i = 0; i < nlandmarks; i++)
	{
		Point _p=Point(landmarks[2*i],landmarks[2*i+1]);
		mPoints[i]=_p;
		printf("%3d: %4d %4d\n", i, landmarks[2 * i], landmarks[2 * i + 1]);
	}
	//////////////////////////////////////////////////////////////////////////draw point information
    // draw the landmarks on the image
    //assert(sizeof(int) == 4); // needed for CvPoint typecast below
    //int *p = landmarks;
    //cvPolyLine(img, (CvPoint **)&p, &nlandmarks, 1, 1, CV_RGB(255,0,0));
    // show the image with the landmarks
	/*
	cvShowImage("stasm example", img);
	cvWaitKey(0);
	cvDestroyWindow("stasm example");*/
	//cvSaveImage("D:\general_botton_blue.bmp",img);
    cvReleaseImage(&img);
	int abc=0;
	DoAction();
	/*InitGuiPlus();*/
    return 0;
}



/*
整体商业化框架的构思笔记：
当然，此处只限于技术的问题！总的来说，可以采用的方式有二：
一，http协议！这种协议易于后期的客户端的扩展！
二，tcp协议。即全C++代码方式！socket上传图片，调用处理代码，处理完成，socket下发给客户端！
弊端有两个，一，客户端复杂。二，web版本不清楚怎么实现。

*/