
#include <iostream>

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#define LIBHANDLE HMODULE
#define LOADLIB   LoadLibrary
#define GETPROC   GetProcAddress
#define FREELIB   FreeLibrary
#else
#include <dlfcn.h>
#define LIBHANDLE void*
#define LOADLIB(LIB)   dlopen(LIB, RTLD_LAZY)
#define GETPROC   dlsym
#define FREELIB   dlclose
#endif

using namespace std;

LIBHANDLE hModule = 0;


extern "C" {

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

typedef SimpleNurbs* ( *NewSimpleNurbsfn)(Point2DNurbs* CtrlPts, int NbCtrlPts);
typedef Point2DNurbs ( *GetPointfn)(SimpleNurbs* Curve, double parametre);
typedef double ( *GetCourburefn)(SimpleNurbs* Curve, double parametre);
typedef void ( *Evaluefn)(SimpleNurbs* Curve, double parametre, Point2DNurbs& PosIdeale, Point2DNurbs& DirectionIdeale, double& Courbure);
typedef void ( *DeleteSimpleNurbsfn)(SimpleNurbs* Curve);

// Nurbs Surface
struct SimpleNurbsSurface;

typedef SimpleNurbsSurface* ( *NewSimpleNurbsSurfacefn)(Point3DNurbs* CtrlPts, int NbRows, int NbCols); // encoded in one dimension : [i * NbRows + j]  to encode [i][j], default orders are 2 and 2
typedef SimpleNurbsSurface* ( *NewSimpleNurbsSurfaceOrderfn)(Point3DNurbs* CtrlPts, int NbRows, int NbCols, int orderU, int orderV); // encoded in one dimension : [i * NbRows + j]  to encode [i][j]
typedef Point3DNurbs ( *SurfaceGetPointfn)(SimpleNurbsSurface* Surface, double u, double v);
typedef Point3DNurbs ( *SurfaceGetNormalefn)(SimpleNurbsSurface* Surface, double u, double v);
typedef double ( *SurfaceFindNearestPointfn)(SimpleNurbsSurface* Surface, 
												const Point3DNurbs& point,
												double& guessU, double& guessV,
												double	error, double	s,
												int  	sep, int  	maxIter,
												double	uMin, double uMax,
												double	vMin, double vMax); 	
typedef double ( *SurfaceFindNearestPointLoopfn)(SimpleNurbsSurface* Surface, 
												const Point3DNurbs& point,
												double& guessU, double& guessV,
												double	error, double	s,
												int  	sep, int  	maxIter); 	
typedef void ( *DeleteSimpleNurbsSurfacefn)(SimpleNurbsSurface* Surface);
}


NewSimpleNurbsfn NewSimpleNurbs = 0;
GetPointfn GetPoint = 0;
GetCourburefn GetCourbure = 0;
Evaluefn Evalue = 0;
DeleteSimpleNurbsfn DeleteSimpleNurbs = 0;

NewSimpleNurbsSurfacefn NewSimpleNurbsSurface = 0;
NewSimpleNurbsSurfaceOrderfn NewSimpleNurbsSurfaceOrder = 0;
SurfaceGetPointfn SurfaceGetPoint = 0;
SurfaceGetNormalefn SurfaceGetNormale = 0;
SurfaceFindNearestPointfn SurfaceFindNearestPoint = 0;
SurfaceFindNearestPointLoopfn SurfaceFindNearestPointLoop = 0;
DeleteSimpleNurbsSurfacefn DeleteSimpleNurbsSurface = 0;

void UnloadSimpleNurbsDll()
{
	NewSimpleNurbs = 0;
	GetPoint = 0;
	GetCourbure = 0;
	Evalue = 0;
	DeleteSimpleNurbs = 0;
	
	NewSimpleNurbsSurface = 0;
	NewSimpleNurbsSurfaceOrder = 0;
	SurfaceGetPoint = 0;
	SurfaceGetNormale = 0;
	SurfaceFindNearestPoint = 0;
	SurfaceFindNearestPointLoop = 0;
	DeleteSimpleNurbsSurface = 0;

	if (hModule!=0)
	{
        FREELIB(hModule);
		hModule = 0;
	}
}

bool LoadSimpleNurbsDll()
{
	if (hModule!=0)
		return true;

#ifdef WIN32
#ifndef _DEBUG
	TCHAR* DllName = _T("SimpleNurbs.dll");
#else
	TCHAR* DllName = _T("SimpleNurbsD.dll");
#endif
#else
    const char* DllName = "libSimpleNurbs.so";
#endif

    hModule = LOADLIB(DllName);
	if (hModule==0)
	{
        fprintf(stderr, "%s\n", dlerror());
		cout << "Failed to load \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

	// Nurbs Curve
    NewSimpleNurbs = (NewSimpleNurbsfn)GETPROC(hModule, "NewSimpleNurbs");
	if (NewSimpleNurbs==0)
	{
		cout << "Failed to find function\"NewSimpleNurbs\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    GetPoint = (GetPointfn)GETPROC(hModule, "GetPoint");
	if (GetPoint==0)
	{
		cout << "Failed to find function\"GetPoint\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    GetCourbure = (GetCourburefn)GETPROC(hModule, "GetCourbure");
	if (GetCourbure==0)
	{
		cout << "Failed to find function\"GetCourbure\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    Evalue = (Evaluefn)GETPROC(hModule, "Evalue");
	if (Evalue==0)
	{
		cout << "Failed to find function\"Evalue\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    DeleteSimpleNurbs = (DeleteSimpleNurbsfn)GETPROC(hModule, "DeleteSimpleNurbs");
	if (DeleteSimpleNurbs==0)
	{
		cout << "Failed to find function\"DeleteSimpleNurbs\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

	// Nurbs Surface
    NewSimpleNurbsSurface = (NewSimpleNurbsSurfacefn)GETPROC(hModule, "NewSimpleNurbsSurface");
	if (NewSimpleNurbsSurface==0)
	{
		cout << "Failed to find function\"NewSimpleNurbsSurface\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    NewSimpleNurbsSurfaceOrder = (NewSimpleNurbsSurfaceOrderfn)GETPROC(hModule, "NewSimpleNurbsSurfaceOrder");
	if (NewSimpleNurbsSurfaceOrder==0)
	{
		cout << "Failed to find function\"NewSimpleNurbsSurfaceOrder\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    SurfaceGetPoint = (SurfaceGetPointfn)GETPROC(hModule, "SurfaceGetPoint");
	if (SurfaceGetPoint==0)
	{
		cout << "Failed to find function\"SurfaceGetPoint\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    SurfaceGetNormale = (SurfaceGetNormalefn)GETPROC(hModule, "SurfaceGetNormale");
	if (SurfaceGetNormale==0)
	{
		cout << "Failed to find function\"SurfaceGetNormale\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    SurfaceFindNearestPoint = (SurfaceFindNearestPointfn)GETPROC(hModule, "SurfaceFindNearestPoint");
	if (SurfaceFindNearestPoint==0)
	{
		cout << "Failed to find function\"SurfaceFindNearestPoint\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    SurfaceFindNearestPointLoop = (SurfaceFindNearestPointLoopfn)GETPROC(hModule, "SurfaceFindNearestPointLoop");
	if (SurfaceFindNearestPointLoop==0)
	{
		cout << "Failed to find function\"SurfaceFindNearestPointLoop\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}

    DeleteSimpleNurbsSurface = (DeleteSimpleNurbsSurfacefn)GETPROC(hModule, "DeleteSimpleNurbsSurface");
	if (DeleteSimpleNurbsSurface==0)
	{
		cout << "Failed to find function\"DeleteSimpleNurbsSurface\" in dll \"" << DllName << "\"" << endl;
		UnloadSimpleNurbsDll();
		return false;
	}	

	return true;
}


int main(int argc, char ** argv)
{
	if (LoadSimpleNurbsDll())
		std::cout << "Load OK" << std::endl;

	UnloadSimpleNurbsDll();
	std::cout << "UnLoad Done" << std::endl;

	return 0;
}
