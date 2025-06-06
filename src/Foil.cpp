//  =================================================================
//
//  Foil.cpp
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

#include <Foil.h>
#include <Globals.h>
#include <Landau.h>
#include <Vavilov.h>
#include <Moliere.h>
#include <Constants.h>
#include <Coordinates.h>
#include <random>
#include <chrono>
#include <map>

using  Constants::PI; 
static const double pi = PI;
static const double me = Constants::ME_MEV*1.0e6;    // electron rest mass [eV]
static const double mp = Constants::MP_MEV*1.0e6;    // proton rest mass [eV]


namespace {
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd() );
  std::normal_distribution<double>       normal(0.0,1.0);  // normal distribution mean=0.0, sigma=1.0
  std::uniform_real_distribution<double> uniform(0.0,1.0);  // uniform distribution mean=0.0, sigma=1.0

  struct Material {
    std::string name; 
    double rho; //  [g/cm**3] density  
    double A;   //  [amu]     atomic mass
    int    Z;   //  Atomic no
  };

  static std::map<std::string, Material> materials { {"C", {"Carbon", 2.27, 12.0, 6}}};
} // anonymous namespace 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Compute approximate radiation length for various materials
// Ref: Y-S Tsai. Reviews of Modern Physics vol 46, no 815 1974 

double Foil::radLength(int Z, double A, double rho)
{
  // Z atomic number (no of protons)   for carbon:  6
  // A relative atomic mass in amu     for carbon:  12.011
  // 1 amu = 931.4940954(57) MeV/c**2
  // rho density in g/cm**3            for carbon: ~2.25 g/cm**3

  auto& alpha = Constants::alpha_fs;
  
  auto Lrad      = [](int Z) {return log(184.15*pow(double(Z),-1.0/3.0));};
  auto Lradprime = [](int Z) {return log(1194.0*pow(double(Z),-2.0/3.0));};
  auto f         = [alpha](int Z) { auto a = alpha*Z;
                               return (a*a)*( pow(1+a*a,-1) + 0.20206 - 0.0369* pow(a,2) + 0.0083*pow(a,4) - 0.002*pow(a,6));
                             };

  auto& r_e = Constants::r_e;  
  auto& N_A = Constants::N_avogadro;
  
  double C   = 4*alpha*(r_e*r_e)*N_A/A;
  double val = C/A*((Z*Z)*(Lrad(Z)-f(Z))+Z*Lradprime(Z));
  return 1.0/val; 
  
  //double X0 = (716.4*A)/(Z*(Z+1)*log(287.0/sqrt(double(Z)))); // in g/cm**2
  //return X0/rho;

}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Foil:: Foil()
  : Element()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Foil::Foil( char const* nm, char const* fnm)
  :  Element(nm, fnm), dE_(1.0282e3), I0_(120.0),vavilov_(), moliere_(), landau_()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Foil::Foil( Foil const& o)
  : Element(o),
  rho_(o.rho_),
  dm_(o.dm_),
  s_(o.s_),
  A_(o.A_),
  Z_(o.Z_),
  Theta0_(o.Theta0_),
  Atheta_(o.Atheta_),
  a0_(o.a0_),
  rp_(o.rp_),
  na_(o.na_),
  X0_(o.X0_),
  I0_(o.I0_),
  dE_(o.dE_),
  vavilov_(o.vavilov_),
  moliere_(o.moliere_),
  landau_(o.landau_)
{
  std::cerr << "Foil::Foil( Foil const& o)" << std::endl;
}

  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Foil& Foil::operator= (Foil const& rhs)
{

  std::cerr <<  "Foil::operator= " << std::endl;

  if (&rhs == this) return *this;
  
  static_cast<Element>(*this) = rhs;

  rho_ = rhs.rho_;
  dm_  = rhs.dm_;
  s_   = rhs.s_;
  A_   = rhs.A_;
  Z_   = rhs.Z_;
  Theta0_ = rhs.Theta0_;
  Atheta_ = rhs.Atheta_;
  a0_ = rhs.a0_;
  rp_ = rhs.rp_;
  na_ = rhs.na_;
  X0_ = rhs.X0_;

  vavilov_ = rhs.vavilov_;
  moliere_ = rhs.moliere_;
  landau_  = rhs.landau_;
  
  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Foil::~Foil()
{
  std::cerr << "Foil::~Foil()" << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Foil::Tmax(double gma) const
{
  static const double         me = 0.511006e6; // electron rest mass [eV];

  return 2*me*(gma*gma - 1.0) ;           
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Foil::initialize( double ms, double gma)
{
  // this function is meant to be called by the particle tracker  
  // FOR NOW: all parameters set to defaults appropriate for the for the booster ring at injection

  std::cerr << "Foil::initialize" << std::endl;
  
  Vavilov::Parameters vparms;
  vparms.gma    = gma;                      // 1.426;  // 400 MeV
  vparms.dE     = dE_;                      // 1.0282e3;                 // [eV] 1.0282e3 eV "expected" or average energy loss  foil density = 400e-6 g/cm**2  
  vparms.I0     = I0_;                      // 120.0;                    // ionization energy for C 
  vparms.nfft   = 8192*4;                   // number of FFT points used to sample the pdf.  

 
  //if (!vavilov_) { vavilov_ = std::make_unique<Vavilov>(vparms); }
  vavilov_ = std::make_unique<Vavilov>(vparms);
    
  Moliere::Parameters mparms;

  double bg  =  sqrt(gma*gma - 1.0);
  double p   =  bg*ms; // momentum, MeV/c
  double bta =  bg/gma;
  
  mparms.thetamin = (2*rp_*Z_)/(bta*bta*gma)*(2*pow(Z_,1.0/3.0))/a0_;  

  double Dp = (4*PI*Z_*Z_*rp_*rp_*na_)/(bg*bg*bta*bta);

  // NOTE: we assume that X0 is in cm
  // NOTE: if X0 is given in g-cm**2, replace s/X0 by dm/X0

  mparms.theta0  = Atheta_/(bta*p)*sqrt(s_/X0_)*(1.0+0.038*log(s_/(bta*bta*X0_))); // scattering angle  
  mparms.Lc      = (mparms.theta0*mparms.theta0)/(s_*Dp); // Lc: "scattering logarithm"  
  std::cout << " theta0 = " << mparms.theta0 << std::endl;
  std::cout << " Lc     = " << mparms.Lc << std::endl;
  moliere_ = std::make_shared<Moliere>(mparms);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Foil::preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const
{
   std::cout << " Foil::preTrack  Enr0 = " << Enr0 << std::endl;

   double gma =  1.+Enr0/ms;
   const_cast<Foil*>(this)->initialize(ms, gma);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  Foil::trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v ) const
{  
  //.............................................
  // ms:   particle rest mass         [ MeV/c*2]
  // Enr0: reference kinetic energy   [ MeV/c*2] 
  // v:    6D Coordinate vector
  //.............................................
  

  // technically, energy is particle dependent.
  // for this element we assume that reference energy = particle energy 

  double gma =  1.0+Enr0/ms;
  double bg  =  sqrt(gma*gma - 1.0);
  double bta =  bg/gma;  
  double p   =  bg*ms; // momentum, MeV/c

  //................................................
  // transverse small angle (multiple scattering) contribution
  //................................................

  Moliere::Fluctuation mfluct = (*moliere_)();
  
  v[0] += mfluct.x; 
  v[2] += mfluct.y; 

  v[1] +=  mfluct.xp;
  v[3] +=  mfluct.yp;
  
  //std::cout <<   "moliere mfluct.x  = " << mfluct.x  << std::endl;
  //std::cout <<   "moliere mfluct.xp = " << mfluct.xp << std::endl;
  //std::cout <<   "moliere mfluct.y  = " << mfluct.y  << std::endl;
  //std::cout <<   "moliere mfluct.yp = " << mfluct.yp << std::endl;

  //................................................
  // Vavilov energy loss(multiple scattering)  contribution
  //................................................
  
  //std::cerr <<  " dE_  = " << dE_  << std::endl; 
  //std::cerr <<  " Enr0 = " << Enr0 << std::endl; 

  double dEv  =   dE_* ((*vavilov_)());
  //std::cout <<   "vavilov dEv = " << dEv  << std::endl;

  v[5] -=   ( ((dE_ + dEv)*1.0e-6)/Enr0 ); //  avg loss + Vavilov fluctuation 

  //................................................
  // Transverse scattering correlated with longitudinal loss
  //................................................

  double tmax   = Tmax(gma);

  //std::cerr <<  " ms  = " << ms  << std::endl; 

  double dtheta  = (me/(ms*1.0e6)) * sqrt(4*dE_*(dEv/tmax))*(1.0-(dEv/tmax)); // dE_, tmax in [eV]
  double alpha   = 2*pi*uniform(gen); 
  double dxp     = dtheta*sin(alpha);
  double dyp     = dtheta*cos(alpha);

  v[1] +=  dxp;
  v[3] +=  dyp;

  //std::cerr <<  "single scattering dxp    = " <<  dxp << std::endl;
  //std::cerr <<  "single scattering dyp    = " <<  dyp << std::endl;

  return 0; // all good
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Foil::toString( char* buf) const
{ /*** FIX ME ***/}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Foil::setParameters( int np, double attributes[], ... )
{ /*** FIX ME ***/

  std::cerr <<  " thickness   = " <<  attributes[0] << std::endl;  // in mu g/cm**2
  std::cerr <<  " density     = " <<  attributes[1] << std::endl;
  std::cerr <<  " Z           = " <<  attributes[2] << std::endl;
  std::cerr <<  " dL/dx       = " <<  attributes[3] << std::endl;
  std::cerr <<  " WedgeEnd    = " <<  attributes[4] << std::endl;


  // ep->L     - thickness [cm]
  // ep->B     - density [g/cm^3]
  // ep->N     - atomic number Z
  // ep->G     - dL/dx
  // ep->T     - tilt [deg]
  // ep->ofsX  - WedgeEnd [cm]

  std::cout << " np = " << np << std::endl;
  for (int i=0; i<np; ++i ) {
    std::cout << " attributes["<<i<<"] = " << attributes[i] << std::endl;
  }
  
  std::cerr <<  " thickness   = " <<  attributes[0] << std::endl;
  std::cerr <<  " density     = " <<  attributes[1] << std::endl;
  std::cerr <<  " Z           = " <<  attributes[2] << std::endl;
  std::cerr <<  " dL/dx       = " <<  attributes[3] << std::endl;
  std::cerr <<  " WedgeEnd    = " <<  attributes[4] << std::endl;

  // Enr0   - reference kinetic energy in MeV
  // ms     - particle mass in MeV
  // v[i]; i=0 ... 5;   vector of particle coordinates (x, TetaX, y, TetaY, s, DeltaP/p)
  

  // *** hardwired for now -- assuming Carbon foil

  static double const& Mp = Constants::MP_kg*1.0e3; // proton mass in g 
  
  
  dm_  = 400e-6;   // foil thickness is g/cm**2
  s_   = dm_/rho_; // foil tkickness in cm

  Theta0_ = 1.0;              // is this correct ??? 

  Atheta_ = 13.6;             // MeV  What is this  ??? 
  a0_    =  0.529e-8;         // cm   What is this  ??? 
  na_    = rho_/(A_*Mp);      // [atom/cm**3] atomic density  
  X0_    = radLength(Z_, A_); // A_ should be expressed in amu !!FIXME!!!

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0

int OptimMainWindow::PassFoil(Element *ep, double Enr0, double ms, Coordinates& v)
{

  int i, j, k, i0;
  int ZZ    = ep->N;			// relative charge of absorber madium
  double Dz = ep->L/100;		// length of the medium [m]
  double xx = v[0]/100;		        // initial x-position [m]
  double yy = v[2]/100;		        // initial y-position [m]
  double z  = 0;			// initial z-position [m]
  double TanPhi = ep->G;		// dL/dx = tg(Phi)
  double x0     = ep->ofsX/100;		// Wedge End coordinate [m]
  double tilt   = ep->T*PI/180;		// Wedge tilt [rad]
  double phi    = atan2(v[3],v[1]);		 // initial azimuthal angle         !!
  double theta  = sqrt(v[1]*v[1]+v[3]*v[3]); // initial polar angle      !!
  double x  = xx;
  double y  = yy;
  int AA    = 35;				// Foil atomic mass
  double Dn = ep->B*1.e3;			// Foil density [kg/m^3]
  double c2 = C_CGS*C_CGS*1.e-4;                //             !!
  double Dl = 5.e-3;				// slice thickness [m]   ?? wrong
  double pM = 1.e6*ms*E_SGS/(C_CGS*C_CGS*C_DERV3)*1e-3;	// beam particle mass [kg]   !!

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
  v[3] = v[3]+ep->L;
  v[1] = 1.;  // ??
  v[4] = 1.;  // ??
  v[5] = Eloss/E0;

  return 0;
}

#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

