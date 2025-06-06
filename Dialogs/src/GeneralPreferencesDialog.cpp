//  =================================================================
//
//  GeneralPreferencesDialog.cpp
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
#include <GeneralPreferencesDialog.h>
#include <Globals.h>
#include <ui_general_preferences_dialog.h>
#include <QDir>
#include <Element.h> /// does not belong here !!!
#include <cfloat>
#include <climits>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

GeneralPreferencesDialog::GeneralPreferencesDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::GeneralPreferencesDialog() )
{
  ui_->setupUi(this);
  ui_->userSeedCheckBox->setChecked(appstate.userseed);
  ui_->seedSpinBox->setEnabled(appstate.userseed);
  ui_->seedSpinBox->setRange(INT_MIN, INT_MAX);
  //std::cerr << "appstate.seed = " << appstate.seed << std::endl;
  ui_->seedSpinBox->setValue(appstate.seed);
  connect(ui_->userSeedCheckBox, &QCheckBox::toggled, ui_->seedSpinBox, &QSpinBox::setEnabled);
 }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

GeneralPreferencesDialog::~GeneralPreferencesDialog()
{
  delete ui_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GeneralPreferencesDialog::accept()
{
  data_.ignore_autorepeat       = ui_->checkBoxIgnoreAutoRepeat->isChecked();
  data_.quad_fringe_off         = ui_->checkBoxQuadFringeOff->isChecked();
  data_.neg_phase_adv_threshold = ui_->doubleSpinBoxPhaseThreshold->value();
  data_.parallel_tracking       = ui_->checkBoxParallelTracking->isChecked();
  
  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GeneralPreferencesDialog::set()
{
  ui_->checkBoxIgnoreAutoRepeat->setChecked( data_.ignore_autorepeat);
  ui_->checkBoxQuadFringeOff->setChecked( data_.quad_fringe_off );
  ui_->checkBoxParallelTracking->setChecked( data_.parallel_tracking );

  // the following prevents  setValue() from rounding its argument.
  ui_->doubleSpinBoxPhaseThreshold->setDecimals( DBL_MAX_10_EXP + DBL_DIG );

  ui_->doubleSpinBoxPhaseThreshold->setValue( data_.neg_phase_adv_threshold);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

