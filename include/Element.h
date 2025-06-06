//  =================================================================
//
//  Element.h
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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <OptimCalc.h>
#include <RMatrix.h>
#include <cstring>
#include <SplineInterpolator.h>
#include <Cavity.h>
#include <memory>
#include <string>
#include <vector>
#include <boost/iterator/iterator_adaptor.hpp>

class Channel;
class Beamline;

struct ExtData;
struct TrackParam;

class  element_private_access;

class Element {

  friend class element_private_access;
  
 public:
   static std::string type(char etype); 
   static bool    fringe_on; 
   static double  neg_phase_adv_threshold;

   static Element* makeElement( char const* nm,  char const* fnm="");
   static int checkEdge(int j, int ns);  //ns = total no of slices, j = current slice index   
  
   Element(); 
   Element( char    const* nm,  char const* fnm="");
   Element( Element const& o); 
   virtual ~Element();

   Element& operator = (Element const& rhs); 

   virtual Element* clone() const;

   virtual Element*      split(int nslices) const;      // return a sliced element

   char  etype()  const;                          // the element type, encoded as a single upper case character
   
   char const* name()     const;
   char const* fullName() const;
   int slices() const { return slices_; } 

   char const*     name(char const *n);
   char const* fullName(char const *n);

   double length() const; 
   double length(double len); 

   double offsX() const; 
   double offsX( double offset); 
   double offsY() const; 
   double offsY( double offset); 

   double tilt() const; 
   double tilt(double val); 
   double tiltErr() const; 

   int    plane() const; 
  
   static void propagateLatticeFunctions(  RMatrix const& tm, Twiss&   v, std::complex<double> ev[][4] ); // const; // return a sliced element
   static void propagateLatticeFunctions(  RMatrix const& tm, Twiss4D& v, std::complex<double> ev[][4] ); // const; // return a sliced element

   static void swap( Element &e1, Element &e2);
  
   virtual void toString( char* buf) const;
   virtual void setParameters( int np, double attributes[], ... ); // = 0; //cannot be abstract for now 
  virtual void setParameters( int np, std::vector<double> const&, ... ); // = 0;

   RMatrix   rmatrix( double& energy, double ms, double& tetaY, double dalfa, int st=3) const; // calls the virtual function below. 

   virtual RMatrix   rmatrix( double& alfap,  double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
   virtual RMatrix rmatrixsc( double& alfap,  double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;

   void virtual preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;
   int  virtual trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;
   int  virtual     track( double ms,   double& Enr0,  Coordinates& v, double& tetaY ) const; // track trajectory

   //.......................................................................................
   // new interface ... 
   
   virtual std::string str() const {return Element::type(this->etype());} // a string that describes the element type 

   virtual std::shared_ptr<Channel> getChannel(std::string const& name);
   
   virtual Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 
   virtual void    propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& R ) const;
   virtual RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;
   void setSlice(int) const; 
   virtual void preTrack( RMatrix_t<3>& frame, double ms,    double Enr0, int n_elem, TrackParam& prm, RMatrix& m1) const;
   
   // void virtual preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;                                          // 
   // int  virtual trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const; // track one turn
   // int  virtual     track( double ms,   double& Enr0,  Coordinates& v, double& tetaY ) const;                                                                 // track trajectory
   
   //........................................................................................

  // virtual Particle& trackFast( Particle& particle );

 static bool use_mad_frame_;
   double B;  // magnetic field 
   double G;  // gradient 
   double S;  // sextupole ? hor aperture  
   int    N;  // aperture shape
   double A;  // vertical aperture 
   
 protected: 

  static   int backwardTest( TrackParam& pm, int n_elem, int n_turn, Coordinates& v); 
  static   int transAmpTest( TrackParam& pm, int n_elem, int n_turn, Coordinates& v); 

   std::string      name_;
   std::string  fullname_;

   double        L_;  // length
   double  TiltErr_;
   int       plane_; 
   double        T_;  // Tilt ( rotation about local z-axis) 
   double ofsX_; 
   double ofsY_; 

   int            slices_; // no of slices
   mutable int    slice_;  // the current slice
   double         psi_;    // [rad] orientation around local z-axis at entrance of current slice 
   RMatrix_t<3> frame_;    // the element local coordinate frame   
};


class Drift : public Element {
 public:
   Drift(); 
   Drift(char const* nm, char const* fnm="");
   Drift( Drift const& o); 
   Drift& operator = (Drift const& rhs); 
  
   //  virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3);
   RMatrix rmatrixsc(double& alphap, double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;

   RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

   Drift*  clone() const;

   int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

   void toString( char* buf) const;
   void setParameters( int np, double attributes[], ... );
   void setParameters( int np, std::vector<double> const&, ... );

   void setLength( );  
   void length( ) const;  

};

class Instrument : public Element {
 public:
  Instrument(); 
  Instrument(char const* nm, char const* fnm="");
  Instrument( Instrument const& o); 
  Instrument& operator = (Instrument const& rhs); 

  int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  //virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3);  
  virtual RMatrix rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  Instrument*  clone() const { return new Instrument(*this); }
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

};

class CFBend : public Element {
 public:
  CFBend(); 
  CFBend(char const* nm, char const* fnm="");
  CFBend( CFBend const& o); 
  CFBend& operator = (CFBend const& rhs); 

  CFBend*  clone() const { return new CFBend(*this); }
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );
 
  void preTrack( double ms,    double Enr0,  double tetaY, int n_elem,  TrackParam& prm, RMatrix& m1) const;
  int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;
  int  track( double ms, double& Enr0,  Coordinates& v, double& tetaY ) const;

  void    propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& R ) const; // propagate Coordinate system and postion according to OPTIMX convention 

  RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  void preTrack(RMatrix_t<3>& frame, double ms, double Enr0, int n_elem, TrackParam& prm, RMatrix& m1 ) const;
  
 private:

  void    propagate( double hr, double ms, RMatrix_t<3>& W) const; // propagate W only as Frenet-Serret. OPTIMX correction not applied.   
  
};

class Edge : public Element {
 public:
   Edge(); 
   Edge( char const* nm, char const* fnm="");
   Edge( Edge const& o); 
   Edge& operator = (Edge const& rhs); 

   Edge*  clone() const { return new Edge(*this);}
   Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

   void preTrack(double ms, double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1 ) const;
   int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm,  RMatrix const& m1, Coordinates& v) const;

   RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
   RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

   virtual void preTrack( RMatrix_t<3>& frame, double ms,    double Enr0, int n_elem, TrackParam& prm, RMatrix& m1) const;

   void toString( char* buf) const;
   void setParameters( int np, double attributes[], ... );
   void setParameters( int np, std::vector<double> const&, ... );

   bool   isupstream;
   double bendGradient;
  
};

class Quadrupole : public Element {
 public:
   Quadrupole(); 
  ~Quadrupole(); 
   Quadrupole( char const* nm, char const* fnm="");
   Quadrupole( Quadrupole const& o); 
   Quadrupole& operator = (Quadrupole const& rhs); 

   Quadrupole*  clone() const { return new Quadrupole(*this);}
   Quadrupole* split(int nslices) const; // return a sliced element

   RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
   RMatrix rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
   RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

    int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

   void toString( char* buf) const;
   void setParameters( int np, double attributes[], ... );
   void setParameters( int np, std::vector<double> const&, ... );

};

class Solenoid : public Element {
 public:
  Solenoid(); 
  Solenoid( char const* nm, char const* fnm="");
  Solenoid( Solenoid const& o); 
  Solenoid& operator = (Solenoid const& rhs); 
 ~Solenoid() {}

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 
  Solenoid* split(int nslices) const;

  int trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  Solenoid*  clone() const { return new Solenoid(*this);}
 
  RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix rmatrixsc( double& alphap, double& Enr, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3) const;
  RMatrix rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

 private:
  static void SolLinearEdge(RMatrix& mi, double k, double a, int edge);
  static void    SolEdge(Coordinates& v, double k, double a, int edge);

};

class Sextupole : public Element {
 public:
  Sextupole(); 
  Sextupole( char const* nm, char const* fnm="");
  Sextupole( Sextupole const& o); 
  Sextupole& operator = (Sextupole const& rhs); 

  static void sext_trans_old(Element const* el, double Hr, Coordinates* vp, Coordinates const* v);
  static void sext_trans_new(Element const* el, double Hr, Coordinates* vp, Coordinates const* v);
  static void     sext_trans(Element const* el, double Hr, Coordinates* vp, Coordinates const* v);

  int  trackOnce( double ms,   double& Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  // virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  virtual RMatrix rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  Sextupole*  clone() const { return new Sextupole(*this);}
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

};

class Multipole : public Element {
 public:
  Multipole(); 
  Multipole( char const* nm, char const* fnm="");
  Multipole( Multipole const& o); 
  Multipole& operator = (Multipole const& rhs); 

  static void multipole_trans(Element const* el, double ht, Coordinates* vp, Coordinates const* v);

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 
  int  trackOnce( double ms,   double& Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  void    propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& R ) const;

  virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  Multipole*  clone() const { return new Multipole(*this);}
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

 private:
  
   void    propagate( double hr, double ms, RMatrix_t<3>& W) const; // propagate W only as Frenet-Serret. OPTIMX correction not applied.   
};


class PCavity : public Element {
 public:
   PCavity(); 
   PCavity( char const* nm, char const* fnm="");
   PCavity( PCavity const& o); 
   PCavity& operator = (PCavity const& rhs); 

   Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 
   int trackOnce( double ms, double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		 RMatrix const& m1, Coordinates& v) const;
   void preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;

   RMatrix      rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
   RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

   PCavity*       clone() const { return new PCavity(*this);}
   void   setParameters( int np, double attributes[], ... );
   void setParameters( int np, std::vector<double> const&, ... );
   void        toString( char* buf) const;
   PCavity*       split( int nslices) const; // return a sliced element

};


class GCavity : public Element
{

 public:
  GCavity(); 
  GCavity( char const* nm, char const* fnm="");
  GCavity( GCavity const& o); 
  GCavity& operator = (GCavity const& rhs); 

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  virtual RMatrix  rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  GCavity*     clone() const { return new GCavity(*this);}
  void      toString(char* buf) const;
  void setParameters(int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );
  GCavity*     split(int nslices) const; // return a sliced element

  void preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;
  void preTrack( RMatrix_t<3>& frame, double ms,    double Enr0, int n_elem, TrackParam& prm, RMatrix& m1) const;
  int  virtual trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  
  ExtData const* ext_dat_;
  int*     NStep_;

  SplineInterpolator e_;
  
  //ExtData const* const fieldtbl_;
  double   ampl_;
  double   ms_;
  double   wavelen_;
  double   phase_;
  double   phase0_;
  
  INTEGRERR dynamics(double      x,  double *y,   double   *dy, ODEparam *p); 

  static INTEGRERR LongitEquations(double      x,  double *y,   double   *dy, ODEparam *p); 

  static INTEGRERR TransvEquations(double      x,  double *y,   double   *dy, ODEparam *p);

  static INTEGRERR      get_energy(ODEparam   *p,  double Enr1, double *Enr2,       int n);

  static INTEGRERR   get_cav_phase(double    En1,  double dEn,  ODEparam  *p,       int m);

  static INTEGRERR      get_matrix(ODEparam   *p,  double Enr1, double *Enr2,  RMatrix& m, int n);


};

class EAcc : public Element {

 public:
  EAcc(); 
  EAcc( char const* nm, char const* fnm="");
  EAcc( EAcc const& o); 
  EAcc& operator = (EAcc const& rhs); 
  EAcc*  clone() const { return new EAcc(*this);}

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  void preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;
  int trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix rmatrixsc( double& alfap,  double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;
  
  EAcc*  split(int nslices) const;
  void toString(char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );
};

class CFEBend : public Element {

 public:
  CFEBend(); 
  CFEBend( char const* nm, char const* fnm="");
  CFEBend( CFEBend const& o); 
  CFEBend& operator = (CFEBend const& rhs); 

  void    propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& R ) const;

  virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  virtual RMatrix rmatrixsc(double& alfap, double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;
  
  void preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;
  int trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  CFEBend*  clone() const { return new CFEBend(*this);}
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

 private:

  void    propagate( double hr, double ms, RMatrix_t<3>& W) const; // propagate W only as Frenet-Serret. OPTIMX correction not applied.   

};

class EQuadrupole : public Element {
 public:
  EQuadrupole(); 
  EQuadrupole( char const* nm, char const* fnm="");
  EQuadrupole( EQuadrupole const& o); 
  EQuadrupole& operator = (EQuadrupole const& rhs); 

  RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  EQuadrupole*  clone() const { return new EQuadrupole(*this);}
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );
  void toString(char*) const;
};

class TCorrector : public Element {
 public:
  TCorrector(); 
  TCorrector( char const* nm, char const* fnm="");
  TCorrector( TCorrector const& o); 
  TCorrector& operator = (TCorrector const& rhs); 

  
  void preTrack( double ms,    double  Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;
  int  trackOnce( double ms,   double& Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const; 

  RMatrix rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  virtual RMatrix rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  TCorrector*  clone() const { return new TCorrector(*this);}
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

};

class LCorrector : public Element {
 public:
  LCorrector(); 
  LCorrector( char const* nm, char const* fnm="");
  LCorrector( LCorrector const& o); 
  LCorrector& operator = (LCorrector const& rhs); 
  LCorrector*  clone() const { return new LCorrector(*this);}

  int  trackOnce( double ms,   double& Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  //  virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3);
  RMatrix rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

};

class LiLens : public Element {
 public:
  LiLens(); 
  LiLens( char const* nm, char const* fnm="");
  LiLens( LiLens const& o); 
  LiLens& operator = (LiLens const& rhs); 

  LiLens*  clone() const { return new LiLens(*this);}

  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  int  trackOnce( double ms,   double& Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;


  RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st=3 ) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;


};

class XferMatrix : public Element  {
 public:
  XferMatrix(); 
  XferMatrix( char const* nm, char const* fnm="");
  XferMatrix( XferMatrix const& o); 
  XferMatrix& operator = (XferMatrix const& rhs); 

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  RMatrix  rmatrix();
  RMatrix  rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  XferMatrix*  clone() const { return new XferMatrix(*this);}
  void     setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );
  void         setMatrix( RMatrix const& tm);
  void          toString( char* buf) const;
  XferMatrix*     split(int nslices) const; // return a sliced element

  RMatrix tmat_;    

};

class Aperture : public Element {
 public:
  Aperture(); 
  Aperture( char const* nm, char const* fnm="");
  Aperture( Aperture const& o); 
  Aperture& operator = (Aperture const& rhs); 

  Aperture*  clone() const { return new Aperture(*this);}
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v ) const;

  RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  
};

class TScatter : public Element {
 public:
  TScatter(); 
  TScatter( char const* nm, char const* fnm="");
  TScatter( TScatter const& o); 
  TScatter& operator = (TScatter const& rhs); 

  TScatter*  clone() const { return new TScatter(*this);}
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  //  virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

  
};

class LScatter : public Element {
 public:
  LScatter(); 
  LScatter( char const* nm, char const* fnm="");
  LScatter( LScatter const& o); 
  LScatter& operator = (LScatter const& rhs); 

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  LScatter*  clone() const { return new LScatter(*this);}

  int  trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;

  //  virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

};

class BBeam : public Element {
 public:
  BBeam(); 
  BBeam( char const* nm, char const* fnm="");
  BBeam( BBeam const& o); 
  BBeam& operator = (BBeam const& rhs); 

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  BBeam*  clone() const { return new BBeam(*this);}

  RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

};

class WakeField : public Element {

 public:

  WakeField(); 
  WakeField( char const* nm, char const* fnm="");
  WakeField( WakeField const& o); 
  WakeField& operator = (WakeField const& rhs); 

  Beamline*  splitnew(int nslices) const;      // return a sliced element as a beamline 

  WakeField*      clone() const { return new WakeField(*this);}
  RMatrix       rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st=3) const;
  void         toString( char* buf) const;
  void    setParameters( int np, double attributes[], ... );
  void setParameters( int np, std::vector<double> const&, ... );

  RMatrix   rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st=3) const;

};

class element_private_access {
 public:
  int getSlices(Element const& e) { return e.slices_; }  
};

#endif // ELEMENT_H
