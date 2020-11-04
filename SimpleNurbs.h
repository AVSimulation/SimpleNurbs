
#ifndef _SIMPLE_NURBS_H_
#define _SIMPLE_NURBS_H_

extern "C" {

#ifdef WIN32
#ifdef SIMPLENURBS_EXPORTS
#define SIMPLENURBS_API __declspec(dllexport)
#else
#define SIMPLENURBS_API __declspec(dllimport)
#endif
#define SIMPLENURBS_API_CDECL __cdecl
#else
#define SIMPLENURBS_API
#define SIMPLENURBS_API_CDECL
#endif

struct Point2DNurbs
{
	double X;
	double Y;
};

struct Point3DNurbs
{
	double X;
	double Y;	
	double Z;
};

// Nurbs 2D Curve
struct SimpleNurbs;

SIMPLENURBS_API SimpleNurbs* SIMPLENURBS_API_CDECL NewSimpleNurbs(Point2DNurbs* CtrlPts, int NbCtrlPts);
SIMPLENURBS_API Point2DNurbs SIMPLENURBS_API_CDECL GetPoint(SimpleNurbs* Curve, double parametre);
SIMPLENURBS_API double SIMPLENURBS_API_CDECL GetCourbure(SimpleNurbs* Curve, double parametre);
SIMPLENURBS_API void SIMPLENURBS_API_CDECL Evalue(SimpleNurbs* Curve, double parametre, Point2DNurbs& PosIdeale, Point2DNurbs& DirectionIdeale, double& Courbure);
SIMPLENURBS_API void SIMPLENURBS_API_CDECL DeleteSimpleNurbs(SimpleNurbs* Curve);


// Nurbs Surface
struct SimpleNurbsSurface;

SIMPLENURBS_API SimpleNurbsSurface* SIMPLENURBS_API_CDECL NewSimpleNurbsSurface(Point3DNurbs* CtrlPts, int NbRows, int NbCols); // encoded in one dimension : [i * NbCols + j]  to encode [i][j]
SIMPLENURBS_API SimpleNurbsSurface* SIMPLENURBS_API_CDECL NewSimpleNurbsSurfaceOrder(Point3DNurbs* CtrlPts, int NbRows, int NbCols, int orderU, int orderV); // encoded in one dimension : [i * NbCols + j]  to encode [i][j]
SIMPLENURBS_API Point3DNurbs SIMPLENURBS_API_CDECL SurfaceGetPoint(SimpleNurbsSurface* Surface, double u, double v);
SIMPLENURBS_API Point3DNurbs SIMPLENURBS_API_CDECL SurfaceGetNormale(SimpleNurbsSurface* Surface, double u, double v);
SIMPLENURBS_API double SIMPLENURBS_API_CDECL SurfaceFindNearestPoint(SimpleNurbsSurface* Surface, 
												const Point3DNurbs& point,
												double& guessU, double& guessV,
												double	error = 0.001,
												double	s = 0.2,
												int  	sep = 9,
												int  	maxIter = 10,
												double	uMin = 0.0,
												double	uMax = 1.0,
												double	vMin = 0.0,
												double	vMax = 1.0); 	
SIMPLENURBS_API double SIMPLENURBS_API_CDECL SurfaceFindNearestPointLoop(SimpleNurbsSurface* Surface, 
												const Point3DNurbs& point,
												double& guessU, double& guessV,
												double	error = 0.001,
												double	s = 0.2,
												int  	sep = 9,
												int  	maxIter = 10);
SIMPLENURBS_API void SIMPLENURBS_API_CDECL DeleteSimpleNurbsSurface(SimpleNurbsSurface* Surface);

}

#endif //_SIMPLE_NURBS_H_
