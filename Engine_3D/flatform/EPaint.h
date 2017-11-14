//EPaint.h
//Paint functions' declaration
//Varies with library tools
//Author: Stophin
//2014.01.08
//Ver: 0.01
//
#ifndef _EPAINT_H_
#define _EPAINT_H_

#include "../common/EPoint.h"
#include "ELibrary.h"

#define DEF_FTCOLOR	BLACK
#define DEF_FFCOLOR	WHITE
#define DEF_FLCOLOR	LIGHTGRAY
#define DEF_BKCOLOR WHITE

typedef COLORREF	ECOLOR;
typedef PIMAGE		EIMAGE;
typedef char		EPCHAR;
typedef LPCTSTR		EPSTRING;
typedef bool		EPBOOL;
typedef int			INT;

EPoint EP_Init(EPTYPE wd,EPTYPE ht);

void EP_RenderStart();
void EP_RenderFlush(INT fps = 60);
void EP_RenderEnd();
void EP_Refresh();
void EP_Refresh(ECOLOR c);

void EP_Rectangle(const EPoint& p, const EPoint& w);
void EP_Rectangle(const RectF& p);
void EP_Rectangle(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey);
void EP_FillRectangle(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey);
void EP_InverseRect(const EPoint& lt,const EPoint& rb);

void EP_LoadImage(EIMAGE& image,LPCTSTR fname,EPTYPE& pwidth,EPTYPE& pheight);
void EP_CopyImage(EIMAGE& image,EIMAGE source);
EIMAGE EP_NewImage();
void EP_ResizeImage(EIMAGE& image,EPTYPE& pwidth,EPTYPE& pheight);
void EP_DeleteImage(EIMAGE& image);
DWORD* EP_GetImageBuffer(EIMAGE image=NULL);
ECOLOR EP_GetPixel(EPTYPE x,EPTYPE y,EIMAGE image=NULL);
void EP_SetPixel(EPTYPE x,EPTYPE y,ECOLOR c);

void EP_TruncateImage(EPTYPE x, EPTYPE y, EPTYPE w, EPTYPE h, EIMAGE image, EPTYPE tx, EPTYPE ty, EPTYPE tw, EPTYPE th, EIMAGE image_bk = NULL, INT bk = 0, ECOLOR c = BLACK);
void EP_PutImage(EPTYPE x,EPTYPE y,EIMAGE image);
EPBOOL EP_TruncImage(EPTYPE x, EPTYPE y, EIMAGE image, EIMAGE image_bk, ECOLOR c = BLACK, EPBOOL bk = false, const PEPoint cur = NULL, int imgtype = 0, EFTYPE rotation = EP_ZERO, EPTYPE vx = 0, EPTYPE vy = 0, EPTYPE sx = 0, EPTYPE sy = 0, EPTYPE ex = 0, EPTYPE ey = 0);
void EP_TruncImage(EPTYPE x,EPTYPE y,EIMAGE image,ECOLOR c=WHITE,int mode=0);
void EP_Line(const EPoint& s,const EPoint& e);
void EP_Line(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey);
void EP_Lines(PointF * points, int count);

void EP_SetColor(ECOLOR c);
void EP_SetFillColor(ECOLOR c);
void EP_SetBKColor(ECOLOR c);

int EP_GetImageWidth(EIMAGE image);
int EP_GetImageHeight(EIMAGE image);

void EP_Text(EPTYPE x,EPTYPE y,EPCHAR * c);
int EP_GetTextWidth(EPCHAR& c);
#endif //end of _EPAINT_H_
//end of file
