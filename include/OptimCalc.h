//  =================================================================
//
//  OptimCalc.h
//
//  This file is part of OptiMX, an interactive tool  
//  for beam optics design and analysis. 
//
//  Copyright (c) 2025 Fermi Forward Discovery Group, LLC.
//  This material was produced under U.S. Government contract
//  89243024CSC000002 for Fermi National Accelerator Laboratory (Fermilab),
//  which is operated by Fermi Forward Discovery Group, LLC for the
//  U.S. Department of Energy. The U.S. Government has rights to use,
//  reproduce, and distribute this software.
//
//  NEITHER THE GOVERNMENT NOR FERMI FORWARD DISCOVERY GROUP, LLC
//  MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
//  LIABILITY FOR THE USE OF THIS SOFTWARE.
//
//  If software is modified to produce derivative works, such modified
//  software should be clearly marked, so as not to confuse it with the
//  version available from Fermilab.
//
//  Additionally, this program is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version. Accordingly,
//  this program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  See the GNU General Public License for more details.
//
//  https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
//  https://www.gnu.org/licenses/gpl-3.0.html
//
//  =================================================================
//

#ifndef OPTIMCALC_H
#define OPTIMCALC_H

#include <Structs.h>
#include <RMatrixFwd.h>
#include <algorithm>
#include <functional>


struct Coordinates;
class  Bunch;
struct Element;

struct d2matr {double e11, e12, e21, e22;};

extern const char* intgrerr[]; 

class Twiss;
class Twiss4D;

//double el_to_matr(double *Enr, double ms, double *tetaY,
//		  Element* el, RMatrix* mi, double dalfa, int st);

//void preTrack(Element *ep, double Enr0,  double tetaY,
//	      int n_elem, TrackParam* prm, RMatrix* m1);

//int trackOnce(Element *ep, double Enr0, int n_elem, int n_turn,
//	      TrackParam* prm, RMatrix* m1, Coordinates *v, LossStruct *loss);

// General subroutines for calculations of the latice parameters ---------------

extern double arch(double x);

extern double tiltb(double alfa, double fi, double& tetaY, RMatrix* tm);

extern double tiltbnew(double alfa, double fi, double& tetaY, RMatrix* tm); // matrix based 

extern void d2mul(d2matr* r, d2matr* m2, d2matr* m1);

extern int find_tunes(RMatrix& tm, double L, Twiss& v, double*alfa);

extern void DeltOrb(double hr, double ms, Element const& el,
			double& x, double& y, double& z, double& tetax, double& tetay);
//extern bool filterName(char* name, char* filter, bool MatchCase);

extern void spline(double const x[], double const y[], int n, double y2[]);

extern int splint(double const xa[], double const ya[], double const y2a[], int n, double x, double *y);

extern double tnorm(double x);

extern int dsplint(double const xa[], double const ya[], double const y2a[], int n, double x, double *y);

extern void getTwiss4DFromEigenVal(std::complex<double> const ev[][4], Twiss4D& v, char* err);

extern void getDisp4D(RMatrix& tm, Twiss4D& v, char* err);

// General subroutines for space charge calculations ---------------------------


extern double tiltsp(double alfa, double fi, double rotat, double *tetaY, RMatrix* tm);

extern void GetSolenoidMatrix(double kb, std::complex<double> Kx, std::complex<double> Ky, double len, RMatrix* mt);

extern void GetBeamFocusing(double current, double bt, BeamSize const& bs, double& gx, double& gy, double& gs);

extern void get2DMatrix(double hr, double g, double len, double& M11, double& M12, double& M21, double& M22);

extern void get2DMatrix(double hr, double g, double len, RMatrix& mi, SubSpace n);

extern void GetNormalizedGradients(double gx, double gy, double gs, double& Gx, double& Gy, double& rotat_deg);
extern void GetNormalizedGradientsNew(double gx, double gy, double gs, double& Gx, double& Gy, double& rotat_rad);


template <typename T>
std::vector<T> operator+(std::vector<T> const& a, std::vector<T> const& b)
{
  //assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(), 
                   std::back_inserter(result), std::plus<T>());
    return result;
}

template <typename T>
std::vector<T> operator-(std::vector<T> const& a, std::vector<T> const& b)
{
  //assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(), 
                   std::back_inserter(result), std::minus<T>());
    return result;
}

// splines and integration (used by Cavity )



#endif // OPTIMCALC_H
