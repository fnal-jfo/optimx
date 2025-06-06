//  =================================================================
//
//  TrackerDistributionDialogNew.cpp
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

#include <TrackerDistributionDialogNew.h>
#include <ui_tracker_distribution_dialog_new.h>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackerDistributionDialogNew::TrackerDistributionDialogNew(QWidget* parent)
  : QDialog(parent), ui_(new Ui::TrackerDistributionDialogNew)
{
  ui_->setupUi(this); 
  ui_->spinBoxNpart->setMinimum(1);
  ui_->spinBoxNpart->setMaximum(10000000);
  ui_->deltaFunctionCheckBox->setChecked(false);
  data_.deltafunction = false;
} 
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackerDistributionDialogNew::~TrackerDistributionDialogNew()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerDistributionDialogNew::accept()
{

  data_.Npart = ui_->spinBoxNpart->value();

  data_.bx1  = ui_->doubleSpinBoxBx1->value();
  data_.ax1  = ui_->doubleSpinBoxAx1->value();
  data_.bx2  = ui_->doubleSpinBoxBx2->value();
  data_.ax2  = ui_->doubleSpinBoxAx2->value();

  data_.by1  = ui_->doubleSpinBoxBy1->value();
  data_.ay1  = ui_->doubleSpinBoxAy1->value();
  data_.by2  = ui_->doubleSpinBoxBy2->value();
  data_.ay2  = ui_->doubleSpinBoxAy2->value();

  data_.eta1   = ui_->doubleSpinBoxEta1->value();
  data_.eta1p  = ui_->doubleSpinBoxEta1p->value();
  data_.eta2   = ui_->doubleSpinBoxEta2->value();
  data_.eta2p  = ui_->doubleSpinBoxEta2p->value();

  data_.u       = ui_->doubleSpinBoxU->value();
  data_.sigma_s = ui_->doubleSpinBoxSigma_s->value();
  data_.sigma_p = ui_->doubleSpinBoxSigma_p->value();
  data_.alpha_l = ui_->doubleSpinBoxAlpha_L->value();

  data_.eps1   = ui_->doubleSpinBoxEps1->value();
  data_.eps2   = ui_->doubleSpinBoxEps2->value();


  data_.Rmin  = ui_->doubleSpinBoxRmin->value();
  data_.Rmax  = ui_->doubleSpinBoxRmax->value();

  data_.deltafunction  = ui_->deltaFunctionCheckBox->isChecked();

  QDialog::accept();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerDistributionDialogNew::set()
{
  ui_->spinBoxNpart->setValue(data_.Npart);

  ui_->doubleSpinBoxBx1->setValue(data_.bx1);
  ui_->doubleSpinBoxAx1->setValue(data_.ax1);
  ui_->doubleSpinBoxBx2->setValue(data_.bx2);
  ui_->doubleSpinBoxAx2->setValue(data_.ax2);

  ui_->doubleSpinBoxBy1->setValue(data_.by1);
  ui_->doubleSpinBoxAy1->setValue(data_.ay1);
  ui_->doubleSpinBoxBy2->setValue(data_.by2);
  ui_->doubleSpinBoxAy2->setValue(data_.ay2);

  ui_->doubleSpinBoxEta1->setValue(data_.eta1);
  ui_->doubleSpinBoxEta1p->setValue(data_.eta1p);
  ui_->doubleSpinBoxEta2->setValue(data_.eta2);
  ui_->doubleSpinBoxEta2p->setValue(data_.eta2p);

  ui_->doubleSpinBoxU->setValue(data_.u);
  ui_->doubleSpinBoxSigma_s->setValue(data_.sigma_s);
  ui_->doubleSpinBoxSigma_p->setValue(data_.sigma_p);
  ui_->doubleSpinBoxAlpha_L->setValue(data_.alpha_l);

  ui_->doubleSpinBoxEps1->setValue(data_.eps1);
  ui_->doubleSpinBoxEps2->setValue(data_.eps2);

  ui_->doubleSpinBoxRmin->setValue(data_.Rmin);
  ui_->doubleSpinBoxRmax->setValue(data_.Rmax);
  ui_->deltaFunctionCheckBox->setChecked(data_.deltafunction);
}
