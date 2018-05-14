//EPaint.cpp
//Paint functions
//Varies with GDI tools
//Author: Stophin
//2014.01.08
//Ver: 0.01
//


#include "EPaint.h"

void EP_RenderStart()
{
	setrendermode(RENDER_MANUAL);
}

void EP_RenderFlush(INT fps)
{
	delay_fps(fps);
}

void EP_RenderEnd()
{
	return;
}

void EP_Refresh()
{
	cleardevice();
}

void EP_Refresh(ECOLOR c)
{
	setbkcolor(c);
}

EPoint EP_Init(EPTYPE wd,EPTYPE ht)
{
	EPoint gm(wd,ht);
	if (wd<=0||ht<=0)
	{
		//full screen
 		int scrwd=GetSystemMetrics(SM_CXSCREEN);
		int scrht=GetSystemMetrics(SM_CYSCREEN);
		initgraph(scrwd, scrht, INIT_RENDERMANUAL);
		HWND hWnd=getHWnd();
		SetWindowLong(hWnd,GWL_STYLE,GetWindowLong(hWnd,GWL_STYLE)-WS_CAPTION);
		SetWindowPos(hWnd,HWND_TOP,0,0,scrwd,scrht,SWP_SHOWWINDOW);
		gm.Set(scrwd,scrht);
	}
	else
	{
		initgraph(wd, ht, INIT_RENDERMANUAL);
	}
	return gm;
}

void EP_Rectangle(const EPoint& p,const EPoint& w)
{
	rectangle(p.x,p.y-w.y,p.x+w.x,p.y);
}

void EP_Rectangle(const ERectF& p)
{
	rectangle(p.X, p.Y, p.X + p.Width, p.Y + p.Height);
}
void EP_Rectangle(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey)
{
	rectangle(sx,sy,ex,ey);
}

void EP_FillRectangle(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey)
{
	bar(sx,sy,ex,ey);
}

void EP_InverseRect(const EPoint& lt,const EPoint& gm)
{
	DWORD* dst=EP_GetImageBuffer();
	int width=getwidth();
	int height=getheight();
	int i,j;
	COLORREF c;
	int pxl_idx;
	for (i=lt.x+1;i<lt.x+gm.x;i++)
		for (j=lt.y+1;j<lt.y+gm.y;j++)
		{
			if (i==lt.x+1||j==lt.y+1||i==lt.x+gm.x-1||j==lt.y+gm.y-1)
			{
				if (j>height||i>width)
					continue;
				pxl_idx=j*width+i;
				if (pxl_idx<0||pxl_idx>=width*height)
					break;

			//	COLORREF c=getpixel(i,j);
				c=dst[pxl_idx];
			//	putpixel(i,j,~c);
				dst[pxl_idx]=~c;
			}
	}
}

void EP_Line(const EPoint& s,const EPoint& e)
{
	line(s.x,s.y,e.x,e.y);
}

void EP_Line(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey)
{
	line(sx,sy,ex,ey);
}

void EP_Lines(EPointF * points, int count)
{
	for (int i = 0; i < count - 1; i++)
	{
		EP_Line(points[i].X, points[i].Y, points[i + 1].X, points[i + 1].Y);
	}
}

void EP_SetColor(ECOLOR c)
{
	setcolor(c);
}

void EP_SetFillColor(ECOLOR c)
{
	//setfillstyle(1,c);
}

void EP_SetBKColor(ECOLOR c)
{
	setfontbkcolor(c);
}

ECOLOR EP_ColorConvert(const EPCHAR * c) {
	ECOLOR col = BLACK;
	if (!strcmp(c, "CYAN")) {
		col = CYAN;
	}
	else if (!strcmp(c, "MAGENTA")) {
		col = MAGENTA;
	}
	else if (!strcmp(c, "BROWN")) {
		col = BROWN;
	}
	else if (!strcmp(c, "LIGHTGRAY")) {
		col = LIGHTGRAY;
	}
	else if (!strcmp(c, "DARKGRAY")) {
		col = DARKGRAY;
	}
	else if (!strcmp(c, "LIGHTBLUE")) {
		col = LIGHTBLUE;
	}
	else if (!strcmp(c, "LIGHTGREEN")) {
		col = LIGHTGREEN;
	}
	else if (!strcmp(c, "LIGHTCYAN")) {
		col = LIGHTCYAN;
	}
	else if (!strcmp(c, "LIGHTRED")) {
		col = LIGHTRED;
	}
	else if (!strcmp(c, "LIGHTMAGENTA")) {
		col = LIGHTMAGENTA;
	}
	else if (!strcmp(c, "YELLOW")) {
		col = YELLOW;
	}
	else if (!strcmp(c, "WHITE")) {
		col = WHITE;
	}
	else if (!strcmp(c, "GREEN")) {
		col = GREEN;
	}
	else if (!strcmp(c, "RED")) {
		col = RED;
	}
	else if (!strcmp(c, "BLUE")) {
		col = BLUE;
	}
	return col;
}

//Load image width file name, and resize
//image				: the image buffer
//fname				: file name
//pwidth&pheight	: Resize to pwidth*pheight, 
//					if pwidth or pheight is less than or equals to 0
//					image will not be resized, and be kept the same
//					with the origin size when loaded.
//					reference symbol '&' means  that this function
//					can change these parameters, so that it can
//					get image size when loaded.
void EP_LoadImage(EIMAGE& image,LPCTSTR fname,EPTYPE& pwidth,EPTYPE& pheight)
{
	//Delete old image and prepare new image
	if (image)
		delimage(image);
	image=newimage();

	//Load image with file name
	//if load error, delete image and return
	if (getimage(image,fname))
	{
// 		delimage(image);
// 		image=NULL;
// 		return;
	}
	//Resize image to pwidth*pheight
	EP_ResizeImage(image,pwidth,pheight);
}


void EP_ResizeImage(EIMAGE& image,EPTYPE& pwidth,EPTYPE& pheight)
{
	//Resize image to pwidth*pheight
	///////////////////////////////////////
	PIMAGE save=gettarget();
	//Get image size
	settarget(image);	
	int width,height;
	width=getwidth();
	height=getheight();
	settarget(save);
	//Resize not available
	if (pwidth<=0||pheight<=0)
	{
		pwidth=width;
		pheight=height;
		return;
	}
	//Prepare temporary image
	PIMAGE backproc=NULL;
	backproc=newimage(pwidth,pheight);
	//Resize
	putimage(backproc,0,0,pwidth,pheight,image,0,0,width,height);
	getimage(image,backproc,0,0,pwidth,pheight);
	delimage(backproc);
	//////////////////////////////////////
}

//Truncate picture by color c in itself
//mode: 1: only when current != c then paint
//		2: only when current == c then paint
//		0: all color will be painted
//	other: no response
void EP_TruncImage(EPTYPE x,EPTYPE y,EIMAGE image,ECOLOR c,int mode)
{
	if (image==NULL)
		return;

	DWORD* dst=EP_GetImageBuffer();
	DWORD* src=EP_GetImageBuffer(image);
	int width=getwidth();
	int height=getheight();
	if (dst==NULL||src==NULL)
		return;
	int img_w=getwidth(image);
	int img_h=getheight(image);
	int src_idx,pxl_idx;
	for (int mvy=0;mvy<img_h;mvy++)
	{
		for (int mvx=0;mvx<img_w;mvx++)
		{
			src_idx=mvy*img_w+mvx;
			if ((y+mvy)>height||(x+mvx)>width)
				continue;
			pxl_idx=(y+mvy)*width+x+mvx;
			if (pxl_idx<0||pxl_idx>=width*height)
					break;
			if (mode)
			{
				if (mode==1&&src[src_idx]!=c)
				{
				//	putpixel(x+mvx,y+mvy,src[src_idx]);
					dst[pxl_idx]=src[src_idx];
				}
				else if (mode==2&&src[src_idx]==c)
				{
				//	putpixel(x+mvx,y+mvy,src[src_idx]);
					dst[pxl_idx]=src[src_idx];
				}
			}
			else
			{
			//	putpixel(x+mvx,y+mvy,src[src_idx]);
				dst[pxl_idx]=src[src_idx];
			}
		}
	}	
}

void EP_TruncateImage(EPTYPE x, EPTYPE y, EPTYPE w, EPTYPE h, EIMAGE image, EPTYPE tx, EPTYPE ty, EPTYPE tw, EPTYPE th, EIMAGE image_bk, INT bk, ECOLOR c)
{
	int reverse = 0;
	if (w < 0) {
		w = -w;
		reverse = 1;
	}
	if (image == NULL)
		return;
	if (image_bk == NULL && bk > 1)
	{
		bk = 0;
	}

	setcolor(WHITE);

	DWORD* dst = EP_GetImageBuffer();
	DWORD* src = EP_GetImageBuffer(image);
	DWORD* msk = EP_GetImageBuffer(image_bk);
	if (dst == NULL || src == NULL)
		return;
	if (image_bk && msk == NULL)
	{
		return;
	}
	int width = getwidth();
	int height = getheight();
	int pxl_idx = 0;
	int src_idx = 0;
	int msk_idx = 0;
	int src_w = getwidth(image);
	int src_h = getheight(image);

	int msk_w = getwidth(image_bk);
	int msk_h = getheight(image_bk);
	if (x > width || y > height)
	{
		return;
	}

	pxl_idx = ((int)(y)* width + x);
	src_idx = ((int)(ty)* src_w + tx);

	float rate_x = tw / w;
	float rate_y = th / h;

	float mask_x = msk_w / (float)src_w;
	float mask_y = msk_h / (float)src_h;
	msk_idx = ((int)(ty * mask_y)* msk_w + (tx * mask_x));


	int i, j;
	if (msk_w < src_w)
	{
		i = 10;
	}

	int pxl_idx_d = pxl_idx, src_idx_d = pxl_idx, msk_idx_d = msk_idx;
	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (x + i < 0 || x + i > width)
			{
				continue;
			}
			if (y + j < 0 || y + j > height)
			{
				continue;
			}
			pxl_idx_d = pxl_idx + (j)* width + i;
			if (reverse) {
				pxl_idx_d = pxl_idx + (j)* width + width - i;
			}
			if (dst[pxl_idx_d] != 0x00000000)
			{
				continue;
			}
			src_idx_d = src_idx + ((int)(j * rate_y)) * src_w + (int)(i * rate_x);
			if (pxl_idx_d < 0 || pxl_idx_d >= width * height)
			{
				return;
			}
			if (src_idx_d < 0 || src_idx_d >= src_w * src_h)
			{
				return;
			}
			if (bk == 0)
			{
				dst[pxl_idx_d] = src[src_idx_d];
			}
			else if (bk == 1)
			{
				if ((src[src_idx_d] & 0x00FFFFFF) != c)
				{
					dst[pxl_idx_d] = src[src_idx_d];
				}
			}
			else
			{
				if (image_bk)
				{
					msk_idx_d = msk_idx + ((int)(j * rate_y * mask_y)) * msk_w + (int)(i * rate_x * mask_x);
					if (msk_idx_d < 0 || msk_idx_d >= msk_w * msk_h)
					{
						continue;
					}
					if ((msk[msk_idx_d] & 0x00FFFFFF) == c)
					{
						if (bk == 2)
						{
							dst[pxl_idx_d] = src[src_idx_d];
						}
					}
					else
					{
						if (bk == 3)
						{
							dst[pxl_idx_d] = src[src_idx_d];
						}
					}
					
				}
			}
		}
	}
}

//Truncate image with mask image
//mask image is divided by color c ( default to BLACK )
//image will be painted at (x,y)
//(vx,vy) is the offset of destination point from the start point
//(sx,sy) and (ex,ey) is the start and end point of the image
//bk determines using image color (true) or using mask image color (false) to paint
//if cp is passed, then judge whether cp is in drawing point
EPBOOL EP_TruncImage(EPTYPE x, EPTYPE y, EIMAGE image, EIMAGE image_bk, ECOLOR c, EPBOOL bk, const PEPoint cp, int imgtype, EFTYPE rotation, EPTYPE vx, EPTYPE vy, EPTYPE sx, EPTYPE sy, EPTYPE ex, EPTYPE ey)
{
	if (image == NULL)
		return false;
	if (image_bk == NULL)
	{
		putimage(x, y, image);
		return false;
	}
	setcolor(WHITE);

	DWORD* dst = EP_GetImageBuffer();
	DWORD* src = EP_GetImageBuffer(image);
	DWORD* src_bk = EP_GetImageBuffer(image_bk);
	if (dst == NULL || src == NULL || src_bk == NULL)
		return false;
	int width = getwidth();
	int height = getheight();
	int pxl_idx = 0;
	int src_idx = 0;
	int dst_idx = 0;
	int img_w = getwidth(image_bk);
	int img_h = getheight(image_bk);
	int dst_w = getwidth(image);
	int dst_h = getheight(image);

	//Return value
	EPBOOL isin = false;

	int mvy = 0, mvx = 0;

	//Points
	EPoint start(x + vx + sx + (ex - sx) / 2, y + vy + sy + (ey - sy) / 2), dstp;

	for (mvy = sy; mvy<img_h && (ey>0 ? mvy<ey : true); mvy++)
	{
		//                 ||not across a line
		for (mvx = sx; x + mvx + vx<width&&mvx<img_w && (ex>0 ? mvx<ex : true); mvx++)
		{
			//||not across a line
			if (x + mvx + vx<0)
				continue;

			//Rotation	
			dstp.Set(x + mvx + vx, y + mvy + vy);
			if (rotation>EP_ZERO)
			{
				dstp.Rotate(start, rotation);
			}
			if (dstp.y>height || dstp.y>width)
				continue;
			pxl_idx = (dstp.y)*width + dstp.x;

			if (pxl_idx<0 || pxl_idx >= width*height)
				break;
			src_idx = mvy*img_w + mvx;
			dst_idx = (mvy + vy)*dst_w + mvx + vx;
			if (dst_idx<0 || dst_idx >= dst_w*dst_h)
				continue;
			if (src_bk[src_idx] == c)
			{
				//Image types
				if (imgtype&&src[src_idx] == BLACK)
					continue;
				//	putpixel(x+mvx+vx,y+mvy+vy,bk?c:src[dst_idx]);
				dst[pxl_idx] = bk ? c : src[dst_idx];
				//judge cp
				if (cp&&!isin)
				{
					if (cp->x == dstp.x&&cp->y == dstp.y)
					{
						isin = true;
					}
				}
			}
		}
	}
	return isin;
}

void EP_SetPixel(EPTYPE x,EPTYPE y,ECOLOR c)
{
	putpixel(x,y,c);
}

ECOLOR EP_GetPixel(EPTYPE x,EPTYPE y,EIMAGE image)
{
	if (image==NULL)
		return getpixel(x,y);
	else
		return getpixel(x,y,image);
}

void EP_CopyImage(EIMAGE& image,EIMAGE source)
{
	if (image)
		delimage(image);
	image=newimage();
	PIMAGE backproc=NULL;
	int width,height;
	//Resize image to width*height
	PIMAGE save=gettarget();
	//get image size
	settarget(image);	
	width=getwidth();
	height=getheight();
	backproc=newimage(width,height);
	settarget(save);
	putimage(backproc,0,0,width,height,source,0,0,width,height);
	getimage(image,backproc,0,0,width,height);
	delimage(backproc);
}

//get image buffer
DWORD* EP_GetImageBuffer(EIMAGE image)
{
	DWORD *ret=NULL;
	if (image!=NULL)
	{
		PIMAGE save=gettarget();
		settarget(image);
		ret=(DWORD *)getbuffer(gettarget());
		settarget(save);
	}
	else
	{
		ret=(DWORD *)getbuffer(gettarget());
	}
	return ret;
}

void EP_PutImage(EPTYPE x,EPTYPE y,EIMAGE image)
{
	putimage(x,y,image);
}

EIMAGE EP_NewImage()
{
	return newimage();
}

void EP_DeleteImage(EIMAGE& image)
{
	delimage(image);
	image=NULL;
}

int EP_GetImageWidth(EIMAGE image)
{
	return getwidth(image);
}

int EP_GetImageHeight(EIMAGE image)
{
	return getheight(image);
}


int EP_GetTextWidth(EPCHAR& c)
{
	return textwidth(c);
}

void EP_Text(EPTYPE x,EPTYPE y,EPCHAR * c)
{
	outtextxy(x,y,c);
}

//end of file
