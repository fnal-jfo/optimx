//  =================================================================
//
//  SpaceChargeControlDialog.cpp
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

#include <SpaceChargeControlDialog.h>
#include <ui_space_charge_control_dialog.h>
#include <RootFinder.h>

SpaceChargeControlDialog::SpaceChargeControlDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::SpaceChargeControlDialog)
{
   ui_->setupUi(this);

   // enum Algorithm { powell_hybrid, generalized_newton, powell_hybrid_unscaled, newton };

   ui_->comboBoxAlgo->addItem("Powell   Hybrid",  uint(RootFinder::powell_hybrid) );
   ui_->comboBoxAlgo->addItem("Var Step Newton",  uint(RootFinder::generalized_newton) );

#ifndef USE_GSL   
   this->setEnabled(false);
#endif

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

SpaceChargeControlDialog::~SpaceChargeControlDialog()
{
  delete ui_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void SpaceChargeControlDialog::accept()
{
  data_.step         = ui_->doubleSpinBoxStepSize->value();
  // data_.relaxation   = ui_->doubleSpinBoxRelaxation->value();
  data_.maxiters     = ui_->spinBoxMaxIterations->value();
  data_.isteps       = ui_->spinBoxCurrentSteps->value();
  data_.eps          = ui_->techSpinBoxEps->value();
  data_.use4dblock   = ui_->checkBoxUse4DBlock->isChecked();
  data_.algo         = ui_->comboBoxAlgo->itemData( ui_->comboBoxAlgo->currentIndex()).toInt(); 
  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void SpaceChargeControlDialog::set()
{
  ui_->doubleSpinBoxStepSize->setValue(data_.step);
  //ui_->doubleSpinBoxRelaxation->setValue(data_.relaxation);
  ui_->spinBoxMaxIterations->setValue(data_.maxiters);
  ui_->spinBoxCurrentSteps->setValue(data_.isteps);
  ui_->techSpinBoxEps->setValue(data_.eps);
  ui_->checkBoxUse4DBlock->setChecked(data_.use4dblock);
  ui_->comboBoxAlgo->setCurrentIndex( ui_->comboBoxAlgo->findData(data_.algo) );
   QDialog::accept();
}



