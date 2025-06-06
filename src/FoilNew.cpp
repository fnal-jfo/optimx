//  =================================================================
//
//  FoilNew.cpp
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

//Material     Z    A     X0ρg/cm2  X0 [cm]  Ec [MeV]
//Hydrogen     1    1.01    63      700000    350
//Helium       2    4.00    94      530000    250
//Lithium      3    6.94    83      156       180
//Carbon       6   12.01    43       18.8      90
//Nitrogen     7   14.01    38      30500     85
//Oxygen       8   16.00    34      24000     75
//Aluminium   13   26.98    24        8.9     40
//Silicon     14   28.09    22        9.4      9
//Iron        26   55.85    13.9      1.7     20.7
//Copper      29   63.55    12.9      1.4     18.8
//Silver      47   109.9     9.3      0.89    11.9
//Tungsten    74  183.9      6.8      0.35    8.0
//Lead        82  207.2      6.4      0.46    7.4
//Air         7.3  14.4     37      30000     84
//SiO2       11.2  21.7     27        12      57
//Water       7.5  14.2     36        36      83
//Kapton                    33.0      19.4
//Concrete                  26.7      10.7
//Beryllium  4     9.01               35.3
//---------------------------------------------------
// 
// From: https://physics.nist.gov/PhysRefData/XrayMassCoef/tab1.html
//....................................................................
// NOTE: some of the quoted densities look wrong ...
//       e.g. C (diamond) has density of 3.5 g/cm**3  C(graphite) has density of 2.26 g/cm**3
//            in the table below, the density is reported to be 1.7 g/cm**3 ???
//...........................................................................................
// Z            Element         Z/A             I0 [eV] Density [g/cm**3] 
// 1 	H 	Hydrogen 	 0.99212 	19.2 	8.375E-05
//2 	He 	Helium 	         0.49968 	41.8 	1.663E-04
//3 	Li 	Lithium 	 0.43221 	40.0 	5.340E-01
//4 	Be 	Beryllium 	 0.44384 	63.7 	1.848E+00
//5 	B 	Boron 	         0.46245 	76.0 	2.370E+00
//6 	C 	Carbon, Graphite 0.49954 	78.0 	1.700E+00
//7 	N 	Nitrogen 	 0.49976 	82.0 	1.165E-03
//8 	O 	Oxygen 	         0.50002 	95.0 	1.332E-03
//9 	F 	Fluorine 	 0.47372 	115.0 	1.580E-03
//10 	Ne 	Neon 	         0.49555 	137.0 	8.385E-04
//11 	Na 	Sodium 	         0.47847 	149.0 	9.710E-01
//12 	Mg 	Magnesium 	 0.49373 	156.0 	1.740E+00
//13 	Al 	Aluminum 	 0.48181 	166.0 	2.699E+00
//14 	Si 	Silicon 	 0.49848 	173.0 	2.330E+00
//15 	P 	Phosphorus 	 0.48428 	173.0 	2.200E+00
//16 	S 	Sulfur 	0.49897 	180.0 	2.000E+00
//17 	Cl 	Chlorine 	0.47951 	174.0 	2.995E-03
//18 	Ar 	Argon 	0.45059 	188.0 	1.662E-03
//19 	K 	Potassium 	0.48595 	190.0 	8.620E-01
//20 	Ca 	Calcium 	0.49903 	191.0 	1.550E+00
//21 	Sc 	Scandium 	0.46712 	216.0 	2.989E+00
//22 	Ti 	Titanium 	0.45948 	233.0 	4.540E+00
//23 	V 	Vanadium 	0.45150 	245.0 	6.110E+00
//24 	Cr 	Chromium 	0.46157 	257.0 	7.180E+00
//25 	Mn 	Manganese 	0.45506 	272.0 	7.440E+00
//26 	Fe 	Iron 	0.46556 	286.0 	7.874E+00
//27 	Co 	Cobalt 	0.45815 	297.0 	8.900E+00
//28 	Ni 	Nickel 	0.47708 	311.0 	8.902E+00
//29 	Cu 	Copper 	0.45636 	322.0 	8.960E+00
//30 	Zn 	Zinc 	0.45879 	330.0 	7.133E+00
//31 	Ga 	Gallium 	0.44462 	334.0 	5.904E+00
//32 	Ge 	Germanium 	0.44071 	350.0 	5.323E+00
//33 	As 	Arsenic 	0.44046 	347.0 	5.730E+00
//34 	Se 	Selenium 	0.43060 	348.0 	4.500E+00
//35 	Br 	Bromine 	0.43803 	343.0 	7.072E-03
//36 	Kr 	Krypton 	0.42959 	352.0 	3.478E-03
//37 	Rb 	Rubidium 	0.43291 	363.0 	1.532E+00
//38 	Sr 	Strontium 	0.43369 	366.0 	2.540E+00
//39 	Y 	Yttrium 	0.43867 	379.0 	4.469E+00
//40 	Zr 	Zirconium 	0.43848 	393.0 	6.506E+00
//41 	Nb 	Niobium 	0.44130 	417.0 	8.570E+00
//42 	Mo 	Molybdenum 	0.43777 	424.0 	1.022E+01
//43 	Tc 	Technetium 	0.43919 	428.0 	1.150E+01
//44 	Ru 	Ruthenium 	0.43534 	441.0 	1.241E+01
//45 	Rh 	Rhodium 	0.43729 	449.0 	1.241E+01
//46 	Pd 	Palladium 	0.43225 	470.0 	1.202E+01
//47 	Ag 	Silver 	0.43572 	470.0 	1.050E+01
//48 	Cd 	Cadmium 	0.42700 	469.0 	8.650E+00
//49 	In 	Indium 	0.42676 	488.0 	7.310E+00
//50 	Sn 	Tin 	0.42120 	488.0 	7.310E+00
//51 	Sb 	Antimony 	0.41889 	487.0 	6.691E+00
//52 	Te 	Tellurium 	0.40752 	485.0 	6.240E+00
//53 	I 	Iodine 	0.41764 	491.0 	4.930E+00
//54 	Xe 	Xenon 	0.41130 	482.0 	5.485E-03
//55 	Cs 	Cesium 	0.41383 	488.0 	1.873E+00
//56 	Ba 	Barium 	0.40779 	491.0 	3.500E+00
//57 	La 	Lanthanum 	0.41035 	501.0 	6.154E+00
//58 	Ce 	Cerium 	0.41395 	523.0 	6.657E+00
//59 	Pr 	Praseodymium 	0.41871 	535.0 	6.710E+00
//60 	Nd 	Neodymium 	0.41597 	546.0 	6.900E+00
//61 	Pm 	Promethium 	0.42094 	560.0 	7.220E+00
//62 	Sm 	Samarium 	0.41234 	574.0 	7.460E+00
//63 	Eu 	Europium 	0.41457 	580.0 	5.243E+00
//64 	Gd 	Gadolinium 	0.40699 	591.0 	7.900E+00
//65 	Tb 	Terbium 	0.40900 	614.0 	8.229E+00
//66 	Dy 	Dysprosium 	0.40615 	628.0 	8.550E+00
//67 	Ho 	Holmium 	0.40623 	650.0 	8.795E+00
//68 	Er 	Erbium 	0.40655 	658.0 	9.066E+00
//69 	Tm 	Thulium 	0.40844 	674.0 	9.321E+00
//70 	Yb 	Ytterbium 	0.40453 	684.0 	6.730E+00
//71 	Lu 	Lutetium 	0.40579 	694.0 	9.840E+00
//72 	Hf 	Hafnium 	0.40338 	705.0 	1.331E+01
//73 	Ta 	Tantalum 	0.40343 	718.0 	1.665E+01
//74 	W 	Tungsten 	0.40250 	727.0 	1.930E+01
//75 	Re 	Rhenium 	0.40278 	736.0 	2.102E+01
//76 	Os 	Osmium 	0.39958 	746.0 	2.257E+01
//77 	Ir 	Iridium 	0.40058 	757.0 	2.242E+01
//78 	Pt 	Platinum 	0.39984 	790.0 	2.145E+01
//79 	Au 	Gold 	0.40108 	790.0 	1.932E+01
//80 	Hg 	Mercury 	0.39882 	800.0 	1.355E+01
//81 	Tl 	Thallium 	0.39631 	810.0 	1.172E+01
//82 	Pb 	Lead 	0.39575 	823.0 	1.135E+01
//83 	Bi 	Bismuth 	0.39717 	823.0 	9.747E+00
//84 	Po 	Polonium 	0.40195 	830.0 	9.320E+00
//85 	At 	Astatine 	0.40479 	825.0 	1.000E+01
//86 	Rn 	Radon 	0.38736 	794.0 	9.066E-03
//87 	Fr 	Francium 	0.39010 	827.0 	1.000E+01
//88 	Ra 	Radium 	0.38934 	826.0 	5.000E+00
//89 	Ac 	Actinium 	0.39202 	841.0 	1.007E+01
//90 	Th 	Thorium 	0.38787 	847.0 	1.172E+01
//91 	Pa 	Protactinium 	0.39388 	878.0 	1.537E+01
//92 	U 	Uranium 	0.38651 	890.0 	1.895E+01
//-----------------------------------------------------------------

#include <FoilNew.h>
#include <Globals.h>
#include <Landau.h>
#include <Vavilov.h>
#include <Moliere.h>
#include <Constants.h>
#include <Coordinates.h>
#include <fmt/format.h>
#include <OptimExceptions.h>
#include <random>
#include <chrono>
#include <map>
#include <algorithm>
#include <TrackParam.h> // DEBUG

using  Constants::PI; 
using  Constants::MP_MEV; 
using  Constants::a0;                                 // [m] Bohr radius 
static const double r_p =  Constants::r_p*100;        // [cm] classical proton radius 
static const double pi  = PI;
static const double me  = Constants::ME_MEV*1.0e6;    // electron rest mass [eV]
static const double mp  = Constants::MP_MEV*1.0e6;    // proton rest mass [eV]
static const double Atheta = 13.6;                    // [MeV] const used in PDB formula for rms scattering angle 

namespace {
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd() );
  std::normal_distribution<double>       normal(0.0,1.0);  // normal distribution mean=0.0, sigma=1.0
  std::uniform_real_distribution<double> uniform(0.0,1.0);  // uniform distribution mean=0.0, sigma=1.0

  struct Material {
    int    Z;   //  Atomic no
    double A;   //  [amu]     atomic mass
    double rho; //  [g/cm**3] density  
  //double I0;  //  [eV] ionization energy
  };

  static std::map<std::string, Material> materials { 
    //"Hydrogen"     {1,    1.01  },    
    //"Helium",      { 2,    4.00 },   
    {{"Lithium"},     {3,    6.94,  0.534   }},
    {{"Beryllium"},   {4,     9.01, 1.85    }},
    {{"Carbon"},      {6,    12.01, 2.2670  }},
    //"Nitrogen"     {7,    14.01 },
    {{"Aluminium"},   {13,    26.98, 2.70   }}, 
    {{"Silicon"},     {14,    28.09, 2.3296 }},  
    {{"Iron"},        {26,    55.85, 7.874  }},  
    {{"Copper"},      {29,    63.55, 8.933  }},  
    {{"Silver"},      {47,    109.9, 10.501 }},  
    {{"Tungsten"},    {74,    183.9, 19.3   }},   
    {{"Lead"},        {82,    207.2, 11.342 }} };   

} // anonymous namespace 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Compute approximate radiation length for various materials
// Ref: Y-S Tsai. Reviews of Modern Physics vol 46, no 815 1974 

double FoilNew::radLength(int Z, double A)
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

  auto r_e = Constants::r_e * 100; // classical electron radius in cm   
  auto& N_A = Constants::N_avogadro;
  
  double C   = 1.0/(4*alpha*(r_e*r_e)*N_A);

  double val = (A*C)/((Z*Z)*(Lrad(Z)-f(Z))+Z*Lradprime(Z));

  return val; 
  
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FoilNew:: FoilNew()
  : Element()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FoilNew::FoilNew( char const* nm, char const* fnm)
  :  Element(nm, fnm), dE_(1.0282e3), I0_(120.0),vavilov_(), moliere_(), landau_()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FoilNew::FoilNew( FoilNew const& o)
  : Element(o),
  rho_(o.rho_),
  dm_(o.dm_),
  s_(o.s_),
  A_(o.A_),
  Z_(o.Z_),
  Theta0_(o.Theta0_),
  na_(o.na_),
  X0_(o.X0_),
  I0_(o.I0_),
  dE_(o.dE_),
  vavilov_(o.vavilov_),
  moliere_(o.moliere_),
  landau_(o.landau_)
{
  std::cerr << "FoilNew::FoilNew( FoilNew const& o)" << std::endl;
}

  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FoilNew& FoilNew::operator= (FoilNew const& rhs)
{

  std::cerr <<  "FoilNew::operator= " << std::endl;

  if (&rhs == this) return *this;
  
  static_cast<Element>(*this) = rhs;

  rho_ = rhs.rho_;
  dm_  = rhs.dm_;
  s_   = rhs.s_;
  A_   = rhs.A_;
  Z_   = rhs.Z_;
  Theta0_ = rhs.Theta0_;
  na_ = rhs.na_;
  X0_ = rhs.X0_;

  vavilov_ = rhs.vavilov_;
  moliere_ = rhs.moliere_;
  landau_  = rhs.landau_;
  
  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FoilNew::~FoilNew()
{
  std::cerr << "FoilNew::~FoilNew()" << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double FoilNew::Tmax(double gma) const
{
  return 2*Constants::ME_MEV*1.0e6*(gma*gma - 1.0) ;           
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FoilNew::initialize( double ms, double gma)
{
  // **** this function is meant to be called by Element::preTrack() **** 
  
  Vavilov::Parameters vparms;
  vparms.gma    = gma;                      // 1.426;  // 400 MeV
  vparms.dE     = dE_;                      // 1.0282e3;                 // [eV] 1.0282e3 eV "expected" or average energy loss  foil density = 400e-6 g/cm**2  
  vparms.I0     = I0_;                      // 120.0;                    // ionization energy for C 
  vparms.nfft   = 8192*4;                   // number of FFT points used to sample the pdf.  

  vavilov_ = std::make_unique<Vavilov>(vparms);  // anay existing vavilov_ object will be destroyed
    
  Moliere::Parameters mparms;

  double bg  =  sqrt(gma*gma - 1.0);
  double p   =  bg*ms; // momentum, MeV/c
  double bta =  bg/gma;
  
  mparms.thetamin = (2*r_p*Z_)/(bta*bta*gma)*(2*pow(Z_,1.0/3.0))/a0;  

  double Dp = (4*PI*(Z_*Z_)*(r_p*r_p)*na_)/(gma*gma*pow(bta,4)); 

  // NOTE: dm, s0 in in [g/cm**2] 

  mparms.theta0  = Atheta/(bta*p)*sqrt(dm_/X0_)*(1.0+0.038*log(dm_/(bta*bta*X0_))); // scattering angle  
  mparms.Lc      = (mparms.theta0*mparms.theta0)/(s_*Dp);                           // Lc: "scattering logarithm"  

  std::cout << "theta0 = " << mparms.theta0 << std::endl;
  std::cout << "Lc     = " << mparms.Lc  << std::endl;
  moliere_ = std::make_shared<Moliere>(mparms);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  FoilNew::preTrack( RMatrix_t<3>& frame, double ms,    double Enr0, int n_elem, TrackParam& prm, RMatrix& m1) const
{
  double gma =  1.+Enr0/ms;
  const_cast<FoilNew*>(this)->initialize(ms, gma);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  FoilNew::trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v ) const
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
  
  //................................................
  // Vavilov energy loss(multiple scattering)  contribution
  //................................................
  
  double dEv  =   dE_* ((*vavilov_)());

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

void FoilNew::toString( char* buf) const
{ /*** FIX ME ***/}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FoilNew::setParameters( int np, double attributes[], ... )
{ /*** FIX ME ***/

  std::cout << "FoilNew::setParameters( int np, double attributes[], ... )" << std::endl;

  // ep->L     - thickness [cm]
  // ep->B     - density [g/cm^3]
  // ep->N     - atomic number Z
  // ep->G     - dL/dx
  // ep->T     - tilt [deg]
  // ep->ofsX  - WedgeEnd [cm]

  std::cerr <<  " thickness   = " <<  attributes[0] << std::endl;  // in mu g/cm**2
  std::cerr <<  " density     = " <<  attributes[1] << std::endl;
  std::cerr <<  " Z           = " <<  attributes[2] << std::endl;
  std::cerr <<  " dL/dx       = " <<  attributes[3] << std::endl;
  std::cerr <<  " WedgeEnd    = " <<  attributes[4] << std::endl;


  std::cout << " (no of attributes) np = " << np << std::endl;
  for (int i=0; i<np; ++i ) {
    std::cout << " attributes["<<i<<"] = " << attributes[i] << std::endl;
  }
  
  std::cerr <<  " thickness   = " <<  attributes[0] << std::endl;

  // Enr0   - reference kinetic energy in MeV
  // ms     - particle mass in MeV
  // v[i]; i=0 ... 5;   vector of particle coordinates (x, TetaX, y, TetaY, s, DeltaP/p)
  

  // *** hardwired for now -- assuming Carbon foil
  Z_ = int(attributes[1]);

  std::cerr <<  " Z_   = " <<  Z_ << std::endl;

  auto found = std::find_if(begin(materials), end(materials), [this](auto const& item) { return item.second.Z == Z_;});  
  if (found == materials.end()) {
      auto msg = fmt::format("Foil: Material Z = {:d} is not in the database.", Z_);
      throw OptimRuntimeException( msg.c_str());
  }
  else {
  }
  
  A_   = found->second.A;
  rho_ = found->second.rho;       //        density g/m**3

  std::cerr <<  " A_       = " <<  A_ << std::endl;
  std::cerr <<  " rho_     = " <<  rho_ << std::endl;
   
 dm_    = attributes[0]*1.0e-6;        // foil thickness micro-g/cm**2
 X0_    = radLength(Z_, A_);           // A_ should be expressed in amu !!FIXME!!!

  std::cerr <<  "radLength(A,Z) = X0_ = " <<  X0_ << std::endl;
  
  static double const& Mp = Constants::MP_kg*1.0e3; // proton mass in g 
  
						 
  s_   = dm_/rho_;     // foil tkickness in cm
  std::cerr <<  "s_ = " <<  s_ << std::endl;
  std::cerr <<  "dm_ = " <<  dm_ << std::endl;
  std::cerr <<  "rho_ = " <<  rho_ << std::endl;

  Theta0_ = 1.0;              // is this correct ??? 

  na_    = rho_/(A_*Mp);      // [atom/cm**3] atomic density  

  std::cerr <<  "na_ = " << na_ << std::endl;
  std::cerr <<  "A_ = " << A_ << std::endl;
  std::cerr <<  "Mp = " << Mp << std::endl;

  
}
  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

