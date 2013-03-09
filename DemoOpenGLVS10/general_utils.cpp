#include "general_utils.h"
#include <math.h>
#include <OleCtl.h>
#include <GL/glut.h>

/**
 *	Olad a image 'szPathName' and convert it to a texture, stored in 'texid'.
 *	Returns 0 if fails, otherwise returns 1.
 *	
 *	The code is borrowed from this website with some minor modification:
 *	http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=41
 */
int BuildTexture(char *szPathName)
{
	HDC hdcTemp;						// The DC To Hold Our Bitmap
	HBITMAP hbmpTemp;					// Holds The Bitmap Temporarily
	IPicture *pPicture;					// IPicture Interface
	OLECHAR	wszPath[MAX_PATH+1];		// Full Path To Picture (WCHAR)
	char szPath[MAX_PATH+1];			// Full Path To Picture
	long lWidth;						// Width In Logical Units
	long lHeight;						// Height In Logical Units
	long lWidthPixels;					// Width In Pixels
	long lHeightPixels;					// Height In Pixels
	GLint glMaxTexDim ;					// Holds Maximum Texture Size

	if (strstr(szPathName, "http://"))				// If PathName Contains http:// Then...
	{
		strcpy(szPath, szPathName);					// Append The PathName To szPath
	}
	else											// Otherwise... We Are Loading From A File
	{
		GetCurrentDirectoryA(MAX_PATH, szPath);		// Get Our Working Directory
		strcat(szPath, "\\");						// Append "\" After The Working Directory
		strcat(szPath, szPathName);					// Append The PathName
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);		// Convert From ASCII To Unicode
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(hr != S_OK)								// If Loading Failed
	{
		return FALSE;							// Return False
	}

	hdcTemp = CreateCompatibleDC(GetDC(0));		// Create The Windows Compatible Device Context
	if(!hdcTemp)								// Did Creation Fail?
	{
		pPicture->Release();					// Decrements IPicture Reference Count
		return FALSE;							// Return False (Failure)
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);	// Get Maximum Texture Size Supported
	
	pPicture->get_Width(&lWidth);						// Get IPicture Width (Convert To Pixels)
	lWidthPixels = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);						// Get IPicture Height (Convert To Pixels)
	lHeightPixels = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// Resize Image To Closest Power Of Two
	if (lWidthPixels <= glMaxTexDim)		// Is Image Width Less Than Or Equal To Cards Limit
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else									// Otherwise  Set Width To "Max Power Of Two" That The Card Can Handle
		lWidthPixels = glMaxTexDim;
 
	if (lHeightPixels <= glMaxTexDim)		// Is Image Height Less Than Or Equal To Cards Limit
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else									// Otherwise  Set Height To "Max Power Of Two" That The Card Can Handle
		lHeightPixels = glMaxTexDim;

	// Create A Temporary Bitmap
	BITMAPINFO bi = {0};					// The Type Of Bitmap We Request
	DWORD *pBits = 0;						// Pointer To The Bitmap Bits

	bi.bmiHeader.biSize	= sizeof(BITMAPINFOHEADER);		// Set Structure Size
	bi.bmiHeader.biBitCount	= 32;						// 32 Bit
	bi.bmiHeader.biWidth = lWidthPixels;				// Power Of Two Width
	bi.bmiHeader.biHeight = lHeightPixels;		// Make Image Top Up (Positive Y-Axis)
	bi.bmiHeader.biCompression = BI_RGB;		// RGB Encoding
	bi.bmiHeader.biPlanes = 1;					// 1 Bitplane

	// Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
	
	if(!hbmpTemp)								// Did Creation Fail?
	{
		DeleteDC(hdcTemp);						// Delete The Device Context
		pPicture->Release();					// Decrements IPicture Reference Count
		return FALSE;							// Return False (Failure)
	}

	SelectObject(hdcTemp, hbmpTemp);			// Select Handle To Our Temp DC And Our Temp Bitmap Object

	// Render The IPicture On To The Bitmap
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// Convert From BGR To RGB Format And Add An Alpha Value Of 255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)			// Loop Through All Of The Pixels
	{
		BYTE* pPixel = (BYTE*)(&pBits[i]);	// Grab The Current Pixel
		BYTE  temp = pPixel[0];				// Store 1st Color In Temp Variable (Blue)
		pPixel[0] = pPixel[2];				// Move Red Value To Correct Position (1st)
		pPixel[2] = temp;					// Move Temp Value To Correct Blue Position (3rd)
		pPixel[3] = 255;					// Set The Alpha Value To 255
	}

	glTexImage2D(GL_TEXTURE_2D, 0, 3, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hbmpTemp);					// Delete The Object
	DeleteDC(hdcTemp);						// Delete The Device Context

	pPicture->Release();					// Decrements IPicture Reference Count

	return TRUE;							// Return True (All Good)
}

/**
 *	Calculate the angle in degree between 'v1' and 'v2'.
 */
float getDegreeBetweenVectors(float v1[3], float v2[3])
{
	double v1_mag = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
	double v2_mag = sqrt(v2[0]*v2[0] + v2[1]*v2[1] + v2[2]*v2[2]);
	double vv = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];

	return (float)(acos(vv/(v1_mag*v2_mag)) * 180 / PI);
}

/**
 *	Calculate the cross product of 'v1' and 'v2', store the result
 *	into 'result'.
 */
void calculateCrossProduct(float v1[3], float v2[3], float result[3])
{
	result[0] = v1[1]*v2[2] - v1[2]*v2[1];
	result[1] = v1[2]*v2[0] - v1[0]*v2[2];
	result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

/**
 *	Calculate the distance between 'p1' and 'p2'.
 */
double getDistanceBetweenPoints(float p1[3], float p2[3])
{
	double d0 = p2[0] - p1[0];
	double d1 = p2[1] - p1[1];
	double d2 = p2[2] - p1[2];
	return sqrt(d0 * d0 + d1 * d1 + d2 * d2);
}

/**
 *	Normalize vector 'v'.
 */
void normalize(float v[3])
{
	double mag = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	
	v[0] /= mag;
	v[1] /= mag;
	v[2] /= mag;
}

/**
 *	Add vector 'v1' with vector 'v2'.
 *	The result is stored back to 'v1' upon return.
 */
void addTwoVectors(float v1[3], float v2[3])
{
	v1[0] += v2[0];
	v1[1] += v2[1];
	v1[2] += v2[2];
}
