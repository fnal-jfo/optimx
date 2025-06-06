//-------------------------------------------------------------------
//
// OptiMX   (c) FRA/Fermilab 
// Please refer to OptiM.Copyright.txt for copyright and license 
// information. All other rights reserved.
//
// Vector.cc
//
// ------------------------------------------------------------------

#include <Vector.h>
#include <complex>

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<> 
std::complex<double> convertTo<std::complex<double>,double>( double value)
{  
  std::complex<double> retvalue = std::complex<double>(value, 0.0);
  return  retvalue;
}
