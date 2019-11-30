//EPoint.cpp
//Point functions
//


#include "Epoint.h"

EPoint::EPoint():
x(0),
y(0)
{
}

EPoint::EPoint(EPTYPE x,EPTYPE y):
x(x),
y(y)
{
}

EPoint::EPoint(const EPoint& p):
x(p.x),
y(p.y)
{
}

EPoint::~EPoint()
{
}

const EPoint EPoint::operator- (const EPoint& pt) const
{
	return EPoint(x-pt.x,y-pt.y);
}

const EPoint EPoint::operator+ (const EPoint& pt) const
{
	return EPoint(x+pt.x,y+pt.y);
}

const EPoint EPoint::operator* (EPTYPE pa) const
{
	return EPoint(x*pa,y*pa);
}

EPTYPE EPoint::operator* (const EPoint& pt) const
{
	return (x*pt.x+y*pt.y);
}

EPoint& EPoint::operator= (const EPoint& pt)
{
	x=pt.x;
	y=pt.y;
	return *this;
}

EPoint& EPoint::operator+= (const EPoint& pt)
{
	x+=pt.x;
	y+=pt.y;
	return *this;
}

EPoint& EPoint::operator-= (const EPoint& pt)
{
	x-=pt.x;
	y-=pt.y;
	return *this;
}

EPoint& EPoint::operator*= (EPTYPE pa)
{
	x*=pa;
	y*=pa;
	return *this;
}


EPoint& EPoint::operator/= (EPTYPE pa)
{
	if (pa==0)
		pa=1;
	x/=pa;
	y/=pa;
	return *this;
}

EPoint& EPoint::operator/= (const EPoint& pt)
{
	if (pt.x!=0)
		x/=pt.x;
	if (pt.y!=0)
		y/=pt.y;
	return *this;
}

EPoint EPoint::operator/ (EPTYPE pa)
{
	if (pa==0)
		pa=1;
	return EPoint(x/=pa,y/=pa);
}

bool EPoint::operator ==(const EPoint& pt) const
{
	return (x==pt.x)&&(y==pt.y);
}

void EPoint::Set(EPTYPE x,EPTYPE y)
{
	this->x=x;
	this->y=y;
}

void EPoint::Set(const EPoint& p)
{
	this->x=p.x;
	this->y=p.y;
}

void EPoint::Normalize(EPTYPE resize)
{
	//EFTYPE n=(EFTYPE)sqrt(EFTYPE(x*x+y*y));
	//x=(EPTYPE)((EFTYPE)(x*resize)/n);
	//y=(EPTYPE)((EFTYPE)(y*resize)/n);
	EFTYPE n=(EFTYPE)Q_rsqrt(EFTYPE(x*x+y*y));
	x=(EPTYPE)((EFTYPE)(x*resize)*n);
	y=(EPTYPE)((EFTYPE)(y*resize)*n);
}

bool EPoint::IsIn(const EPoint& lp,const EPoint& rbgm,E_RectType mode) const
{
	switch(mode)
	{
	case E_RectLtGm:
		{
			return x>=lp.x&&y>=lp.y&&x<=lp.x+rbgm.x&&y<=lp.y+rbgm.y;
			break;
		}
	case E_RectPtRd:	//Judge if a point is in an ellipse
		{
			if (rbgm.x==0||rbgm.y==0)
				return false;
			EPoint vp(x-lp.x,y-lp.y);
			return (vp.x*vp.x)/(rbgm.x*rbgm.x)+(vp.y*vp.y)/(rbgm.y*rbgm.y)<=1;
		}
		break;
	default:
		return false;
	}
}

bool EPoint::RectIsIntersect(const EPoint& gm,const EPoint& lp,const EPoint& rbgm,E_RectType mode) const
{
	EPoint c1p,c2p;
	switch(mode)
	{
	case E_RectLtGm:
		c1p.Set(x+gm.x/2,y+gm.y/2);
		c2p.Set(lp.x+rbgm.x/2,lp.y+rbgm.y/2);
		break;
	default:
		return false;
	}
	return ((EP_ABS(c1p.x-c2p.x)<=(gm.x/2+rbgm.x/2))&&(EP_ABS(c1p.y-c2p.y)<=(rbgm.y/2+gm.y/2)));
}

bool EPoint::RectIsIntersect(EFTYPE xs, EFTYPE ys, EFTYPE xe, EFTYPE ye, EFTYPE oxs, EFTYPE oys, EFTYPE oxe, EFTYPE oye) {
	EFTYPE gmx = xe - xs, gmy = ye - ys;
	EFTYPE ogmx = oxe - oxs, ogmy = oye - oys;
	EFTYPE cpx = xs + gmx / 2, cpy = ys + gmy / 2;
	EFTYPE ocpx = oxs + ogmx / 2, ocpy = oys + ogmy / 2;
	return ((EP_ABS(cpx - ocpx) <= (gmx / 2 + ogmx / 2)) && (EP_ABS(cpy - ocpy) <= (ogmy / 2 + gmy / 2)));
}

bool EPoint::RectIsIn(const EPoint& gm,const EPoint& lp,const EPoint& rbgm,E_RectType mode) const
{
	EPoint c1p,c2p;
	switch(mode)
	{
	case E_RectLtGm:
		return	(x>lp.x&&x<lp.x+rbgm.x)&&(y>lp.y&&y<lp.y+rbgm.y)&&\
			(x+gm.x>lp.x&&x+gm.x<lp.x+rbgm.x)&&(y+gm.y>lp.y&&y+gm.y<lp.y+rbgm.y);
		break;
	default:
		return false;
	}
}

void EPoint::GeometryCut(const EPoint& geom,const EPoint& objgeom)
{
	if (x>geom.x-objgeom.x)
		x=geom.x-objgeom.x;
	if (y<objgeom.y)
		y=objgeom.y;
	if (x<0)
		x=0;
	if (y>geom.y)
		y=geom.y;
}

void EPoint::GeometryLimit(const EPoint& lp,const EPoint& rbgm,const EPoint& objgm,E_RectType mode)
{
	switch(mode)
	{
	case E_RectLtGm:
		if (x>lp.x+rbgm.x-objgm.x) x=lp.x+rbgm.x-objgm.x;
		else if (x<lp.x) x=lp.x;
		if (y>lp.y+rbgm.y-objgm.y) y=lp.y+rbgm.y-objgm.y;
		else if (y<lp.y) y=lp.y;
		break;
	default:
		break;
	}
}

EFTYPE EPoint::GetAngle(const EPoint& p) const
{
	EPTYPE dx=p.x-x;
	EPTYPE dy=p.y-y;

	if (dx==0||dy==0)
	{
		if (dx==0)
		{
			if (dy==0)
				return 0;
			else if (dy>0)
				return DEGREE2RADIAN(90);
			else
				return DEGREE2RADIAN(270);
		}
		if (dy==0)
		{
			if (dx==0)
				return 0;
			else if (dx>0)
				return 0;
			else if (dx<0)
				return DEGREE2RADIAN(180);
		}
	}
	EFTYPE arcang=atan((EFTYPE)dy/(EFTYPE)dx);
	if (dx>0)
	{
		if (dy>0)
			return arcang;
		else
			return 2*EP_PI+arcang;
	}
	else
	{
		return EP_PI+arcang;
	}
}

void EPoint::Rotate(const EPoint& o,EFTYPE angle)
{
	*this-=o;
	Rotate(angle);
	*this+=o;
}

void EPoint::Rotate(EFTYPE angle)
{
	EPTYPE tx,ty;
	tx=x;
	ty=y;

	x=EPTYPE(tx*cos(angle)-ty*sin(angle));
	y=EPTYPE(ty*cos(angle)+tx*sin(angle));
}

EETYPE Q_rsqrt(EETYPE number)
{
	//return sqrt(number);
	long i;
	EETYPE x2, y;
	const EETYPE threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;              // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);               // what the fuck? 
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

//end of file
