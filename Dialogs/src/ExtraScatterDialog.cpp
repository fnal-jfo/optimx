//  =================================================================
//
//  ExtraScatterDialog.cpp
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
#include <ExtraScatterDialog.h>
#include <ui_extra_scatter_dialog.h> 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ExtraScatterDialog::ExtraScatterDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::ExtraScatterDialog)
{
   ui_->setupUi(this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ExtraScatterDialog::~ExtraScatterDialog()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ExtraScatterDialog::accept()
{
  data_.dX  = ui_->doubleSpinBoxdX->value();     
  data_.dY  = ui_->doubleSpinBoxdY->value();     
  data_.dS  = ui_->doubleSpinBoxdS->value();     
  data_.dPX = ui_->doubleSpinBoxdPX->value();     
  data_.dPY = ui_->doubleSpinBoxdPY->value();     
  data_.dPS = ui_->doubleSpinBoxdPP->value();     

  data_.dSigmaX  =  ui_->doubleSpinBoxSigmaX->value();     
  data_.dSigmaY  =  ui_->doubleSpinBoxSigmaY->value();     
  data_.dSigmaS  =  ui_->doubleSpinBoxSigmaS->value();     
  data_.dSigmaPX =  ui_->doubleSpinBoxSigmaPX->value();     
  data_.dSigmaPY =  ui_->doubleSpinBoxSigmaPY->value();     
  data_.dSigmaPS =  ui_->doubleSpinBoxSigmadPP->value();     
 
  QDialog::accept();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ExtraScatterDialog::set()
{
  
  ui_->doubleSpinBoxdX->setValue( data_.dX);     
  ui_->doubleSpinBoxdY->setValue( data_.dY);     
  ui_->doubleSpinBoxdS->setValue( data_.dS);     
  ui_->doubleSpinBoxdPX->setValue( data_.dPX);     
  ui_->doubleSpinBoxdPY->setValue( data_.dPX);     
  ui_->doubleSpinBoxdPP->setValue( data_.dPS);     

  ui_->doubleSpinBoxSigmaX->setValue( data_.dSigmaX);     
  ui_->doubleSpinBoxSigmaY->setValue( data_.dSigmaY);     
  ui_->doubleSpinBoxSigmaS->setValue( data_.dSigmaS);     
  ui_->doubleSpinBoxSigmaPX->setValue( data_.dSigmaPX);     
  ui_->doubleSpinBoxSigmaPY->setValue( data_.dSigmaPY);     
  ui_->doubleSpinBoxSigmadPP->setValue( data_.dSigmaPS);     

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

