//  =================================================================
//
//  Element.cpp
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

#include <iostream>

#include <algorithm> 
#include <cmath>
#include <utility> 

#include <Channel.h>
#include <Beamline.h>
#include <Twiss.h>
#include <TrackParam.h>
#include <Element.h>
#include <Medium.h>
#include <FoilNew.h>
#include <Cavity.h>
#include <Utility.h>
#include <Constants.h>
#include <Coordinates.h>
#include <RMatrix.h>
#include <OptimMessages.h>
#include <Globals.h>

using std::acosh;

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;

bool    Element::fringe_on               = Globals::preferences().fringe_effects_on; // fringe effects on 
double  Element::neg_phase_adv_threshold = Globals::preferences().neg_phase_adv_threshold; 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Element::Element(char const* nm, char const* fnm)
  : 
    name_{nm},
    fullname_{fnm}, 
    L_(0.0),
    B(0.0),
    G(0.0),
    S(0.0),
    T_(0.0),
    A(0.0),
    N(0), 
    plane_(0),
    ofsX_(0.0),
    ofsY_(0.0),
    TiltErr_(0.0),
    slices_(1)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Element::Element()
  : 
  name_(),
  fullname_(), 
  L_(0.0),
  B(0.0),
  G(0.0),
  S(0.0),
  T_(0.0),
  A(0.0),
  N(0),
  plane_(0),
  ofsX_(0.0),
  ofsY_(0.0),
  TiltErr_(0.0),
  slices_(1)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Element::Element( Element const& o)
  :
  name_(o.name_),
  fullname_(o.fullname_), 
  L_(o.L_),
  B(o.B),
  G(o.G),
  S(o.S),
  T_(o.T_),
  A(o.A),
  N(o.N),
  plane_(o.plane_),
  ofsX_(o.ofsX_),
  ofsY_(o.ofsY_),
  TiltErr_(o.TiltErr_),
  slices_(o.slices_)
{}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Element& 
Element::operator=( Element const& rhs)
{
  Element tmp(rhs);
  Element::swap(*this,tmp);

  return *this;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::swap( Element& e1, Element& e2)
{
  using std::swap;
  
  swap( e1.name_,      e2.name_);
  swap( e1.fullname_,  e2.fullname_);
  swap( e1.L_,         e2.L_);
  swap(	e1.B,          e2.B);
  swap(	e1.G,          e2.G);
  swap(	e1.S,          e2.S);
  swap(	e1.T_,          e2.T_);
  swap(	e1.A,          e2.A);
  swap(	e1.N,          e2.N);
  swap(	e1.plane_,     e2.plane_);
  swap(	e1.ofsX_,      e2.ofsX_);
  swap(	e1.ofsY_,      e2.ofsY_);
  swap(	e1.TiltErr_,   e2.TiltErr_);
  swap(	e1.slices_,    e2.slices_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Element* Element::clone() const
{
  // this should NEVER be called !
  std::cerr << " Element::clone()  this should NEVER be called !" << std::endl;
  return new Element(*this);   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Element::~Element()
{
  // std::cout << "Element destructor called." << std::endl;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* Element::name() const
{
  return name_.c_str();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char Element::etype() const
{
  // the element type encoded in the first char
  return toupper(name_[0]);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* Element::name(char const* name) 
{
  name_ = std::string(name);
  return name_.c_str();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* Element::fullName() const
{
  return fullname_.c_str();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* Element::fullName(char const* name) 
{
  fullname_ = std::string(name);
  return fullname_.c_str();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::toString( char* buf) const
{
  sprintf(buf, "%g \t%g \t%g \t%g", B, G, S, T_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Element::offsX() const
{
  return ofsX_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Element::offsX( double offset)
{
  ofsX_ =  offset;
  return ofsX_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Element::offsY() const
{
  return ofsY_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Element::offsY(double offset ) 
{
  ofsY_ = offset;
  return ofsY_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Element::tiltErr() const
{
  return TiltErr_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
double Element::tilt(double value) 
{
  T_ = value;
  return T_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Element::tilt() const
{
  return T_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Element::length() const
{
  return L_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Element::length(double value)
{
  L_ = value;
  return L_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Element::plane() const
{
  return plane_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::shared_ptr<Channel> Element::getChannel( std::string const& name) 
{

  if (name == "length") {
    return std::make_shared<Channel>(L_);
  }
  else if  (name == "gradient") {
    //std::cout << "getting gradient channel" << std::endl; 
    return std::make_shared<Channel>(G);
  }
  else if  (name == "bfield") {
    return std::make_shared<Channel>(B);
  }
  else { 
   return std::shared_ptr<Channel>(nullptr);
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::setParameters( int np, double dat[], ... )
{
  std::cout << "**THIS SHOULD NOT BE CALLED*** Element::setParameters "<< std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::setParameters( int np, std::vector<double> const& dat, ... )
{
  std::cout << "**THIS SHOULD NOT BE CALLED*** Element::setParameters "<< std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::string Element::type(char etype)
{
  switch (etype) {
    
  case 'X': // Transfer matrix
    return "Matrix";
     break;
  case '_': //  beam-beam element
    return "BeamBeamKick";
  case 'Y': //  wake field
    return "WakefieldKick";
     break;
  case 'O': // drift
    return "Drift";
    break;
  case 'I': // instrument
    return "Instrument";
    break;
  case 'K': // corrector
    return "TCorrector";
    break;
  case 'Z': // long corrector
    return "LCorrector";
    break;
  case 'S': // sextupole
    return "Sextupole";
     break;
  case 'T': // t-scatter
    return "TScatter";
    break;
  case 'U': // l-scatter
    return "LScatter";
    break;
  case 'V': // media
    return "Medium";
    break;
  case 'J': // foil
    return "Foil";
    break;
  case 'M':// multipole
    return "Multipole";
     break;
  case 'H': // aperture
    return "Aperture";
      break;
  case 'Q':                             // quad
    return "Quadrupole";
    break;
  case 'L':                             // electrostatic quad
    return "EQuadrupole";
     break;
  case 'F':                             // lithium lens
    return "LiLens";
     break;
   case 'G':  // Edge focusing. G - angle in degrees  S - aperture
     return "Edge";
      break;
   case 'D':
   case 'B':  // combined functions dipole
     return "CFBend";
      break;
   case 'R':  // Generalized Wien Filter
     return "CFEBend";
      break;
   case 'C':  // solenoid
     return "Solenoid";
     break;
   case 'E':  // electrostatic accelerator section
     return "EAcc";
     break;
  case 'A':  // RF accelerator section
    return "PCavity";
     break;
   case 'W':  // RF accelerator section with external file for accelerating field
     return "GCavity";
     break;
  default:
    return "Unknown";
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Element* Element::makeElement( char const* nm, char const* fnm)
{
  char etype = toupper(nm[0]);
  
  switch (etype) {
    
  case 'X': // Transfer matrix
    return new XferMatrix(nm,fnm);
     break;
  case '_': //  beam-beam element
    return new BBeam(nm, fnm);  // arguments ??  was nm, nm 
     break;
  case 'Y': //  wake field
    return new WakeField(nm,fnm);
    break;
  case 'O': // drift
    return new Drift(nm,fnm);
    break;
  case 'I': // instrument
    return new Instrument(nm,fnm);
    break;
  case 'K': // corrector
    return new TCorrector(nm,fnm);
    break;
  case 'Z': // long corrector
    return new LCorrector(nm,fnm);
    break;
  case 'S': // sextupole
    return new Sextupole(nm,fnm);
    break;
  case 'T': // t-scatter
    return new TScatter(nm,fnm);
    break;
  case 'U': // l-scatter
    return new LScatter(nm,fnm);
    break;
  case 'M':// multipole
    return new Multipole(nm,fnm);
     break;
  case 'H': // aperture
    return new Aperture(nm,fnm);
     break;
  case 'Q':                             // quad
    return new Quadrupole(nm,fnm);
    break;
  case 'L':                             // electrostatic quad
    return new EQuadrupole(nm,fnm);
     break;
  case 'F':                             // lithium lens
    return new LiLens(nm,fnm);
     break;
   case 'G':  // Edge focusing. G - angle in degrees  S - aperture
     return new Edge(nm,fnm);
     break;
   case 'D':
   case 'B':  // combined functions dipole
     return new CFBend(nm, fnm);
     break;
   case 'R':  // Generalized Wien Filter
     return new CFEBend(nm, fnm);
     break;
   case 'C':  // solenoid
     return new Solenoid(nm,fnm);
     break;
   case 'E':  // electrostatic accelerator section
     return new EAcc(nm, fnm);
     break;
  case 'A':  // RF accelerator section
    return new PCavity(nm,fnm);
     break;
  case 'W':  // RF accelerator section with external file for accelerating field
     return new GCavity(nm,fnm);
     break;
  case 'V':  // Medium
     return new Medium(nm,fnm);
     break;
  case 'J': // foil
    return new FoilNew(nm,fnm);
    break;
  }
 
  return 0; // error   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Element* Element::split(int nslices) const
{
  Element* e = this->clone();
  e->L_ /= nslices;
  e->slices_  = nslices;
  return e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Element::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  double P      = sqrt(energy *(energy + 2.0 * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1.0 + energy/ms;

  RMatrix mi;
  mi.toUnity();
  
  mi[0][1] = mi[2][3] = L_;
  mi[4][5] = L_/(gamma1*gamma1);

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix  Element::rmatrix( double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  // in this version of the function, alfap is ignored. 
  double alfap = 0.0; // net rotation of the local 'y-axis' around local beam axis
  return rmatrix(alfap, energy, ms, tetaY, dalfa, st);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::propagateLatticeFunctions( RMatrix const& tm, Twiss& v,  std::complex<double> ev[][4]) // const
{

  // propagate lattice functions through an element with matrix tm. 

  for(int i=0; i<4; ++i){

    ev[1][i] = {0.0, 0.0};
    ev[3][i] = {0.0, 0.0};

      for(int j=0; j<4; ++j) {
   	ev[1][i] += tm[i][j]*ev[0][j];
      	ev[3][i] += tm[i][j]*ev[2][j];
      }
   }
   
   double dnux = -arg(ev[1][0]/ev[0][0])/(2.*PI);

   if ( dnux < -neg_phase_adv_threshold )  { dnux +=0.5; }   // not sure at all why this would be needed

   double dnuy = -arg(ev[3][2]/ev[2][2])/(2.*PI);

   if ( dnuy < -neg_phase_adv_threshold)  { dnuy +=0.5;  }  // not sure at all why this would be needed  

   for(int i=0; i<4; ++i) {
     ev[0][i] = ev[1][i];
     ev[2][i] = ev[3][i];
   }

   Twiss4D v4;
   v4.betatronFromEV(ev);
 
   v.nuX += dnux;
   v.nuY += dnuy;

   v.BtX = v4.btx1 /(1-v4.u);
   v.BtY = v4.bty2 /(1-v4.u);
   v.AlX = v4.alfx1/(1-v4.u);
   v.AlY = v4.alfy2/(1-v4.u);

   v = tm * (Vector_t<6,double>) v;  //  dispersion computation.
                              //  Dispersion is extracted and returned as a coordinate vector.

   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::propagateLatticeFunctions( RMatrix const& tm, Twiss4D& v,  std::complex<double> ev[][4]) // const
{

  //  We propagate only the eigenvectors ev[0][i] and  ev[2][i] since
  //  ev[1][i] and  ev[3][i] are complex conjugate and carry no
  //  additional information. 

  //  The two eigenvectors ev[1][i] and ev[3][i] are used here to
  //  store the **propagated**  ev[0][i]  and  ev[2][i]. 
  
   for(int i=0; i<4; ++i) {
     ev[1][i] = {0.0, 0.0};
     ev[3][i] = {0.0, 0.0};

     // propagate ev[0][i] and ev[2][i] and store the result in ev[1][i] and ev[3][i]

     for(int j=0; j<4; ++j) {
       ev[1][i] += tm[i][j]*ev[0][j];
       ev[3][i] += tm[i][j]*ev[2][j];
     }
   }

   // we assume that a very small negative phase advance over a single element
   // is not real. The threshold value should be under user control ... 

   double dnu1 = -arg(ev[1][0]/ev[0][0])/(2.*PI);   if (dnu1 < -neg_phase_adv_threshold) dnu1 += 1.0;
   double dnu2 = -arg(ev[3][2]/ev[2][2])/(2.*PI);   if (dnu2 < -neg_phase_adv_threshold) dnu2 += 1.0;

   v.qx += dnu1;
   v.qy += dnu2;

   // set the new input eigenvectors to the previous output eigenvectors   
   
   for(int i=0; i<4; ++i){
      ev[0][i] = ev[1][i];
      ev[2][i] = ev[3][i];
   }

   v.betatronFromEV(ev); 
   v = tm * (Vector_t<6,double>) v;  //  dispersion computation.
                              //  Dispersion is extracted and returned as a coordinate vector.

}
      
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::preTrack(double ms, double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{   
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);

   m1 = Element::rmatrix(Enr0, ms, tetaY, 0.0, 3);  // NOTE: this non-virtual function calls the element-specific virtual function
                                                    // with alfap as first argument, with alfap=0.0. This is appropriate for all
                                                    // NON-BENDING elements. Bending element slices have alfap !=0.0 
                                                    // rmatrix(0.0, Enr0, ms, tetaY, 0.0, 3)      
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Element::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		      RMatrix const& m1, Coordinates& v) const
{
  int status = 0; 
  if ( (status = backwardTest(prm, n_elem, n_turn, v)) ) return status;

  //RMatrix tm = e.rmatrix(Enr0, ms, tetaY, 0.0, 3);
  //v = tm*v;
  
   v.c  = m1*v.c;
   
 done:	

   if ( (status = transAmpTest(prm, n_elem, n_turn, v)) ) return status; 

  return 0;  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Element::track( double ms, double& Enr,  Coordinates& v, double& tetaY ) const

{
  int ns = 1; // for now, just one slice
  int j  = 0; // currentslice for now, 0

  
  // ms    = particle rest mass
  // Enr   = particle kinetic energy
  // tetaY = reference frame angle w/r to vertical
  
  RMatrix me = rmatrix(Enr, ms, tetaY, 0.0, checkEdge(j,ns));
  v.c = me*v.c;
  
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Element::checkEdge(int j, int ns)
{
  if(ns <= 1)       return 3;   // TOTAL transfer matrix (upstream edge + body + downstream edge)
   if(j==0)         return 1;   // FIRST slice ( BODY + upstream edge )
   if(j == (ns-1) ) return 2;   // LAST slice  (BODY + downstream edge)
   return 0;                    // NO EDGES 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


RMatrix Element::rmatrixsc(double& alphap, double& Enr, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{

  // X  matrix 
  // W  general cavity
  // H  aperture
  // M  multipole 
  // G  edge
  // A  pbox cavity 
    
  double P  = Enr+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(Enr+ms);
  double alfap = 0.;

  // space charge has no effect here
  // return zero current case

  return rmatrix( alfap, Enr, ms, tetaY, dalfa, st );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Element::backwardTest( TrackParam& prm, int n_elem, int n_turn, Coordinates& v) 
{ 

  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  


 if( p <0.0) {
    v.lost  = 2;
    v.nelem = n_elem+1;
    v.npass = n_turn;
    return 1;
  }

 return 0;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Element::transAmpTest(TrackParam& prm, int n_elem, int n_turn, Coordinates& v)
{
  if( fabs(v[0])>1000.0 || fabs(v[2])>1000.0) {

    v.lost  = 1;
    v.nelem = n_elem+1;;
    v.npass = n_turn;;

    return 2;
  }
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


#if 0 

// possible cleanup
// make trackOnce NOT virtual
// make new virtual trackOncePrivate 


int Element::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		                     RMatrix const& m1, Coordinates& v ) const
{
  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  

  if( p <0.0) {
    v.lost  = 2;
    v.nelem = n_elem+1;
    v.npass = n_turn;
    return 1;
  }

  // the private function is virtual and element specific   
  this->trackOncePrivate(ms, Enr0, n_elem, n_turn, prm, m1, v);
   

  if( fabs(v[0])>1000.0 || fabs(v[2])>1000.0) {
    v.lost  = 1;
    v.nelem = n_elem+1;
    v.npass = n_turn;
    return 2;
  }

  return 0;  
}
#endif


