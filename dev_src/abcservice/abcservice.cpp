// abcservice.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include "stasm.hpp"
#include "wasm.hpp"
#include "winfile.hpp"

#define WC_BLACK RGB(0, 0, 0)
#define WC_WHITE RGB(255,255,255)
//
//
//
// 
static char         sgLogFile[SLEN];    // expanded log filename 
static RgbImage     gImg;               // current image
static RgbImage     gImgWithShape;      // current image with shape drawn
static char         sgFile[SLEN];       // filename of current image
static DET_PARAMS   gDetParams;         // face detector parameters
static SHAPE        gStartShape;        // start shape
static SHAPE        gShape;             // located landmarks
static char         sgDataDir[SLEN];    // dir for face detector files etc.
static char         sgConfFile0[SLEN];  // first config file, see Init()
static char         sgConfFile1[SLEN];  // second config file
static int          ngMainWidth;        // main window dimensions
static int          ngMainHeight;
static bool         fgSearching;        // true while in AsmSearch 
static bool         fgPaleface;
static bool         fgCrop = true;
static bool         fgConnectDots = true;
static bool         fgShowNbrs;


int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

