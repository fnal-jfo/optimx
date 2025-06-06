//  =================================================================
//
//  TrackPlotDistributionDialog.cpp
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

#include <TrackPlotDistributionDialog.h>
#include <ui_track_plot_distribution_dialog.h> 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackPlotDistributionDialog::TrackPlotDistributionDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::TrackPlotDistributionDialog)
{
 
  ui_->setupUi(this);
  ui_->spinBoxNbins->setRange(1, 1000); 
  ui_->doubleSpinBoxInitial->setRange(-10000.0, 10000.0); 
  ui_->doubleSpinBoxFinal->setRange(-10000.0, 10000.0); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackPlotDistributionDialog::~TrackPlotDistributionDialog()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackPlotDistributionDialog::accept()
{
  data_.Nbin = ui_->spinBoxNbins->value(); 
  data_.Xin  = ui_->doubleSpinBoxInitial->value(); 
  data_.Xfin = ui_->doubleSpinBoxFinal->value(); 
  data_.X    = ui_->checkBoxX->isChecked(); 
  data_.Y    = ui_->checkBoxY->isChecked();
  data_.S    = ui_->checkBoxS->isChecked();
  data_.Px   = ui_->checkBoxPx->isChecked();
  data_.Py   = ui_->checkBoxPy->isChecked();
  data_.Ps   = ui_->checkBoxPs->isChecked();
  QDialog::accept();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackPlotDistributionDialog::set()
{
  ui_->spinBoxNbins->setValue( data_.Nbin ); 
  ui_->doubleSpinBoxInitial->setValue(data_.Xin ); 
  ui_->doubleSpinBoxFinal->setValue(data_.Xfin ); 
  ui_->checkBoxX->setChecked(data_.X); 
  ui_->checkBoxY->setChecked(data_.Y);
  ui_->checkBoxS->setChecked(data_.S);
  ui_->checkBoxPx->setChecked(data_.Px);
  ui_->checkBoxPy->setChecked(data_.Py);
  ui_->checkBoxPs->setChecked(data_.Ps);
}

