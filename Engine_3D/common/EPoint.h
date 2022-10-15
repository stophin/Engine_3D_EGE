//EPoint.h
//Point definitions
//
//Author: Stophin
//2014.01.08
//Ver: 0.01
#ifndef _EPOINT_H_
#define _EPOINT_H_

#include <math.h>


#define MINGW_COMPLIER

#ifdef MINGW_COMPLIER
#define EP_ABS(x) ((x)>=0?(x):(-x))
#else
#define EP_ABS(x) abs(x)
#endif

#define EP_MAX			9999999

#define EP_PI			3.141592654
#define EP_PI_DEGREE	180

//Degree and Radian transformation
#define DEGREE2RADIAN(x) (EFTYPE)(x*EP_PI/EP_PI_DEGREE)
#define RADIAN2DEGREE(x) (EFTYPE)(x*EP_PI_DEGREE/EP_PI)

//Macros for delay using, x must be >=0 and not lager than 0xFF
#define SET_INT16_MAX(x) (((x+1)<<16)+(x+1))
#define GET_INT16_MAX(x) ((x>>16)-1)
#define GET_INT16_NOW(x) (x&0xFF)
#define GET_INT16_MIN(x) ((GET_INT16_NOW(x))==0)
#define RST_INT16_MAX(x) SET_INT16_MAX(GET_INT16_MAX(x))

typedef float	EPTYPE;
typedef float	EFTYPE;
typedef float	EETYPE;
typedef double	EMTYPE;
typedef void	EPVOID;
typedef bool	EBTYPE;

#define USING_SIMD_INTRINSIC

#ifdef USING_SIMD_INTRINSIC
#include <xmmintrin.h>
#include <immintrin.h>
extern __declspec(align(16)) __m256i gatherA12;
extern __declspec(align(16)) __m256i gatherA34;

extern __declspec(align(16)) __m256i gatherB11;
extern __declspec(align(16)) __m256i gatherB22;
extern __declspec(align(16)) __m256i gatherB33;
extern __declspec(align(16)) __m256i gatherB44;

extern __declspec(align(16)) __m256i gatherAA12;
extern __declspec(align(16)) __m256i gatherBB11;
extern __declspec(align(16)) __m256i gatherBB22;
#endif

#define EP_ZERO			1e-6

#define EP_LTZERO(x)	(x < -EP_ZERO)
#define EP_GTZERO(x)	(x > EP_ZERO)
#define EP_ISZERO(x)	(x > -EP_ZERO && x < EP_ZERO)
#define EP_ISZERO_INT(x)	(0 == x)
#define EP_RANGE_INT(x, r) (x > -r && x < r)
#define EP_LEZERO(x)	(EP_LTZERO(x) || EP_ISZERO(x))
#define EP_GEZERO(x)	(EP_GTZERO(x) || EP_ISZERO(x))
#define EP_NTZERO(x)	(!EP_ISZERO(x))

//When judging IsIn
//use these to determinate the type of argument
typedef enum E_RectType
{
	E_RectLtRb,
	E_RectLbRt,
	E_RectLtGm,
	E_RectLbGm,
	E_RectPtRd
}E_RectType;

typedef class EPoint
{
public:
	EPoint();
	EPoint(EPTYPE x,EPTYPE y);
	EPoint(const EPoint& p);
	~EPoint();

	const EPoint operator- (const EPoint& pt) const;
	const EPoint operator+ (const EPoint& pt) const;
	const EPoint operator* (EPTYPE pa) const;
	EPTYPE operator* (const EPoint& pt) const;
	EPoint& operator= (const EPoint& pt);
	EPoint& operator+= (const EPoint& pt);
	EPoint& operator-= (const EPoint& pt);
	EPoint& operator*= (EPTYPE pa);
	EPoint& operator/= (EPTYPE pa);
	EPoint& operator/= (const EPoint& pt);
	EPoint operator/ (EPTYPE pa);
	bool operator ==(const EPoint& pt) const;

	void Rotate(const EPoint& o,EFTYPE angle);
	void Rotate(EFTYPE angle);

	void Set(EPTYPE x,EPTYPE y);
	void Set(const EPoint& p);
	void Normalize(EPTYPE resize=1);
	
	bool IsIn(const EPoint& lp,const EPoint& rbgm,E_RectType mode=E_RectLtRb) const;
	bool RectIsIntersect(const EPoint& gm,const EPoint& lp,const EPoint& rbgm,E_RectType mode=E_RectLtRb) const;
	bool RectIsIn(const EPoint& gm,const EPoint& lp,const EPoint& rbgm,E_RectType mode=E_RectLtRb) const;
	void GeometryCut(const EPoint& geom,const EPoint& objgeom);
	void GeometryLimit(const EPoint& lp,const EPoint& rbgm,const EPoint& objgm,E_RectType mode=E_RectLtRb);


	static bool RectIsIntersect(EFTYPE xs, EFTYPE ys, EFTYPE xe, EFTYPE ye, EFTYPE oxs, EFTYPE oys, EFTYPE oxe, EFTYPE oye);
	
	EFTYPE GetAngle(const EPoint& p) const;
	
	EPTYPE x;
	EPTYPE y;


	int InTriangle(const EPoint& a, const EPoint& b, const EPoint& c) {
		EPoint p(*this);

		float signOfTrig = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
		float signOfAB = (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x);
		float signOfCA = (a.x - c.x)*(p.y - c.y) - (a.y - c.y)*(p.x - c.x);
		float signOfBC = (c.x - b.x)*(p.y - c.y) - (c.y - b.y)*(p.x - c.x);

		int d1 = (signOfAB * signOfTrig > 0);
		int d2 = (signOfCA * signOfTrig > 0);
		int d3 = (signOfBC * signOfTrig > 0);

		return d1 && d2 && d3;
	}
}EPoint,* PEPoint;

EETYPE Q_rsqrt(EETYPE number);

#endif //end of _EPOINT_H_
//end of file
