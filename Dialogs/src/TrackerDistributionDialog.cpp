//  =================================================================
//
//  TrackerDistributionDialog.cpp
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

#include <TrackerDistributionDialog.h>
#include <ui_tracker_distribution_dialog.h>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackerDistributionDialog::TrackerDistributionDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::TrackerDistributionDialog)
{
  ui_->setupUi(this); 
  ui_->spinBoxNpart->setMinimum(1);
  ui_->spinBoxNpart->setMaximum(10000000);
} 
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackerDistributionDialog::~TrackerDistributionDialog()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerDistributionDialog::accept()
{

  data_.Npart = ui_->spinBoxNpart->value();
  data_.Rmin  = ui_->doubleSpinBoxRmin->value();
  data_.Rmax  = ui_->doubleSpinBoxRmax->value();

  QDialog::accept();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerDistributionDialog::set()
{
  ui_->spinBoxNpart->setValue(data_.Npart);
  ui_->doubleSpinBoxRmin->setValue(data_.Rmin);
  ui_->doubleSpinBoxRmax->setValue(data_.Rmax);
}
