//  =================================================================
//
//  FixedLimitsDialog.cpp
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

#include <FixedLimitsDialog.h>
#include <ui_fixed_limits_dialog.h>

FixedLimitsDialog::FixedLimitsDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::FixedLimitsDialog)
{
   ui_->setupUi(this);
   callback_ = [](){}; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FixedLimitsDialog::~FixedLimitsDialog()
{
  delete ui_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void FixedLimitsDialog::accept()
{
  data_.minimum       = ui_->techSpinBoxMin->value();
  data_.maximum       = ui_->techSpinBoxMax->value();
  data_.active        = ui_->checkBoxActive->isChecked();
  data_.zoom          = ui_->checkBoxZoom->isChecked();
  data_.autoscale     = ui_->checkBoxAutoScale->isChecked();
  
  callback_();
  
  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void FixedLimitsDialog::setCallback(std::function<void()> const& cb )
{
  callback_= cb;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FixedLimitsDialog::set()
{
  ui_->techSpinBoxMin->setEnabled(!data_.autoscale);
  ui_->techSpinBoxMin->setValue(data_.minimum);

  ui_->techSpinBoxMax->setEnabled(!data_.autoscale);
  ui_->techSpinBoxMax->setValue(data_.maximum);

  ui_->checkBoxActive->setChecked(data_.active);
  ui_->checkBoxActive->setEnabled(data_.active_box_enabled);
  ui_->checkBoxZoom->setChecked(data_.zoom);
  ui_->checkBoxAutoScale->setChecked(data_.autoscale);
}


