//  =================================================================
//
//  Analyze2.cpp
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
#include <cmath>

#include <OptimMainWindow.h>
#include <OptimMessages.h>
#include <Utility.h>
#include <Element.h>
#include <Constants.h>
#include <Beamline.h>
#include <RMatrix.h>
#include <TrackParam.h>

using Constants::PI;
using Constants::C_DERV1;
using std::shared_ptr;
using Utility::factorial;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::analyze2 (Coordinates& v)
{
  // *** NOTE ***
  // analyze2 is called only when the lattice is analyzed
  // on the "excited" (reference)  orbit. 

  std::vector<std::shared_ptr<Element>>  edict; // new element dictionary 
  Beamline bml;   // new beamline  

  char buf[1024];

  // Count new numbers for Elements and references

   
  int NElm2     = 0;
  int NElmList2 = 0;
  int j         = 0;
  
  for( int i =0; i<nelm_; ++i){
    auto ep = beamline_[i];
    switch ( ep->etype() ) {
    // Elements which are converted to a single Element
    case 'I': 
    case 'X': 
    case 'T': 
    case 'U':
    case 'O': 
    case 'H':
      ++NElm2;
      ++NElmList2;
      break;
    // Elements which are described by three Elements
    case 'Q': 
    case 'K': 
    case 'C':
      NElm2     += 3;
      NElmList2 += 3;
      break;
    // Elements which are described by five Elements
    case 'S':
      NElm2     += 5;
      NElmList2 += 5;
      break;
    // Multipole
    case 'M':
      if(ep->N >= 0){
        NElm2     += (ep->N + 1);
        NElmList2 += (ep->N + 1);
      }
      else {
        ++NElm2;
        ++NElmList2;
      }
      break;
      // Edge
      case 'G':
        if(i>0 && i<nelm_-1){
          if( (( beamline_[i-1]->etype() !='B') && ( beamline_[i-1]->etype() !='D')) &&
	      (( beamline_[i+1]->etype() !='B') && ( beamline_[i+1]->etype() !='D')) ) {
erredge:    sprintf(buf,"Edge Element %s[%d] does not have nearby dipole", ep->name(), i+1);
            OptimMessageBox::warning(this, "Parsing lattice on excited orbit", buf, QMessageBox::Ok);
	    return 1;
          }
        }
        if(i==0) {
          if( (beamline_[i+1]->etype() !='B') && (beamline_[i+1]->etype() !='D') ) goto erredge;
        }
        if(i==nelm_-1){
          if( (beamline_[i-1]->etype() !='B') && (beamline_[i-1]->etype() !='D') ) goto erredge;
        }
        NElm2     +=1;
        NElmList2 +=1;    
        break;
      case 'B': 
      case 'D':
	  if (i==0) {
	    sprintf(buf, "Line starting with a dipole (bend) is not allowed.\n Please insert a zero-length drift.");
	    OptimMessageBox::warning(this, "Parsing lattice on excited orbit", buf, QMessageBox::Ok);
	    return 1;
	  }
	  if (i< nelm_-1) {
	    auto t = beamline_[i+1]->etype();
	    if (t == 'B' || t == 'D') { 
	      sprintf(buf, "Consecutive dipoles (bends) are not allowed.\n Please insert a zero-length drift.");
	      OptimMessageBox::warning(this, "Parsing lattice on excited orbit", buf, QMessageBox::Ok);
	      return 1;
	    }
	  }

	NElm2     +=5;
        NElmList2 +=5;
	break;
      case 'A': 
      case 'W':  
      case 'E': 
      case 'Z':
      case 'L': 
      case 'R':  
      case 'F':
        ++NElm2;
        ++NElmList2;
	break;
      default:
        OptimMessageBox::warning(this, "Parsing lattice on excited orbit", "Internal error in Analyze2.\n Please report this error to the developers.", QMessageBox::Ok);
        return 1;
    }
  }
   // Allocate memory

   bml.resize(NElm2);             // Pointer to Array of pointers with length NElm2
   edict.resize(NElmList2);     //Array keeping Element parameters,Length is equal to NElmList2


   //***FIXME*** !!!!!!!
   //if(!(beamline_ && elmdict_ )) {
   //   OptimMessageBox::warning(this, "Can not get enough memory for the task","Conversion Error", QMessageBox::Ok);
   // }
   // creates derivative structure

   RMatrix     me;
   TrackParam  prm;
   double Enrtmp;
   double Hr;
   double alfa_exit;
   double r0;
   double fi0;
   double xp;
   double yp;
   double x;
   double y;
   double px;
   double py;
   double px_exit;
   double tilt;
   double s1;
   double s2;
   double fidip;
   double tiltdip;
   int m;
   int n;
   int ibend = 0;
   int nbend;
   int dtype;
 
   Coordinates vold = v;
   //  ************************ WARNING *********************WARNING ******************************************
   double tetaY  = tetaYo0_; // this is the value defined in the lattice file IT WILL BE WRONG IF tetaY is overriden from the menu !!! FIXME 
   
   double Enr    = Ein;
   double p0     = (1.+v[5])*sqrt(2.*ms*Ein+Ein*Ein);
   double Enr0   = sqrt(p0*p0+ms*ms)-ms;
   double Hr0    = p0/C_DERV1;

   NElm2     = 0;
   NElmList2 = 0;

   for( int i=0; i<nelm_; ++i ){
     auto  ep = beamline_[i];
     //preTrack (*ep, Enr,  tetaY, i, prm, me);
     ep->preTrack (ms, Enr,  tetaY, i, prm, me);

     if( ibend==0)  vold = v;
     
     if(  (ep->etype() =='G') || (ep->etype() =='B') 
                              || (ep->etype() =='D') ) ++ibend;

     Enrtmp = Enr;

     if( ep->trackOnce( ms, Enrtmp, i, 0, prm, me, v)){
       sprintf(buf,"Particle lost at Element %d (%s)", i+1, ep->name());
       OptimMessageBox::warning(this, "Analyze on reference trajectory", buf, QMessageBox::Ok);
       return 1;
      }
      if(ibend){
      	if(i<nelm_-1){
      	  if( (   beamline_[i+1]->etype() !='G')
	      && (beamline_[i+1]->etype() !='B')
              && (beamline_[i+1]->etype() !='D') ) {nbend=ibend; ibend=0;}
        }
      	else {nbend=ibend; ibend=0;}
      }
      me = ep->rmatrix(x, Enr, ms, tetaY, 0.0, 3);

      if( ( ep->etype() == 'B') || (ep->etype() == 'D') ) alfa_exit = PI*x/180.0;
      if(ibend)continue;

      xp = v[1]-vold[1];
      yp = v[3]-vold[3];

      // creates new lattice
      Hr = prm.Hr0 * (1.0 +v[5]);

      shared_ptr<Element> etmp;
      std::string tmpstr;
      
      switch ( ep->etype() ){
        //Elements which are described by the same parameters
        case 'I': 
        case 'X': 
        case 'T': 
        case 'U':
	  etmp = shared_ptr<Element>( ep->clone() );
          bml[NElm2++] = edict[NElmList2++] = etmp;
	  break;
	//Elements which are converted to a single Element
        case 'O':
	  etmp = shared_ptr<Element>( ep->clone() );
          etmp->length( ep->length()*sqrt(1.0 + v[1]*v[1] + v[3]*v[3]) );
          bml[NElm2++] = edict[NElmList2++] = etmp;
	  break;
        case 'H':
	  etmp = shared_ptr<Element>( ep->clone() );
          etmp->B -= v[0];
          etmp->G -= v[2];
          bml[NElm2++] = edict[NElmList2++] = etmp;
	  break;
        // Elements which are described by three Elements
        case 'Q': 
        case 'C':
	  tmpstr = std::string("m") + ep->fullName(); 
	  etmp = shared_ptr<Element>( Element::makeElement("m",  tmpstr.c_str()));
          etmp->N = 0;
          etmp->length(0.0);
          etmp->S = 0.5 * Hr * sqrt(xp*xp + yp*yp);
          etmp->tilt( (etmp->S > 0.0) ? 180.0*atan2(-yp, -xp)/PI : 0.0);
	  bml[NElm2++] = edict[NElmList2++] = etmp;

	  etmp = shared_ptr<Element>( ep->clone() );
	  bml[NElm2++] = edict[NElmList2++] = etmp;

	  tmpstr = std::string("m") + ep->fullName(); 
	  etmp = shared_ptr<Element>( Element::makeElement("m", tmpstr.c_str() ));
	  etmp->N = 0;
          etmp->length(0.0);
          etmp->S = edict[NElmList2-2]->S;
          etmp->tilt( edict[NElmList2-2]->tilt() );
          bml[NElm2++] = edict[NElmList2++] = etmp;
	  break;
        case 'K':
	  tmpstr = std::string("o") + ep->fullName(); 
	  etmp = shared_ptr<Element>( Element::makeElement("o", tmpstr.c_str() ));
          etmp->length(ep->length()*0.5);
          bml[NElm2++] = edict[NElmList2++] = etmp;

	  tmpstr = std::string("m") + ep->fullName(); 
	  etmp = shared_ptr<Element>( Element::makeElement("m", tmpstr.c_str() ));
          etmp->N = 0;
          etmp->length(0.0);
          etmp->S = -ep->length() * ep->B;
          bml[NElm2++] = edict[NElmList2++] = etmp;

	  tmpstr = std::string("o") + ep->fullName(); 
	  etmp = shared_ptr<Element>( Element::makeElement("o", tmpstr.c_str() ));
          etmp->length(ep->length()*0.5);
          bml[NElm2++] = edict[NElmList2++] = etmp;
	  break;
        // Elements which are described by five Elements
        case 'S':
          x  = 0.5*(v[0]+vold[0]);
          y  = 0.5*(v[2]+vold[2]);
          r0 = sqrt(x*x + y*y);

	  fi0 = (r0 >0.0) ? 180.0/PI*atan2(y, x) : 0.0;
	  
	  tmpstr = std::string("o") + ep->fullName();
	  etmp = shared_ptr<Element>( Element::makeElement("o",  tmpstr.c_str() ));
          etmp->length(ep->length()*0.5);
          bml[NElm2++] = edict[NElmList2++] = etmp;

	  tmpstr = std::string("m") + ep->fullName();
	  etmp = shared_ptr<Element>( Element::makeElement("m",  tmpstr.c_str() ));
          etmp->N = 0;
          etmp->length(0.0);
          etmp->S = Hr * sqrt(xp*xp + yp*yp);
	  etmp->tilt( (etmp->S>0.0) ? 180.*atan2(-yp, -xp)/PI : 0.0);
          bml[NElm2++] = edict[NElmList2++] = etmp;

          tmpstr = std::string("m") + ep->fullName();
	  etmp = shared_ptr<Element>( Element::makeElement("m", tmpstr.c_str() ));
          etmp->N = 1;
          etmp->length(0.0);
          etmp->S = ep->S*ep->length()*r0;
          etmp->tilt( (3.*ep->tilt() - fi0)/2. );
          bml[NElm2++] = edict[NElmList2++] = etmp;

          tmpstr = std::string("m") + ep->fullName();
	  etmp = shared_ptr<Element>( Element::makeElement("m", tmpstr.c_str() ));
          etmp->N = 2;
          etmp->length(0.0);
          etmp->S = ep->S*ep->length();
          bml[NElm2++] = edict[NElmList2++] = etmp;

          tmpstr = std::string("o") + ep->fullName();
	  etmp = shared_ptr<Element>( Element::makeElement("o", tmpstr.c_str() ));
          etmp->length(ep->length()*0.5);
          bml[NElm2++] = edict[NElmList2++] = etmp;
	  break;
        // Multipole
	case 'M':
	  etmp= shared_ptr<Element>( ep->clone() );
          if(ep->N > 0){
            etmp->N = 0;
            etmp->length(0.0);
            etmp->S = Hr * sqrt(xp*xp + yp*yp);
            etmp->tilt( (etmp->S>0.0 ) ?  180.*atan2(-yp, -xp)/PI : 0.0);
          }
          bml[NElm2++] = edict[NElmList2++] = etmp;
          if(ep->N > 0){
            r0 = sqrt(v[0]*v[0] + v[2]*v[2]);
            fi0 = (r0>0.0) ? 180.0/PI*atan2(v[2], v[0]) : 0.0;
            m=ep->N;
            for(n=1; n <= m; ++n){
	      etmp = shared_ptr<Element>( ep->clone() );
              etmp->N = n;
              etmp->length(0.0);
              etmp->S = ep->S * std::pow(r0, m-n) / double(factorial(m-n));
              etmp->tilt( ep->tilt() - (m-n)*(fi0 - ep->tilt())/((double)(n+1)) );
              bml[NElm2++] = edict[NElmList2++] = etmp;
            }
	  }
	  break;
        // Dipole
        case 'B': 
        case 'D': 
        case 'G':
	  
          // Determine dipole type
          if(nbend>3){
            sprintf(buf, "Dipole %s[%d] is described by more than 3 Elements", beamline_[i-nbend+1]->name(), i-nbend+2);
	  diperr:     OptimMessageBox::warning(this, "Parsing lattice on excited orbit", buf, QMessageBox::Ok);
            return 1;
          }
	  if(nbend==1)  dtype=0;  // B

	  if(nbend==2) {
            dtype=0;
            if( (beamline_[i-1]->etype()== 'G') &&
                ((beamline_[i]->etype()  == 'B') || ( beamline_[i]->etype() !='D') ))  dtype=1; // GB

	    if( (beamline_[i]->etype() == 'G' ) &&
                ((beamline_[i-1]->etype()== 'B') ||  (beamline_[i-1]->etype() !='D') )) dtype=2; // BG
            if(dtype==0) {
              sprintf(buf, "[1] Incorrect sequence of elements to describe dipole %s[%d]", beamline_[i-nbend+1]->name(), i-nbend+2);
              goto diperr;
            }
          }

	  if(nbend==3) {
            if( (beamline_[i-2]->etype() =='G') &&
                ((beamline_[i-1]->etype() =='B') || (beamline_[i-1]->etype() =='D')) &&
                (beamline_[i]->etype()   =='G') ) dtype=3; // GBG
	    else{
	      std::cerr << beamline_[i-2]->name() << " "
			<< beamline_[i-1]->name() << " "
			<< beamline_[i]->name() << std::endl;
              sprintf(buf, "[2] Incorrect sequence of elements to describe dipole %s[%d]", beamline_[i-nbend+1]->name(), i-nbend+2);
              goto diperr;
            }
          }
          // Build new dipole; dtype = B, GB, BG, GBG
          if(dtype==0 || dtype==1){ j=i;   ep=beamline_[j]; } // B, GB
          else                    { j=i-1; ep=beamline_[j];}  // BG, GBG;    ep and j - points to the dipole body
           
          tilt    =  ep->tilt()*PI/180.;
          x       =  vold[0]*cos(tilt) + vold[2]*sin(tilt);
          y       = -vold[0]*sin(tilt) + vold[2]*cos(tilt);
          px      =  vold[1]*cos(tilt) + vold[3]*sin(tilt);
          py      = -vold[1]*sin(tilt) + vold[3]*cos(tilt);
          px_exit =  v[1]*cos(alfa_exit) + v[3]*sin(alfa_exit);
          fidip   =  ep->length()*ep->B/prm.Hr0 + (px-px_exit);

          if(dtype==1 || dtype==3){ // GB, GBG
	    tmpstr = std::string("o") + ep->fullName();
      	    edict[NElmList2] = shared_ptr<Element>( Element::makeElement("o", tmpstr.c_str() ));
            fi0  = tan(beamline_[j-1]->G*PI/180.);
	    if(ep->B < 0.0)   fi0=-fi0;
            edict[NElmList2]->length( x*fi0/(cos(px)-sin(px)*fi0) );
            bml[NElm2++]         = edict[NElmList2++];
          } 
          else {
            fi0 = 0;
          }
          // input dipole correction
	  tmpstr = std::string("m") + ep->fullName();
      	  etmp = shared_ptr<Element>( Element::makeElement("m", tmpstr.c_str() ));
          etmp->N = 0;
          etmp->length(0.0);
	  if (j==0) {
	    sprintf(buf, "General parsing error."); // this should NEVER be called.
	    goto diperr;
	  }
	  s1   = (ep->B * ep->B * beamline_[j-1]->S / Hr  - fi0 * ep->B)*y;
	  py  += s1/Hr;
          tiltdip = -py*tan(0.5*fidip);
          s2 = (fidip!=0.0) ? py*(1.-sin(fidip)/fidip) + tiltdip*(1.-cos(fidip))/fidip : 0.0;
          etmp->S = s1+s2*Hr;
          etmp->tilt( tilt-90.0);
          bml[NElm2++] = edict[NElmList2++] = etmp;
          // input edge focusing
          if(dtype==0 || dtype==2){	 // B, BG
	    tmpstr = std::string("g") + ep->fullName();
       	    etmp = shared_ptr<Element>( Element::makeElement("g",  tmpstr.c_str() ));
            etmp->G =  (ep->B >= 0.0) ? 180./PI*px : -180./PI*px;
          }
	  else {	//GB, GBG
	    auto  uel = beamline_[j-1];
       	    etmp = shared_ptr<Element>( uel->clone() );
            etmp->G += (ep->B >= 0.0) ? 180./PI*px : -180./PI*px;
          }
          etmp->length(0.0);
          bml[NElm2++] = edict[NElmList2++] = etmp;
          // Magnet body
       	  etmp = shared_ptr<Element>( ep->clone() );
          if(ep->B != 0.0){
             s1       = sqrt(1.-v[3]*v[3]);
             etmp->length(Hr*fidip/ep->B/s1);
             etmp->B *= s1;
          }
          etmp->tilt( etmp->tilt() + 180./PI*tiltdip);  
          bml[NElm2++] = edict[NElmList2++] = etmp;
          // output edge
          if(dtype==0 || dtype==1){	 // B, GB
	    tmpstr = std::string("g") + ep->fullName();
            etmp = shared_ptr<Element>( Element::makeElement("g", tmpstr.c_str() ));
            etmp->G = (ep->B >= 0.0) ? 180./PI*px : -180./PI*px;
          }
	  else {	//BG, GBG
	    auto del =  beamline_[j+1];
            etmp = shared_ptr<Element>( del->clone() );
            etmp->G += (ep->B >= 0.0) ? -180./PI*px : 180./PI*px;
          }
          etmp->length(0.0);
          bml[NElm2++] = edict[NElmList2++] = etmp;
          // output dipole kick
          x =  v[0]*cos(alfa_exit) + v[2]*sin(alfa_exit);
          y = -v[0]*sin(alfa_exit) + v[2]*cos(alfa_exit);
	  tmpstr = std::string("m") + ep->fullName();
          etmp = shared_ptr<Element>( Element::makeElement("m", tmpstr.c_str() ));
          etmp->N = 0;
          etmp->length(0.0);
          if(dtype==2 || dtype==3){ // BG, GBG
	    auto upe = beamline_[j+1]; 
	    fi0 = (ep->B > 0.0) ? tan(upe->G*PI/180.) : -tan(upe->G*PI/180.);
          } 
          else fi0 = 0;
          etmp->S = (ep->B * ep->B * ep->S / Hr  - fi0 * ep->B)*y - s2;
          etmp->tilt(tilt-90.0);
          bml[NElm2++] = edict[NElmList2++] = etmp;
          // output drift
          if(dtype==2 || dtype==3){ // BG, GBG
            tmpstr = std::string("o") + ep->fullName();
            etmp = shared_ptr<Element>( Element::makeElement("o", tmpstr.c_str() ));
            etmp->length( x*fi0/(cos(px_exit) + sin(px_exit)*fi0) );
            bml[NElm2++] = edict[NElmList2++] = etmp;
          }
	  break;
          // Other Elements
        case 'A': 
        case 'W':  
        case 'E': 
        case 'Z':
        case 'L': 
        case 'R':  
        case 'F':
          etmp = shared_ptr<Element>( ep->clone() );
          bml[NElm2++] = edict[NElmList2++] = etmp;
	  break;
        default:
          OptimMessageBox::warning(this, "Internal error in Analyze2. Contact software authors.",
   							"Inverse Lattice", QMessageBox::Ok);
          return 1;
      }
    }
   
   beamline_  = bml;
   elmdict_   = edict;


   nelm_       = NElm2;
   nelmlist_   = NElmList2;
   Ein         = Enr0;
   Hr          = Hr0;
   return 0;
}
