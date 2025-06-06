//  =================================================================
//
//  CompressDialog.h
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

#ifndef  COMPRESSDIALOG_H
#define  COMPRESSDIALOG_H

#include <QDialog>
#include <QString>

struct ComprsDlgStruct {

  int  offset; 

  bool cor;     // true : merge correctors
  bool sext;    // true : merge sextupoles
  bool sol;     // true : merge solenoids
  bool inst;    // true : merge intrumentation 
  bool apert;   // true : aperture elements
  bool wkfld;   // true : wakefield elements
  bool scatter; // true : scattering element 
  bool vrbls;   // true : variables

  QString filenamefrom;  
  QString filenameto;    
};

namespace Ui { class CompressDialog; } 

class CompressDialog: public QDialog {

Q_OBJECT

 public:

           CompressDialog( QWidget* parent);
  virtual ~CompressDialog();

  void set(); 

  ComprsDlgStruct data_;

public slots:

  void accept(); 
  void selectInputFile();
  void selectOutputFile();
      
 
 private:

  Ui::CompressDialog* ui_;

};

#endif //   COMPRESSDIALOG_H                 

