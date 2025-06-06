//  =================================================================
//
//  BeamMoments.h
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

#ifndef BEAMMOMENTS_H
#define BEAMMOMENTS_H

#include <utility>
#include <string>
#include <ostream>
#include <complex>
#include <SymMatrix.h>
#include <sqlite/connection.hpp>

template <int n, typename T>
class RMatrix_t;

struct sqlite3_context;
struct sqlite3_value;
struct sqlite3;

class  Bunch;
struct BeamMoments;

std::ostream& operator<<( std::ostream& os, BeamMoments const& m);

struct BeamMoments { 
 
  BeamMoments(double gamma, Bunch const& v, int n, bool parallel_tracking=false); 
  BeamMoments();

  double&       emitX()       { return eps[0];} 
  double const& emitX() const { return eps[0];}  
  double&       emitY()       { return eps[1];} 
  double const& emitY() const { return eps[1];} 
  double&       emitZ()       { return eps[2];} 
  double const& emitZ() const { return eps[2];} 

  double&       Xav()       { return uavg[0];} 
  double const& Xav() const { return uavg[0];}  
  double&       Yav()       { return uavg[2];} 
  double const& Yav() const { return uavg[2];} 
  double&       Sav()       { return uavg[4];} 
  double const& Sav() const { return uavg[4];} 

  double&       PXav()       { return uavg[1];} 
  double const& PXav() const { return uavg[1];}  
  double&       PYav()       { return uavg[3];} 
  double const& PYav() const { return uavg[3];} 
  double&       PSav()       { return uavg[5];} 
  double const& PSav() const { return uavg[5];} 
 
  double&       Xmax()        { return umax[0];} 
  double const& Xmax() const  { return umax[0];} 
  double&       Ymax()        { return umax[2];} 
  double const& Ymax() const  { return umax[2];} 
  double&       Smax()        { return umax[4];} 
  double const& Smax() const  { return umax[4];}

  double&       PXmax()        { return umax[1];} 
  double const& PXmax() const  { return umax[1];} 
  double&       PYmax()        { return umax[3];} 
  double const& PYmax() const  { return umax[3];} 
  double&       PSmax()        { return umax[4];} 
  double const& PSmax() const  { return umax[5];}

  double&       Xmin()        { return umin[0];} 
  double const& Xmin() const  { return umin[0];} 
  double&       Ymin()        { return umin[2];} 
  double const& Ymin() const  { return umin[2];} 
  double&       Smin()        { return umin[4];} 
  double const& Smin() const  { return umin[4];} 

  double&       PXmin()        { return umin[1];} 
  double const& PXmin() const  { return umin[1];} 
  double&       PYmin()        { return umin[3];} 
  double const& PYmin() const  { return umin[3];} 
  double&       PSmin()        { return umin[5];} 
  double const& PSmin() const  { return umin[5];} 

  double&       sigmaDP()        { return cov[5][5];} 
  double const& sigmaDP()  const { return cov[5][5];} 

  double  eps[3];  // emittances
  double uavg[6];  // phase space coordinate averages 
  double umax[6];  // phase space coordinate max values 
  double umin[6];  // phase space coordinate min values 

  std::complex<double> mode1[6];  // mode1 eigenvector
  std::complex<double> mode2[6];  // mode2 eigenvector
  std::complex<double> mode3[6];  // mode3 eigenvector
   
  SymMatrix_t<6,double> cov;      // the covariance matrix

  double intensity;               // normalized intensity 
  double s;                       // s - long. coordinate along machine
  int    nlost;                   // the number of lost particles
  double bta;
  double gma;
  
  void dbWrite(sqlite::connection& con, int turn, int idx, int nturns);
  void  dbRead(sqlite::connection& con, std::string const& query);

  static void init_moments_table( sqlite::connection& db);

  static void  initDBCustomFunctions(sqlite3* db);
  static void  initDBCustomFunctions(sqlite::connection& con);
  static void     extract_lf_xFunc( sqlite3_context* context, int n, sqlite3_value** values);
  static void   extract_dvec_xFunc( sqlite3_context* context, int n, sqlite3_value** values);
  static void   extract_cvec_xFunc( sqlite3_context* context, int n, sqlite3_value** values);
  static void    extract_cov_xFunc( sqlite3_context* context, int n, sqlite3_value** values);
  static void    extract_cor_xFunc( sqlite3_context* context, int n, sqlite3_value** values);
 
  void          compute_moments( Bunch const& v, double* mu, SymMatrix_t<6,double>& sigma_mtx,  RMatrix_t<4,double>& sigma4_mtx);
  void compute_moments_parallel( Bunch const& v, double* mu, SymMatrix_t<6,double>& sigma_mtx,  RMatrix_t<4,double>& sigma4_mtx);

};


#endif // BEAMMOMENTS_H
