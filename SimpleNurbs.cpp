
#include "SimpleNurbs.h"

#pragma warning (disable : 4275)

#include <nurbs.h>
#include <nurbsS.h>
#include <barray2d.h>
#include <color.h>

#include <vector>

using std::vector;

//la class Vector_Point2Dd n'est pas dans le namespace PLib
//using PLib::Vector_Point2Dd;
//using PLib::Cp;
using PLib::Point3Dd;
using PLib::HPoint3Dd;
using PLib::NurbsCurved;
using PLib::NurbsSurfaced;


// COURBES

struct SimpleNurbs
{
public:
//lire la trajectoire/courbure dans le fichier de sol specifie
//ou creer la trajectoire a partir des points "Trj"
	SimpleNurbs(Point2DNurbs* CtrlPts, int NbCtrlPts);
	
    Point2DNurbs GetPoint(double parametre);
	double GetCourbure(double parametre);
    void Evalue(double parametre, Point2DNurbs& PosIdeale, Point2DNurbs& DirectionIdeale, double& Courbure);
private:
	Vector_HPoint3Dd PtControl;
	NurbsCurved MaCourbe;
};

SIMPLENURBS_API SimpleNurbs* NewSimpleNurbs(Point2DNurbs* CtrlPts, int NbCtrlPts)
{
	return new SimpleNurbs(CtrlPts, NbCtrlPts);
}

SIMPLENURBS_API Point2DNurbs GetPoint(SimpleNurbs* Curve, double parametre)
{
	return Curve->GetPoint(parametre);
}

SIMPLENURBS_API double GetCourbure(SimpleNurbs* Curve, double parametre)
{
	return Curve->GetCourbure(parametre);
}

SIMPLENURBS_API void Evalue(SimpleNurbs* Curve, double parametre, Point2DNurbs& PosIdeale, Point2DNurbs& DirectionIdeale, double& Courbure)
{
	Curve->Evalue(parametre, PosIdeale, DirectionIdeale, Courbure);
}

SIMPLENURBS_API void DeleteSimpleNurbs(SimpleNurbs* Curve)
{
	delete Curve;
}

//Creation de la trajectoire
//Il faut deux points par troncon (ligne droite ou arc de cercle)
//Chaque arc fait au max 60° ...On est obligé de le discretiser si il fait plus
//	->voir la doc ref:
//BUG 4 points minimum pour la Nurbs ou 3?
//BUG: referencer la doc ou j'ai trouvé la methode!
SimpleNurbs::SimpleNurbs(Point2DNurbs* CtrlPts, int NbCtrlPts)
{
	//il faut au minimum 4 points pour que la nurbs ne foire pas
	if (NbCtrlPts<4)
	{
		PtControl.resize(4);
		PtControl[0] = HPoint3Dd(0,0,0,1);
		PtControl[1] = HPoint3Dd(1,0,0,1);
		PtControl[2] = HPoint3Dd(2,0,0,1);
		PtControl[3] = HPoint3Dd(4,0,0,1);
	}
	else
	{
		PtControl.resize(NbCtrlPts);
		for (int i=0 ; i<NbCtrlPts ; i++)
		{
			PtControl[i] = HPoint3Dd(CtrlPts[i].X, CtrlPts[i].Y, 0, 1);
		}
	}

	Vector_Point3Dd PCtrl(PtControl.size());
	//	Conversion du tableau de points de type Point2DNurbs en Point2Dd pour construction NURBS
	//	Point2Dd type specifique a la Librairie NURBS++
	for (int i=0 ;i < PtControl.size(); i++)
	{
		PCtrl[i].x() = PtControl[i].x();
		PCtrl[i].y() = PtControl[i].y();
		PCtrl[i].z() = PtControl[i].z();
	}
	MaCourbe.globalInterp(PCtrl,3);

	//	MaCourbe.writeVRML97("Latrj.wrl",2,18,cyanColor,30,180) ;
}

Point2DNurbs SimpleNurbs::GetPoint(double parametre)
{
	Point3Dd P = PLib::project(MaCourbe(parametre));
	Point2DNurbs Pt;
	Pt.X = P.x();
	Pt.Y = P.y();
	return Pt;
}

double SimpleNurbs::GetCourbure(double parametre)
{
	Vector_Point3Dd dw;		//Stocke les dérivées suivant x et y
	MaCourbe.deriveAt(parametre, 2, dw);	//Retourne la dérivée de 0 a 2 de la nurbs dans dw

	//////////////////////////////
	//	Calcul de la courbure	//
	//	       x'y" - y'x"		//
	//	C = -----------------	//
	//	    (x'² + y'²)^(3/2)	//
	//////////////////////////////
	double Denom = dw[1].x() * dw[1].x() + dw[1].y() * dw[1].y();
	double Numer = dw[1].x() * dw[2].y() - dw[1].y() * dw[2].x();
	return Numer / sqrt(Denom * Denom * Denom);
}

void SimpleNurbs::Evalue(double parametre, Point2DNurbs& PosIdeale, Point2DNurbs& DirectionIdeale, double& Courbure)
{
	Vector_Point3Dd dw;		//Stocke les dérivées suivant x et y
	MaCourbe.deriveAt(parametre, 2, dw);	//Retourne la dérivée de 0 a 2 de la nurbs dans dw

	//////////////////////////////
	//	Calcul de la courbure	//
	//	       x'y" - y'x"		//
	//	C = -----------------	//
	//	    (x'² + y'²)^(3/2)	//
	//////////////////////////////
	double Denom = dw[1].x() * dw[1].x() + dw[1].y() * dw[1].y();
	double Numer = dw[1].x() * dw[2].y() - dw[1].y() * dw[2].x();
	Courbure = Numer / sqrt(Denom * Denom * Denom);
		
	//////////////////////////////
	//	"Calcul" de la tangente	//
	//		T = (dy , dx)		//
	//////////////////////////////
	PosIdeale.X = dw[0].x();
	PosIdeale.Y = dw[0].y();
	DirectionIdeale.X = dw[1].x()/dw[1].norm();
	DirectionIdeale.Y = dw[1].y()/dw[1].norm();
}


// SURFACES

struct SimpleNurbsSurface
{
public:
	SimpleNurbsSurface(Point3DNurbs* CtrlPts, int NbRows, int NbCols, int orderU, int orderV);
	
    Point3DNurbs GetPoint(double u, double v);
	Point3DNurbs GetNormale(double u, double v);
    
	double FindNearestPoint(const Point3DNurbs& point,
							double& guessU, double& guessV,
							double	error = 0.001,
							double	s = 0.2,
							int  	sep = 9,
							int  	maxIter = 10,
							double	uMin = 0.0,
							double	uMax = 1.0,
							double	vMin = 0.0,
							double	vMax = 1.0);
	double FindNearestPointLoop(const Point3DNurbs& point,
							double& guessU, double& guessV,
							double	error = 0.001,
							double	s = 0.2,
							int  	sep = 9,
							int  	maxIter = 10);
private:
	NurbsSurfaced mySurface;
};

// Dll interface implementations
SIMPLENURBS_API SimpleNurbsSurface* NewSimpleNurbsSurface(Point3DNurbs* CtrlPts, int NbRows, int NbCols)
{
	return new SimpleNurbsSurface(CtrlPts, NbRows, NbCols, 2, 2);
}

// Dll interface implementations
SIMPLENURBS_API SimpleNurbsSurface* NewSimpleNurbsSurfaceOrder(Point3DNurbs* CtrlPts, int NbRows, int NbCols, int orderU, int orderV)
{
	return new SimpleNurbsSurface(CtrlPts, NbRows, NbCols, orderU, orderV);
}

SIMPLENURBS_API Point3DNurbs SurfaceGetPoint(SimpleNurbsSurface* Surface, double u, double v)
{
	return Surface->GetPoint(u, v);
}

SIMPLENURBS_API Point3DNurbs SurfaceGetNormale(SimpleNurbsSurface* Surface, double u, double v)
{
	return Surface->GetNormale(u, v);
}

SIMPLENURBS_API double SurfaceFindNearestPoint(SimpleNurbsSurface* Surface, 
												const Point3DNurbs& point,
												double& guessU, double& guessV,
												double	error,
												double	s,
												int  	sep,
												int  	maxIter,
												double	uMin,
												double	uMax,
												double	vMin,
												double	vMax)
{
	return Surface->FindNearestPoint(point, guessU, guessV, error, s, sep, maxIter, uMin, uMax, vMin, vMax);
}

SIMPLENURBS_API double SurfaceFindNearestPointLoop(SimpleNurbsSurface* Surface, 
												const Point3DNurbs& point,
												double& guessU, double& guessV,
												double	error,
												double	s,
												int  	sep,
												int  	maxIter)
{
	return Surface->FindNearestPointLoop(point, guessU, guessV, error, s, sep, maxIter);
}

SIMPLENURBS_API void SIMPLENURBS_API_CDECL DeleteSimpleNurbsSurface(SimpleNurbsSurface* Surface)
{
	delete Surface;
}

// Convertions between PLib::Point3Dd and Point3DNurbs
Point3DNurbs convert(const Point3Dd& A)
{
	Point3DNurbs tmp;
	tmp.X = A.x();
	tmp.Y = A.y();
	tmp.Z = A.z();
	return tmp;
}

Point3Dd convert(const Point3DNurbs& A)
{
	Point3Dd tmp;
	tmp.x() = A.X;
	tmp.y() = A.Y;
	tmp.z() = A.Z;
	return tmp;
}

// Class Implementation
SimpleNurbsSurface::SimpleNurbsSurface(Point3DNurbs* CtrlPts, int NbRows, int NbCols, int orderU, int orderV)
{
	Matrix_Point3Dd matrix(NbRows, NbCols);

	//[i*NbCols + j]  to encode [i][j]
	for (int i=0; i<NbRows; i++)
		for (int j=0; j<NbCols; j++)
			matrix[i][j] = convert(CtrlPts[i*NbCols+j]);

	mySurface.globalInterp(matrix, orderU, orderV);
/*	
	static int file = 0;
	std::stringstream sstr;
	sstr << "c:\\test" << file++ << ".wrl";
	mySurface.writeVRML97(sstr.str().c_str(), PLib::cyanColor, 200, 200, 0.0, 1.0, 0.0, 1.0) ;
	*/
}
	
Point3DNurbs SimpleNurbsSurface::GetPoint(double u, double v)
{
	Point3Dd tmp = mySurface.pointAt(u,v);
	return convert(tmp);
}

Point3DNurbs SimpleNurbsSurface::GetNormale(double u, double v)
{
	Point3Dd tmp = mySurface.normal(u,v);
	return convert(tmp);
}

double SimpleNurbsSurface::FindNearestPoint(const Point3DNurbs& point,
											double& guessU, double& guessV,
											double	error,
											double	s,
											int  	sep,
											int  	maxIter,
											double	uMin,
											double	uMax,
											double	vMin,
											double	vMax)
{
	return mySurface.minDist2(convert(point), guessU, guessV,error, s, sep, maxIter, uMin, uMax, vMin, vMax);
}


double SimpleNurbsSurface::FindNearestPointLoop(const Point3DNurbs& point,
											double& guessU, double& guessV,
											double	error,
											double	s,
											int  	sep,
											int  	maxIter)
{
	return mySurface.minDist2Loop(convert(point), guessU, guessV,error, s, sep, maxIter);
}

#ifdef WIN32
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif
