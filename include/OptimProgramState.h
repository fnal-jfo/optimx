//  =================================================================
//
//  OptimProgramState.h
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
#ifndef OPTIMPROGRAMSTATE_H
#define OPTIMPROGRAMSTATE_H


class RMatrix;
class Element;
class Coordinates;
struct ODEparam;

struct OptimProgramState { 

     bool interrupted_;
     bool analyzed_;
     int  BufLen; 

     RMatrix*  tmat_ ; // transfer matrix 
     RMatrix** tmatp_; // array of transfer matrices

     int nelm_;     // [ was: NElm ]
     int nelmlist_; // [ was: NElmList ]      
     int NmbPer;

     Element*  elmdict_; // Elements       // [ was: ElmList] 
     Element** beamline_; // beamline array // [ was: Elm]


     double Ein; 
     double ms;
     double Hr;
     double BetaXin; 
     double AlfaXin; 
     double BetaYin; 
     double AlfaYin;
     double QXin;
     double QYin;
     double ex; 
     double ey; 
     double dpp; 
     double DispXin;
     double DispYin;
     double DispPrimeXin; 
     double DispPrimeYin;      
     double Length;
     double DeltaL;

     double stepint; 
                 
     bool   CtOr;           // Start from file in orbit dialog
     bool   OrbFirstLetter;
     bool   OrbMatchCase;
     char   OrbFilter[1024];

     double xo;             // initial conditions for orbit calculations
     double yo;
     double zo;
     double so;
     double tetaXo;
     double tetaYo; 
     double stepo;     

     double xo0;           // initial conditions for orbit calculations from a file
     double yo0;
     double zo0;
     double so0;
     double tetaXo0; 
     double tetaYo0;

     double dL;
     double dB;
     double dG;
     double dS;
     double dV;
     double dA;
     double dPhase;
  
     Element*  ElmListFit;

     char   ElemNameFCh[27];     // First letters valid as Element names.
     int    LineLFin;            // Line no of Element list last line in editor
     int    LineLIn;             // Line no of first line of Element list
     int    LineIn;            
     int    Ncvg;                // maximum number of iterations allowed for trajectory closure
     double cnvg, CvgErr;        // convergence coefficient and relative error for trajectory closure
     int    NElmListFit;         // Number of different Elements in the Element list for fit save
     int    lineOptiM;           // line no for OptiM marker in lattice file 
     char   funcFilter[1024];    // filter for cmdViewFunctions()
     double stepv;               // Step for  cmdViewFunctions()
     int    startFrom;
     int    stopAt;              // Start/stop from line numbers - for  cmdViewFunctions()
     bool   printnames;
     bool   MatchCase;
     bool   PrintEnerg;
     bool   NoFirstLetter;
     bool   StartSfrom0;
     double scale_read;          // scale for read field integrals
     bool   IncludeMode;
     int    m1, m2;		 // initial and final Element numbers for matrix calculations

     Coordinates   v_anlz;

     int view_elem;              // number of Element to do "View Functions at Element

     int Nmat;
     int Nmatp;
 
     char**       FullElmName;   // Pointer to Array of full names, size is equal to  [NElm][NAME_LENGTH]

     ODEparam p;

     double   cav_accuracy; 
     int          arraylen;       // length of array for plots
     int         n_cav_int;  

     double CouplThreshold;       // threshhold above which View|Betas will report an error
     double space_charge_step_;

     char IncludeFileName[1024];
     int  InclLineNum;

     //  .... beam-beam  
     int     Nbb;         // Full number of beam-beam IPs
     int*    nslicep;    // no umber of slices for the beam-beam Element
     double* Npp; 
     double* emxp; 
     double* emyp; 
     double* sigmasp; 
     double* Scolisp; 

};

#endif
