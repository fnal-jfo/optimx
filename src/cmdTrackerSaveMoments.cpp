//  =================================================================
//
//  cmdTrackerSaveMoments.cpp
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
#include <sstream>
#include <fstream>
#include <string>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include <Constants.h>
#include <BeamMoments.h>
#include <Globals.h>
#include <Element.h>
#include <RMatrix.h>
#include <OptimMainWindow.h>
#include <MomentsSelectionDialog.h>
#include <OptimMessages.h>
#include <OptimTrackerNew.h>
#include <TrackParam.h>
#include <QMdiArea>
#include <QFileDialog>


#undef emit
#include <sqlite/execute.hpp>
#include <sqlite/private/private_accessor.hpp>
#include <sqlite/query.hpp>
//#include <sqlite3.h>

using Constants::PI;
using Constants::C_DERV1;

void OptimTrackerNew::cmdTrackerSaveMoments()
{

#if 0
  static MomentsSelectionDialog* dialog = 0;

  if (!dialog) {
    dialog = new MomentsSelectionDialog();
  }  

  if ( dialog->exec() == QDialog::Rejected ) return;


  auto selection = dialog->data_; // selected moments;
#endif  

  std::fstream fs;

  QString FileName = QFileDialog::getSaveFileName ( 0, "Save Beam Statistical Data", ".");
  if (FileName == "" ) return; 
  
  fs.open( FileName.toUtf8().data(),  std::ios_base::out | std::ios_base::trunc );
  if (fs.fail()) {
     OptimMessageBox::warning(this, "Error opening file - ", FileName.toUtf8().data(), QMessageBox::Ok);  
    return;
  }    

  std::string labels[6][6];
  for (int i=0; i<6; ++i) {
     for (int j=0; j<=i; ++j) {
       labels[i][j] = labels[j][i] = fmt::format("S_{:1d}{:1d}",i,j);
     }
  }

  fmt::print(fs, "# {:>6s} {:>4s} {:>12s} ", "turn", "elm", "s");
  for (int i=0; i<6; ++i) {
    for (int j=0; j<=i; ++j) {
      std::string lbl = fmt::format("Cov_{:1d}{:1d}",i,j);
      fmt::print(fs, "{:>12s} ",lbl);   
    }
  }
  fmt::print(fs, "\n");   

  std::string sql = fmt::format("SELECT turn, eidx, pathlen, covariance FROM Moments ORDER BY turn,eidx;");

  sqlite::connection& con = *Globals::preferences().con;
  sqlite::query q( con, sql);
  
  auto result = q.emit_result();

  std::vector<unsigned char> vec;
  
  while(result->next_row()) {

      result->get_binary(3,vec);  
      int sz = vec.size()/sizeof(double);

      int  turn = result->get_int(0);  
      int  eidx = result->get_int(1);
      double s  = result->get_double(2);
      
      
      fmt::print(fs, "{:8d} {:4d} {:12g} ", turn, eidx, s);

      double* covariance =  (double*) &vec[0]; 
      for(int i=0; i<sz; ++i) { 
        fmt::print(fs, "{:12.5g} ",covariance[i]); 
      }
      fmt::print(fs, "\n");
   }
}
