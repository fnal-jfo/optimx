//  =================================================================
//
//  ControlDialog.h
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

#ifndef CONTROLDIALOG_H
#define CONTROLDIALOG_H

#include <QDialog>

namespace Ui { class ControlDialog;}


struct ControlStruct {

  bool   IsRingCh;
  bool   CompAtExcitedOrb;
  bool   AutoBeta; 
  bool   AutoLattice; 
  bool   ClearPlot;
  bool   ClearText; 
  bool   RewriteBuf; 
  bool   PlotBoxes;
  bool   PlotApertures; 
  bool   PlotTotalSize;
  int    ArrayLen;
  double CouplThreshold;
  int    NStep;
  double Accuracy;
  double AccuracyL;
  bool   use_fractional_tune;

  ControlStruct( ControlStruct const& o); 
  ControlStruct(); 
  ControlStruct& operator=( ControlStruct const& o);
};

class ControlDialog: public QDialog {

Q_OBJECT

 public:

 ControlDialog(QWidget* parent=0);
 ~ControlDialog();
  
 public slots:

   void set();
   void accept();

 private slots:

   void showFullsize( bool set);
 
 private:

   Ui::ControlDialog* ui_;

 public:
   

 ControlStruct data_;

};
 


#endif //CONTROLDIALOG_H
