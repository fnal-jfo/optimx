//  =================================================================
//
//  StepsDialog.cpp
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
#include <StepsDialog.h>
#include <ui_steps_dialog.h> 

StepsDialog::StepsDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::StepsDialog)
{
 
   ui_->setupUi(this);
#if 0
   ui_->doubleSpinBoxdL->setDecimals(5);
   ui_->doubleSpinBoxdB->setDecimals(5);
   ui_->doubleSpinBoxdG->setDecimals(5);
   ui_->doubleSpinBoxdS->setDecimals(5);
   ui_->doubleSpinBoxVar->setDecimals(5);
   ui_->doubleSpinBoxVolts->setDecimals(5);
   ui_->doubleSpinBoxPhase->setDecimals(5);

   ui_->doubleSpinBoxdL->setRange(-10,10);
   ui_->doubleSpinBoxdB->setRange(-10,10);
   ui_->doubleSpinBoxdG->setRange(-10,10);
   ui_->doubleSpinBoxdS->setRange(-10,10);
   ui_->doubleSpinBoxVar->setRange(-10,10);
   ui_->doubleSpinBoxVolts->setRange(-10,10);
   ui_->doubleSpinBoxPhase->setRange(-10,10);
#endif

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

StepsDialog::~StepsDialog()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void StepsDialog::accept()
{

  data_.dL     = ui_->doubleSpinBoxdL->value();
  data_.dOfsx  = ui_->doubleSpinBoxdOfsx->value();
  data_.dOfsy  = ui_->doubleSpinBoxdOfsy->value();
  data_.dB     = ui_->doubleSpinBoxdB->value();
  data_.dG     = ui_->doubleSpinBoxdG->value();
  data_.dS     = ui_->doubleSpinBoxdS->value();
  data_.dV     = ui_->doubleSpinBoxVar->value();
  data_.dA     = ui_->doubleSpinBoxVolts->value();
  data_.dPhase = ui_->doubleSpinBoxPhase->value();
 
  QDialog::accept();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void StepsDialog::set()
{
  ui_->doubleSpinBoxdL->setValue( data_.dL );
  ui_->doubleSpinBoxdOfsx->setValue( data_.dOfsx);
  ui_->doubleSpinBoxdOfsy->setValue( data_.dOfsy);
  ui_->doubleSpinBoxdB->setValue( data_.dB );
  ui_->doubleSpinBoxdG->setValue( data_.dG );
  ui_->doubleSpinBoxdS->setValue( data_.dS );
  ui_->doubleSpinBoxVar->setValue( data_.dV );
  ui_->doubleSpinBoxVolts->setValue( data_.dA );
  ui_->doubleSpinBoxPhase->setValue( data_.dPhase);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

