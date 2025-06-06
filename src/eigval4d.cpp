//  =================================================================
//
//  eigval4d.cpp
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

#include <RMatrix.h>
#include <Utility.h>
#include <cmath>
#include <cstring>
#include <complex>
#include <utility>
#include <limits>
#include <type_traits>

#if 0
namespace Utility{ 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template void balanc(double(**a),    int n);
template void balanc(double(*a)[2],  int n);
template void balanc(double(*a)[3],  int n);
template void balanc(double(*a)[4],  int n);
template void balanc(double(*a)[5],  int n);
template void balanc(double(*a)[6],  int n);
  
template void balanc(std::complex<double>   (**a),  int n);
template void balanc(std::complex<double> (*a)[2],  int n);
template void balanc(std::complex<double> (*a)[3],  int n);
template void balanc(std::complex<double> (*a)[4],  int n);
template void balanc(std::complex<double> (*a)[5],  int n);
template void balanc(std::complex<double> (*a)[6],  int n);

//---------------------------------------------------------------------------

template void hqr(double   (**a), int n, std::complex<double> wri[], char* err);
template void hqr(double (*a)[2], int n, std::complex<double> wri[], char* err);
template void hqr(double (*a)[3], int n, std::complex<double> wri[], char* err);
template void hqr(double (*a)[4], int n, std::complex<double> wri[], char* err);
template void hqr(double (*a)[5], int n, std::complex<double> wri[], char* err);
template void hqr(double (*a)[6], int n, std::complex<double> wri[], char* err);

//template void hqr(std::complex<double>   (**a), int n,  std::complex<double> wri[], char* err);
//template void hqr(std::complex<double> (*a)[2], int n,  std::complex<double> wri[], char* err);
//template void hqr(std::complex<double> (*a)[3], int n, std::complex<double> wri[], char* err);
//template void hqr(std::complex<double> (*a)[4], int n, std::complex<double> wri[], char* err);
//template void hqr(std::complex<double> (*a)[5], int n, std::complex<double> wri[], char* err);
//template void hqr(std::complex<double> (*a)[6], int n, std::complex<double> wri[], char* err);

//---------------------------------------------------------------------------

template void elmhes(double   (**a), int n);
template void elmhes(double (*a)[2], int n);
template void elmhes(double (*a)[3], int n);
template void elmhes(double (*a)[4], int n);
template void elmhes(double (*a)[5], int n);
template void elmhes(double (*a)[6], int n);

template void elmhes(std::complex<double>   (**a), int n);
template void elmhes(std::complex<double> (*a)[2], int n);
template void elmhes(std::complex<double> (*a)[3], int n);
template void elmhes(std::complex<double> (*a)[4], int n);
template void elmhes(std::complex<double> (*a)[5], int n);
template void elmhes(std::complex<double> (*a)[6], int n);

//---------------------------------------------------------------------------

} // namespace Utility
#endif
