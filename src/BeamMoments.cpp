//  =================================================================
//
//  BeamMoments.cpp
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

#include <SymMatrix.h>
#include <sqlite3.h>
#include <Twiss.h>
#include <RMatrix.h>
#include <BeamMoments.h>
#include <Coordinates.h>
#include <Bunch.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <vector>
#include <variant>
#include <sqlite/query.hpp>
#include <sqlite/execute.hpp>
#include <sqlite/command.hpp>
#include <sqlite/private/private_accessor.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>

extern "C" {
#include <sqlite3.h>
}
std::ostream& operator<<( std::ostream& os, BeamMoments const& m)
{
  os << "covariance matrix: \n" << RMatrix(m.cov) << "\n" 
     << "averages : \n" <<  Vector_t<6,double>(m.uavg) << "\n"
     << "min : \n" <<  Vector_t<6,double>(m.umin) << "\n"
     << "max : \n" <<  Vector_t<6,double>(m.umax) << "\n"
     << "eigenemittances : \n" <<  Vector_t<2,double>(m.eps) << "\n";
  // << "symmatrix :\n" << SymMatrix::setAsciiMode << m.cov << "\n";
	  
  return os;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamMoments::BeamMoments()
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamMoments::BeamMoments(double gamma, Bunch const& v, int n, bool parallel_tracking)
{

  static const std::complex<double> I(0.0,1.0); 
  static double const pi = 4*atan(1.0);

  static const RMatrix_t<4> U('J');                        // symplectic matrix
  static const RMatrix_t<4,std::complex<double>> Uc('J');  // complex symplectic matrix

  double bg = sqrt(gamma*gamma - 1.0); // beta*gamma
  gma = gamma;
  bta = bg/gamma;
  
  // Define an accumulator set for calculating the mean and the
  // 2nd moment ...

  double mu[6]; // centroids
  
  SymMatrix_t<6,double>& sigma_mtx  = cov;
  RMatrix_t<4> sigma4_mtx;
  if (parallel_tracking) {
    compute_moments_parallel(v, mu, sigma_mtx, sigma4_mtx);
  }
  else {
    compute_moments(v, mu, sigma_mtx, sigma4_mtx);
  }
  
  // compute eigenvectors, eigenvalues and eigen-emittances ... 
  
  std::complex<double>     ev[4][4];
  std::complex<double> lambda[4];
  
  // Find eigenvectors and eigenvalues
  // NOTE: Eigenvectors are returnes as ROWS of ev 

  try {
    (U*sigma4_mtx.inverse()).findEigenValues(lambda, ev, false);
  }
  catch (...) {
    std::cerr << "findEigenValues fails. " << std::endl;
    return; 
  }
    
  eps[0] = 1.0/std::abs(lambda[0]);
  eps[1] = 1.0/std::abs(lambda[2]); 
  
  // eigenvectors normalization
  //---------------------------

  Vector_t<4,std::complex<double>> v0(ev[0]);
  Vector_t<4,std::complex<double>> v1(ev[1]);
  Vector_t<4,std::complex<double>> v2(ev[2]);
  Vector_t<4,std::complex<double>> v3(ev[3]);

  // adjust the phase to make the x-like component pure real  (phase = 0)
  v0 /= v0[0];
  v1 /= v1[0];

  // adjust the phase to make the y-like component pure real  (phase = 0)
  v2 /= v2[2];
  v3 /= v3[2];
  
  // normalize 
  auto c0 = conj(v0)*(Uc*v0);
  auto c1 = conj(v1)*(Uc*v1);
  auto c2 = conj(v2)*(Uc*v2);
  auto c3 = conj(v3)*(Uc*v3);

  v0 =  sqrt(2.0*I/c0)*v0; 
  v1 = sqrt(-2.0*I/c1)*v1; 
  v2 =  sqrt(2.0*I/c2)*v2; 
  v3 = sqrt(-2.0*I/c3)*v3; 

  // save eigenmodes
  std::copy( &v1[0], &v1[v1.size()], &mode1[0]);
  std::copy( &v3[0], &v3[v3.size()], &mode2[0]);
  
  // compute lattice functions  

  Twiss4D t4d;
  
  double& b1x = t4d.btx1;
  b1x = (conj(v1[0])*v1[0]).real(); 

  double& b1y = t4d.bty1;
  b1y = std::abs((conj(v1[2])*v1[2]).real()); 

  double& a1x = t4d.alfx1;
  a1x  = -std::real(v1[1]*sqrt(b1x));

  double& nu1 = t4d.qx;
  nu1 = std::arg(v1[2]/b1y);
  
  double& b2x = t4d.btx2;
  b2x =  std::abs( (conj(v3[0])*v3[0]).real() ); 

  double& b2y = t4d.bty2;
  b2y =  (conj(v3[2])*v3[2]).real(); 

  double& a2y = t4d.alfy2;
  a2y = -std::real(v3[3]*sqrt(b2y));

  double& nu2 = t4d.qy;
  nu2 =  std::arg(v3[0]/b2x);

  double& u  = t4d.u;
  u = 1.0 + std::imag(v1[1]*sqrt(b1x));

  double ua  = -std::imag((v3[1]*sqrt(b2x))/exp(I*nu2));

  double& a1y = t4d.alfy1;
  a1y = -std::real(v1[3]*sqrt(b1y)*exp(-I*nu1));

  double& a2x = t4d.alfx2;
  a2x = -std::real(v3[1]*sqrt(b2x)*exp(-I*nu2));

  t4d.e1 = eps[0];
  t4d.e2 = eps[1];

  auto optimx_logger = spdlog::get("optimx_logger");

  SPDLOG_LOGGER_INFO( optimx_logger, fmt::format("beta_x1 [comp] = {:12.5g}", b1x) );
  SPDLOG_LOGGER_INFO( optimx_logger, fmt::format("beta_y1 [comp] = {:12.5g}", b1y) );
  SPDLOG_LOGGER_INFO( optimx_logger, fmt::format("beta_x2 [comp] = {:12.5g}", b2x) );
  SPDLOG_LOGGER_INFO( optimx_logger, fmt::format("beta_y2 [comp] = {:12.5g}", b2y) );
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamMoments::init_moments_table( sqlite::connection& con)
{

  std::string cmd = "CREATE TABLE IF NOT EXISTS Moments ("
    "turn       INTEGER NOT NULL ,"
    "eidx       INTEGER NOT NULL ,"
    "pathlen    REAL NOT NULL, "
    "umin       BLOB NOT NULL, "
    "umax       BLOB NOT NULL, "
    "uavg       BLOB NOT NULL, "
    "covariance BLOB NOT NULL, "
    "mode1      BLOB NOT NULL, "
    "mode2      BLOB NOT NULL, "
    "mode3      BLOB NOT NULL, "
    "eps1       REAL NOT NULL, "
    "eps2       REAL NOT NULL, "
    "eps3       REAL NOT NULL, "
    "nlost      INTEGER NOT NULL, "
    "PRIMARY KEY (turn ASC, eidx ASC));";

  sqlite::execute(con, cmd, true);
  sqlite::execute(con, R"(DELETE FROM Moments;)", true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
void BeamMoments::compute_moments_parallel( Bunch const& v, double* mutmp, SymMatrix_t<6,double>& sigma_mtx, RMatrix_t<4,double>& sigma4_mtx )
{

  double mu[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};   

  nlost = 0;

  double smtx[6][6];
  for (int i=0; i<6; ++i) {
    umin[i] = umax[i] = 0.0;
    for (int j=0; j<6; ++j) {
      smtx[i][j] = 0.0;
    }
  }

  
#pragma omp parallel for reduction (+:mu,smtx), reduction(min:umin), reduction(max:umax)    
 for ( int k=0; k<v.size(); ++k) { // loop over n particles (Coordinates)

    auto const& particle = v[k];
    if(particle.lost != 0) {
#pragma omp atomic
       ++nlost;
       continue;
    } // skip lost particles

    for (int i=0; i<6; ++i) {
      mu[i]   += particle[i];
      umin[i] = std::min(umin[i],particle[i]);
      umax[i] = std::max(umax[i],particle[i]);
      for (int j=0; j<=i; ++j) {
      	smtx[i][j] += particle[i]*particle[j];  // NOTE: we only fill the entries below the diagonal. (the matrix is symmetric)
      }
    }
  } // loop over particles

  for (int i=0; i<6; ++i) {
     mu[i] /= (v.size()-nlost);
  }
  for (int i=0; i<6; ++i) {
    mutmp[i] = mu[i];
    for (int j=0; j<=i; ++j) {
      sigma_mtx[i][j] = smtx[i][j]/(v.size()-nlost);
      sigma_mtx[i][j] -= mu[i]*mu[j];
    }
  }

  for (int i=0; i<6; ++i) {
    uavg[i] =  mu[i];
  }
  // restrict to transverse degs of freedom   

  for (int i=0; i<4; ++i) {
        for (int j=0; j<4; ++j) {
	  sigma4_mtx[i][j] = sigma_mtx[i][j];
        }
  }

  // and remove dispersion contribution
  double sdpp = sigma_mtx[5][5];
  // NOTE: etax = sigma_mtx[0][5]/sdpp 

  if ( sdpp > 0.0) {  
    for (int i=0; i<4; ++i) {
      for (int j=0; j<4; ++j) {
            sigma_mtx[i][j]  -= (sigma_mtx[i][5]*sigma_mtx[j][5])/sdpp;   
	    sigma4_mtx[i][j]  -= (sigma_mtx[i][5]*sigma_mtx[j][5])/sdpp;   // THIS IS DONE SO THAT sigma4_mtx can be used to compute eigenvectors.  
      }
    }
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamMoments::compute_moments( Bunch const& v, double* mu, SymMatrix_t<6,double>& sigma_mtx, RMatrix_t<4,double>& sigma4_mtx )
{
  
  using namespace boost::accumulators;
  //std::vector<std::vector< accumulator_set<double, stats<tag::covariance<double, tag::covariate1>>>>> accmtx(6);

  SymMatrix_t<6, accumulator_set<double, stats<tag::min, tag::max, tag::mean, tag::covariance<double, tag::covariate1>>>> accmtx;

  //....................................................................................
  // NOTE: unfortunately, the boost accumulators framework requires all operations
  //       on the accumulator set (pushing new data or computing the summary function) to be executed ***sequentially***.
  //       This means: boost accumulators is not thread-safe and cannot run in parallel.
  //.....................................................................................

  nlost = 0;
  
  for ( int k=0; k<v.size(); ++k) { // loop over n particles (Coordinates)
    auto const& particle = v[k];
    if(particle.lost != 0) {
       ++nlost;
       continue;
    } // skip lost particles
    for (int i=0; i<6; ++i) {
      for (int j=0; j<=i; ++j) {
	  accmtx[i][j]( particle[i], covariate1 = particle[j] );
        }
     }
  }
  intensity = 1.0 - double(nlost)/double(v.size());
  
  // extract statistical quantities from accumulator objects ... 

  for (int i=0; i<6; ++i) {

    umin[i] =  boost::accumulators::min(accmtx[i][i]);
    umax[i] =  boost::accumulators::max(accmtx[i][i]);
    uavg[i] =  boost::accumulators::mean(accmtx[i][i]);
    
        for (int j=0; j<=i; ++j) {
	  sigma_mtx[i][j] = covariance(accmtx[i][j]);
        }
  }

  // restrict to 4x4 (transverse) for now ...

  for (int i=0; i<4; ++i) {
        for (int j=0; j<4; ++j) {
	  sigma4_mtx[i][j] = sigma_mtx[i][j];
        }
  }

  // and remove dispersion contribution 

  double sdpp = sigma_mtx[5][5];
  // NOTE: etax = sigma_mtx[0][5]/sdpp 

  if ( sdpp > 0.0) {  
    for (int i=0; i<4; ++i) {
      for (int j=0; j<4; ++j) {
           sigma4_mtx[i][j]  -= (sigma_mtx[i][5]*sigma_mtx[j][5])/sdpp;   
      }
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamMoments::dbWrite(sqlite::connection& con, int turn, int eidx, int nturns)
{

  static bool transaction = false; 
  // serialize the vector into a binary blob ... 

  int const cvsiz = (6*(6+1))/2;

  //  if (turn % 5 ) return; **** FIXME 
  
  static std::vector<unsigned char> covariance_blob(cvsiz*sizeof(double) );
  static std::vector<unsigned char>      mode1_blob(    6*sizeof(std::complex<double>) );
  static std::vector<unsigned char>      mode2_blob(    6*sizeof(std::complex<double>) );
  static std::vector<unsigned char>      mode3_blob(    6*sizeof(std::complex<double>) );

  static std::vector<unsigned char>      umin_blob(    6*sizeof(double) );
  static std::vector<unsigned char>      umax_blob(    6*sizeof(double) );
  static std::vector<unsigned char>      uavg_blob(    6*sizeof(double) );


  unsigned char* pbegin = (unsigned char*) &cov[0][0];
  unsigned char* pend   = pbegin + cvsiz*sizeof(double);

  std::copy( pbegin, pend, &covariance_blob[0]);

  pbegin = (unsigned char*) &mode1[0];
  pend   = (unsigned char*) &mode1[6];

  std::copy( pbegin, pend, &mode1_blob[0] );

  pbegin = (unsigned char*) &mode2[0];
  pend   = (unsigned char*) &mode2[6];

  std::copy( pbegin, pend, &mode2_blob[0] );

  pbegin = (unsigned char*) &mode3[0];
  pend   = (unsigned char*) &mode3[6];

  std::copy( pbegin, pend, &mode3_blob[0] );

  pbegin = (unsigned char*) &umin[0];
  pend   = (unsigned char*) &umin[6];

  std::copy( pbegin, pend, &umin_blob[0] );

  pbegin = (unsigned char*) &umax[0];
  pend   = (unsigned char*) &umax[6];

  std::copy( pbegin, pend, &umax_blob[0] );

  pbegin = (unsigned char*) &uavg[0];
  pend   = (unsigned char*) &uavg[6];

  std::copy( pbegin, pend, &uavg_blob[0] );


  if (!transaction) {
    transaction = true;
    sqlite::execute(con, "BEGIN TRANSACTION;", true);
  }
  
  static std::string sql( "INSERT INTO Moments "
	  	   "(turn, eidx, pathlen, umin, umax, uavg, covariance, mode1,  mode2, mode3, eps1, eps2, eps3, nlost) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?); " );
  
  static sqlite::command insert_moments(con, sql);

  insert_moments.clear();
  insert_moments % turn % eidx % s % umin_blob % umax_blob % uavg_blob
                                   % covariance_blob % mode1_blob % mode2_blob % mode3_blob
                                   % eps[0] % eps[1] % eps[2] % nlost; 
  insert_moments.emit();

  
  if (transaction && ((turn%20 == 0) || (turn == nturns))) {
    sqlite::execute(con, "END TRANSACTION;", true);
    transaction = false;
  }

  //if ( (turn%20) || (turn==nturns) ) return;
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamMoments::dbRead(sqlite::connection& con, std::string const& query)
{
  sqlite::query q(con, query);

  int    turn        = 0;
  int    eidx        = 0;
  double cov_value   = 0.0;
  
  int const sz = (6*(6+1))/2*sizeof(double);

  auto res = q.emit_result();

  int cols = res->get_column_count();

  sqlite::type tp[2];
  tp[0] = res->get_column_type(0);
  tp[1] = res->get_column_type(1);

  //using var_t = std::variant<int, double, std::string, std::vector<unsigned char>>;
  using var_t = std::variant<int, double, std::string>;

   auto getval = [&res, &tp](int col) {
             using namespace sqlite;
	     std::vector<unsigned char> vec;
	     var_t var;
                                         switch (tp[col]) {
                                         case integer:
					     var = res->get_int(col);
					   break;
                                          case blob:
					      res->get_binary(col, vec);
					      var = 0;
                                          case text:
					      var =res->get_string(col);
	                                    break;
                                          case real:
					      var = res->get_double(col);
					   break;
					  case null:
					  case unknown:
					  default:
					       var = 0;  
                                        };
					return var;
                                      };

  unsigned char covariance_blob[sz];
 
#if 0
  var_t var; 
  std::visit([](auto&& arg){std::cout << arg;}, var);

  do {
    for (int i=0; i<cols; ++i) {
      var_t val =   getval(i);
      std::visit([](auto&& arg){std::cout << arg << " ";}, val);
    }
    std::cout << std::endl;
  }
  while(res->next_row()); 
#endif
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BeamMoments::extract_dvec_xFunc( sqlite3_context* context, int n, sqlite3_value** values)
{
  int arg1 = sqlite3_value_int(values[0]);
  double* arg2 = (double*) sqlite3_value_blob(values[1]);
  sqlite3_result_double(context, arg2[arg1]); // return value
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BeamMoments::extract_cvec_xFunc( sqlite3_context* context, int n, sqlite3_value** values)
{
  int  arg1 = sqlite3_value_int(values[0]); // index into the vetor
  char arg2 = sqlite3_value_int(values[1]); // 'r' real : 'i' imag  
  std::complex<double>* arg3 = (std::complex<double>*) sqlite3_value_blob(values[2]);
  sqlite3_result_double(context, (toupper(arg2) == 'I' ? arg3[arg1].imag() : arg3[arg1].real()) ); // return value
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamMoments::extract_lf_xFunc( sqlite3_context* context, int n, sqlite3_value** values)
{  

  static const std::complex<double> I(0.0, 1.0);

  // extract lattice functions from eigenvectors
  // n = 4 
  
  int i     = sqlite3_value_int(values[0]); 
  int j     = sqlite3_value_int(values[1]);

  Vector_t<4,std::complex<double>> v1( (std::complex<double>*) sqlite3_value_blob(values[2]));
  Vector_t<4,std::complex<double>> v2( (std::complex<double>*) sqlite3_value_blob(values[3]));

  double b1x = (conj(v1[0])*v1[0]).real();

  double b1y = std::abs((conj(v1[2])*v1[2]).real()); 

  double b2x = std::abs( (conj(v2[0])*v2[0]).real() ); 

  double b2y =  (conj(v2[2])*v2[2]).real(); 

  double beta[2][2] = { b1x, b1y, b2x, b2y};
  sqlite3_result_double(context, beta[i][j]); // return value

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamMoments::extract_cov_xFunc( sqlite3_context* context, int n, sqlite3_value** values)
{
  // covariance matrix element 
  // n:           = no of arguments
  // values[0]:   = int i  (matrix index)
  // values[1]:   = int j  (matrix index)
  // values[3]    = double* (matrix data binary blob) 
  
  assert(n==3);

  int i     = sqlite3_value_int(values[0]);
  int j     = sqlite3_value_int(values[1]);
  double* arg3 = (double*) sqlite3_value_blob(values[2]);

  if (j>i) std::swap(i,j);
  int idx = 0;
  for (int k=0; k<=i; ++k) { idx += k; }
  double result = arg3[idx+j];
  
  sqlite3_result_double(context, result); // return value
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamMoments::extract_cor_xFunc( sqlite3_context* context, int n, sqlite3_value** values)
{
  // correlation matrix element 
  // n:           = no of arguments
  // values[0]:   = int i  (matrix index)
  // values[1]:   = int j  (matrix index)
  // values[3]    = double* (matrix data binary blob) 
  
  assert(n==3);

  int i     = sqlite3_value_int(values[0]);
  int j     = sqlite3_value_int(values[1]);
  double* arg3 = (double*) sqlite3_value_blob(values[2]);

  auto get_entry = [&arg3](int i,int j) { 
 
    if (j>i) std::swap(i,j);
    int idx = 0;
    for (int k=0; k<=i; ++k) { idx += k; } // i=3 ==> idx = 0 + 1 + 2 + 3 = 6  idx = i==0? 0 : i+(i+1)/2  
    double result = arg3[idx+j];
    return result;
  };

  double sii = sqrt(get_entry(i,i));
  double sjj = sqrt(get_entry(j,j));
  
  double result = ((sii < 1.0e-20) ||  (sjj < 1.0e-20) ) ? 0.0: get_entry(i,j)/(sii*sjj);
  sqlite3_result_double(context, result); // return value
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamMoments::initDBCustomFunctions( sqlite::connection& con)
{
  sqlite3* db = sqlite::private_accessor::get_handle(con);
  initDBCustomFunctions(db);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamMoments::initDBCustomFunctions( sqlite3* db)
{

  auto optimx_logger = spdlog::get("optimx_logger");

  // initialize sqlite custom function(s)
  void* pApp  = 0;

  
  int status = sqlite3_create_function(db, "extract_lf", 4, (SQLITE_UTF8 | SQLITE_DETERMINISTIC), db,  &BeamMoments::extract_lf_xFunc, NULL, NULL);  
  if (status != SQLITE_OK) { SPDLOG_LOGGER_ERROR(optimx_logger, fmt::format("BeamMoments::initDBCustomFunctions [extract_lf_xFunc]: SQLITE returns error code: {:d}", status));}; 

  auto    xFunc = &BeamMoments::extract_cov_xFunc; 
  status = sqlite3_create_function( db, "extract_cov", 3, (SQLITE_UTF8 | SQLITE_DETERMINISTIC), db, xFunc, NULL, NULL);  
  if (status != SQLITE_OK) { SPDLOG_LOGGER_ERROR(optimx_logger, fmt::format("BeamMoments::initDBCustomFunctions [extract_cov_xFunc]: SQLITE returns error code: {:d}", status));}; 

  xFunc = &BeamMoments::extract_cor_xFunc; 
  status = sqlite3_create_function( db, "extract_cor", 3, (SQLITE_UTF8 | SQLITE_DETERMINISTIC), db, xFunc, NULL, NULL);  
  if (status != SQLITE_OK) { SPDLOG_LOGGER_ERROR(optimx_logger, fmt::format("BeamMoments::initDBCustomFunctions [extract_cor_xFunc]: SQLITE returns error code: {:d}", status));}; 

  status = sqlite3_create_function(db,   "extract_dvec", 2, (SQLITE_UTF8 | SQLITE_DETERMINISTIC), db,  &BeamMoments::extract_dvec_xFunc, NULL, NULL);  
  if (status != SQLITE_OK) { SPDLOG_LOGGER_ERROR(optimx_logger, fmt::format("BeamMoments::initDBCustomFunctions [extract_dvec_xFunc]: SQLITE returns error code: {:d}", status));}; 

  status = sqlite3_create_function(db, "extract_cvec", 3, (SQLITE_UTF8 | SQLITE_DETERMINISTIC), db,  &BeamMoments::extract_cvec_xFunc, NULL, NULL);  
  if (status != SQLITE_OK) { SPDLOG_LOGGER_ERROR(optimx_logger, fmt::format("BeamMoments::initDBCustomFunctions [extract_cvec_xFunc]: SQLITE returns error code: {:d}", status));}; 

}
