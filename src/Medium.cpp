//  =================================================================
//
//  Medium.cpp
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

#include <Medium.h>
#include <Beamline.h>
#include <Utility.h>
#include <Globals.h>
#include <Constants.h>
#include <Coordinates.h>
#include <random>
#include <chrono>

using  Constants::PI; 
using  Constants::B_Euler;
using  Constants::C_SI;
using  Constants::C_CGS;
using  Constants::C_DERV3;
using  Constants::E_SI; // elementary charge
using  Constants::E_CGS;
using  Constants::ME_kg;

using  Utility::Landau;

static const double pi = PI;
static const double         me = 0.511006e6;    // electron rest mass [eV]
static const double         mp = 938.2720813e6; // proton rest mass [eV]


namespace {
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd() );
  std::normal_distribution<double>       normal(0.0,1.0);  // normal distribution mean=0.0, sigma=1.0
  std::uniform_real_distribution<double> uniform(0.0,1.0);  // uniform distribution mean=0.0, sigma=1.0
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Medium:: Medium()
  : Element()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Medium::Medium( char const* nm, char const* fnm)
  :  Element(nm, fnm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Medium::Medium( Medium const& o)
  : Element(o)
{}

  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Medium& Medium::operator= (Medium const& rhs)
{

  std::cerr <<  "Medium::operator= " << std::endl;

  if (&rhs == this) return *this;
  
  static_cast<Element>(*this) = rhs;

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Medium::~Medium()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* Medium::splitnew(int nslices) const  // return a sliced element as a beamline 
{
 // Medium cannot be split so we return a cloned element. 
  Beamline* bml = new Beamline();
  Medium* e      = this->clone();
  e->L_      /= 1;
  e->slices_  = 1;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  Medium::trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const
{  

  int i, j, k, i0;
  int ZZ    = N;			// relative charge of absorber madium
  double Dz = L_/100;		// length of the medium [m]
  double xx = v[0]/100;		        // initial x-position [m]
  double yy = v[2]/100;		        // initial y-position [m]
  double z  = 0;			// initial z-position [m]
  double TanPhi = G;		// dL/dx = tg(Phi)
  double x0     = ofsX_/100;		// Wedge End coordinate [m]
  double tilt   = T_*PI/180;		// Wedge tilt [rad]
  double phi    = atan2(v[3],v[1]);		 // initial azimuthal angle         !!
  double theta  = sqrt(v[1]*v[1]+v[3]*v[3]); // initial polar angle      !!
  double x  = xx;
  double y  = yy;
  int AA    = 35;				// Medium atomic mass
  double Dn = B*1.e3;			// Medium density [kg/m^3]
  double c2 = C_CGS*C_CGS*1.e-4;                //             !!
  double Dl = 5.e-3;				// slice thickness [m]   ?? wrong
  double pM = 1.e6*ms*E_CGS/(C_CGS*C_CGS*C_DERV3)*1e-3;	// beam particle mass [kg]   !!

  double ct , st;
  if (tilt != 0) {			        // pass to the turned coordinate system
    ct = cos(tilt);
    st = sin(tilt);
    x = xx*ct - yy*st;
    y = xx*st + yy*ct;                          // !!
  }

  // !!! All computations inside this procedure are performed in SI !!!
  //	pre-computations

  int	Nl       = Dz/Dl;			// number of slices
  double pi2     = 2.*PI;                                                             //!!
  double ea      = E_SI;			// absolute value of the electron charge            //!!
  double E0      = 1.e6*Enr0*ea;		// initial energy in [J]
  double pMc2    = pM*c2;			// rest energy of the particle
  double p       = sqrt(E0*E0-pMc2*pMc2)/C_SI;	// particle initial momentum              !!
  double E0p     = 1.+E0/pMc2;
  double v0      = C_SI*sqrt(1.-1./(E0p*E0p));	// initial velocity of the particle
  double beta    = v0/C_SI;                     //!!
  double bet2    = beta*beta;
  double Thetmin = pow(ZZ,1./3.)*ME_kg*C_SI/(192.*p);
  double Thetmax = 274.*ME_kg*C_SI/(pow(AA,1./3.)*p);
  double S       = 2*ZZ*E_SI*E_SI/(v0*p); S*=S;
  double Dlc     = 2.*PI*Dn*log(Thetmax/Thetmin);
  double Sigma   = S/(Thetmin*Thetmin);                                             //!!
  double xi      = exp(-6.25); 
  double a       = 0.125/log(204./pow(ZZ,1./3.));
  double pis     = sqrt(PI);                                                           //!!
  double PotIon  = ZZ*13.5*ea;			 // Ionization potential of the atom
  double CC      = 1.5-B_Euler;		         // Euler's constant
  double eps1    = (1.-bet2)*PotIon*PotIon/(2.*pM*v0*v0)+bet2- 1.+CC;
  double xi0     = x*TanPhi*pi2*Dn*ZZ/(pM*v0*v0*AA);

  i0 = 0;			// initialization for the random number generator
  double Eloss = 0;		// Energy loss
  
  for ( int j=0; j<Nl; ++j ){	// Loop over the material slices
    double st = sin(theta);
    double ct = cos(theta);
    double r = Dl/ct;
    double sp = sin(phi);
    double cp = cos(phi);
    x = x + r*st*sp;
    y = y + r*st*cp;
    z = z + Dl;
    //	Calculation for the energy loss distribution
    xi = xi0*z;
    double dd = xi*(log(xi)-eps1);
    double Fmax = 0;
    double Fi[100];
    for ( int k=0; k<100; ++k){
      double Del=0.9*E0*(1.+k/100.);
      double lambda= (Del-dd)/xi;
      Fi[k]=Landau(lambda);
      if (Fmax < Fi[k]) Fmax = Fi[k];
    }
    double psi=xi*rand()*Fmax;      // ??     randm(i0)
    int l;
    bool left = rand() < 0.5;	// choose the left or right branch of the distribution   randm(i0)??
    for (k=0;k<100;k++){
      l = k;
      if(  left && Fi[k]<psi && Fi[k+1]> psi) break;
      if( !left && Fi[k]>psi && Fi[k+1]< psi) break;
    }
    if( l == 99){
      //cout << '' Error in Energy loss calculation '' << endl;
      return(3);   //   !!
    }
    Eloss += xi*l+dd;
    //	Check, if particle passed the absorber Element 
    if(x < x0){				// particle is lower than the wedge end
      x += (Dz-z)*tan(theta);
      break;
    }
    if(z > Dz+x*TanPhi){	// particle reaches right side of medium
      x += (Dz-z)*tan(theta);
      break;
    }
    //  Calculation the random angles theta1, phi1 in collision
    double Thet2= sqrt(Dlc*S);
    double phi1 = rand()/pi2;          //   randm(i0)??
    double xx = rand();             //   randm(i0)??
    double theta1;

    if(xx < xi)
      theta1 = Thet2*sqrt(-log(xx));
    else
      theta1 = Thet2*pow(a*pis/xx,1./3.);
    //  Calculation the new angles phi, theta by adding the random angles
    double r1 = r*tan(theta1);
    double x1 = r*cos(phi1);
    double y1 = r*sin(phi1);
    double z1 = r;
    double x2 = x1*ct*sp + y1*ct*cp + z1*st*cp;
    double y2 =-x1*ct*cp + y1*ct*sp + z1*st*sp;
    double z2 =          - y1*st*sp + z1*ct;

    phi = atan2(y2,x2); 
    theta= acos(z2/sqrt(x2*x2+y2*y2+z2*z2));
  }

  if (tilt != 0) {			// back pass to the original coordinate system
    xx = x*ct + y*st;
    y = -x*st + y*ct;
    x = xx;
  }

  //  Setup the results of computations
  v[0] = x*100;
  v[2] = y*100;
  v[3] = v[3]+L_;
  v[1] = 1.;  // ??
  v[4] = 1.;  // ??
  v[5] = Eloss/E0;

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Medium::toString( char* buf) const
{ 
  sprintf(buf, "%g \t%g \t%g \t%g", B, G, S, T_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Medium::setParameters( int np, double attributes[], ... )
{ /*** FIX ME ***/

  // ep->L      - thickness [cm]
  // ep->B      - density [g/cm^3]
  // ep->N      - atomic number Z
  // ep->G      - dL/dx
  // ep->T_     - tilt [deg]
  // ep->ofsX_  - WedgeEnd [cm]

  L_    =   attributes[0];
  B     =   attributes[1];   
  N     =   attributes[2];  
  G     =   attributes[3];  
  T_    =   attributes[4]; 
  ofsX_ =   attributes[5]; 
  
  // Enr0   - reference kinetic energy in MeV
  // ms     - particle mass in MeV
  // v[i]; i=0 ... 5;   vector of particle coordinates (x, TetaX, y, TetaY, s, DeltaP/p)
  
  // ..................................................................
  // rp:  proton charge/mass ratio [cgs = statcoulomb/g or esu/g]
  // mp:  proton mass
  // mp = 1.67262192369e-27  [kg] 1.67262192369e-24 [g]
  // e  : elementary charge
  // e  = 4.8029 e-10 esu
  //    = 1.6021 e-18 C
  // 1 C = 2.9979e10 cm/s * 1.0e-1 * 1 C  =  4.8029 e-10 esu  
  //...................................................................

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Medium::setParameters( int np, std::vector<double> const& attributes, ... )
{ /*** FIX ME ***/

  // ep->L      - thickness [cm]
  // ep->B      - density [g/cm^3]
  // ep->N      - atomic number Z
  // ep->G      - dL/dx
  // ep->T_     - tilt [deg]
  // ep->ofsX_  - WedgeEnd [cm]

  L_    =   attributes[0];
  B     =   attributes[1];   
  N     =   attributes[2];  
  G     =   attributes[3];  
  T_    =   attributes[4]; 
  ofsX_ =   attributes[5]; 
  
  // Enr0   - reference kinetic energy in MeV
  // ms     - particle mass in MeV
  // v[i]; i=0 ... 5;   vector of particle coordinates (x, TetaX, y, TetaY, s, DeltaP/p)
  
  // ..................................................................
  // rp:  proton charge/mass ratio [cgs = statcoulomb/g or esu/g]
  // mp:  proton mass
  // mp = 1.67262192369e-27  [kg] 1.67262192369e-24 [g]
  // e  : elementary charge
  // e  = 4.8029 e-10 esu
  //    = 1.6021 e-18 C
  // 1 C = 2.9979e10 cm/s * 1.0e-1 * 1 C  =  4.8029 e-10 esu  
  //...................................................................

}
