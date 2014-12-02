#include "oileffect.h" 
#include <math.h>
/************************************************************************/
/*
copyright (C) songtzu 2.17.12 night.if you use those code,plz contact songtzu@126.com to get a permission.
XiaMen.FuJian pro.
*/
/************************************************************************/
	/*
	Oil Painting Effect Details 
    Analysis of nearest pixels is explained with a real example. A pixel from the above image is considered to analyze the oil painting algorithm. This example considers Radius as 2 and Intensity as 10.
	For every pixel, the surrounding pixels are analysed. Find the maximum repeated pixels within the area, and place it as output. Processing of a pixel [X,Y] analyzes pixels from [X-Radius,Y-Radius] to [X+Radius,Y+Radius].
	Collapse | Copy Code
	/// Function to process Oil Painting Effect.
	// Parameters:
	pbyDataIn_i: Input RGB buffer of input image.
	nRadius:     Radius of processing. This values is used to consider nearest pixels.
		         If this value is high, processing cost will increase. 2 ~5 are good values.
	fIntensityLevels: Applied to r,g,b values intensity.
                  Increasing this values will create blocky output image.
	nWidth: Width of image.
	nHeight: Height of image.
	pbyDataOut_o : Output RGB buffer.
	*/
void oilEffectProcess(const BYTE* pbyDataIn_i,
	const int nRadius_i,
	const float fIntensityLevels_i,
	const int nWidth_i,
	const int nHeight_i,
	BYTE* pbyDataOut_o )
{
	int nIntensityCount[256];
	int nSumR[256];
	int nSumG[256];
	int nSumB[256];
	// Border pixes( depends on nRadius) will become black.
	// On increasing radius boundary pixels should set as black.
	memset( pbyDataOut_o, 0, nWidth_i * nHeight_i * 3 );

	// If total bytes in a row of image is not divisible by four, 
	// blank bytes will be padded to the end of the row.
	// nBytesInARow bytes are the actual size of a row instead of nWidth * 3.
	// If width is 9, then actual bytes in a row will will be 28, and not 27.
	int nBytesInARow = ceil( nWidth_i * 3 / 4.0 ) * 4.0; 
	// nRadius pixels are avoided from left, right top, and bottom edges.
	for( int nY = nRadius_i; nY < nHeight_i - nRadius_i; nY++)
	{
		for( int nX = nRadius_i; nX < nWidth_i - nRadius_i; nX++)
		{
			// Reset calculations of last pixel.
			memset( nIntensityCount, 0, sizeof(nIntensityCount) );
			memset( nSumR, 0, sizeof(nSumR) );
			memset( nSumG, 0, sizeof(nSumG) );
			memset( nSumB, 0, sizeof(nSumB) ); 
			// Find intensities of nearest nRadius pixels in four direction.
			for( int nY_O = -nRadius_i; nY_O <= nRadius_i; nY_O++ )
			{
				for( int nX_O = -nRadius_i; nX_O <= nRadius_i; nX_O++ )
				{
					int nR = pbyDataIn_i[( nX+nX_O) * 3  + ( nY + nY_O ) * nBytesInARow ];
					int nG = pbyDataIn_i[( nX+nX_O) * 3  + ( nY + nY_O ) * nBytesInARow + 1];
					int nB = pbyDataIn_i[( nX+nX_O) * 3  + ( nY + nY_O ) * nBytesInARow + 2]; 
					// Find intensity of RGB value and apply intensity level.
					int nCurIntensity =  ( ( ( nR + nG + nB ) / 3.0 ) * fIntensityLevels_i ) / 255;
					if( nCurIntensity > 255 )
						nCurIntensity = 255;
					int i = nCurIntensity;
					nIntensityCount[i]++; 
					nSumR[i] = nSumR[i] + nR;
					nSumG[i] = nSumG[i] + nG;
					nSumB[i] = nSumB[i] + nB;
				}
			}

			int nOutR = 0;
			int nOutG = 0;
			int nOutB = 0; 
			int nCurMax = 0;
			int nMaxIndex = 0;
			for( int nI = 0; nI < 256; nI++ )
			{
				if( nIntensityCount[nI] > nCurMax )
				{
					nCurMax = nIntensityCount[nI];
					nMaxIndex = nI;
				}
			}
			nOutR = nSumR[nMaxIndex] / nCurMax;
			nOutG = nSumG[nMaxIndex] / nCurMax;
			nOutB = nSumB[nMaxIndex] / nCurMax; 
			/*pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow ] = nOutR);
			pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow + 1] = nOutG);
			pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow + 2] = nOutB);*/
			/*
			pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow ] = pbyDataIn_i[( nX) * 3 + ( nY ) * nBytesInARow ]*sin((double)nOutR);
			pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow + 1] = pbyDataIn_i[( nX) * 3 + ( nY ) * nBytesInARow + 1]*sin((double)nOutG);
			pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow + 2] = pbyDataIn_i[( nX) * 3 + ( nY ) * nBytesInARow + 2]*sin((double)nOutB);
			*/

			pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow ] = pbyDataIn_i[( nX) * 3 + ( nY ) * nBytesInARow ];
			pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow + 1] = pbyDataIn_i[( nX) * 3 + ( nY ) * nBytesInARow + 1];
			pbyDataOut_o[( nX) * 3 + ( nY ) * nBytesInARow + 2] = pbyDataIn_i[( nX) * 3 + ( nY ) * nBytesInARow + 2];
		}
	}
}