//  =================================================================
//
//  TuneDiagramDialog.cpp
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

#include <TuneDiagramDialog.h>
#include <ui_tune_diagram_dialog.h>

#include <QIntValidator>
#include <QDoubleValidator>
#include <iostream>



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagramDialog::TuneDiagramDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::TuneDiagramDialog() )
{
  ui_->setupUi(this);
 

  ui_->doubleSpinBoxQxMin->setRange(0.0,200.0);
  ui_->doubleSpinBoxQxMax->setRange(0.0,200.0);
  ui_->doubleSpinBoxQyMin->setRange(0.0,200.0);
  ui_->doubleSpinBoxQyMax->setRange(0.0,200.0);

  ui_->doubleSpinBoxQxMin->setDecimals(1);
  ui_->doubleSpinBoxQxMax->setDecimals(1);
  ui_->doubleSpinBoxQyMin->setDecimals(1);
  ui_->doubleSpinBoxQyMax->setDecimals(1);

  ui_->doubleSpinBoxQx->setRange(0.0,200.0); 
  ui_->doubleSpinBoxQy->setRange(0.0,200.0);

  ui_->doubleSpinBoxQx->setDecimals(3); 
  ui_->doubleSpinBoxQy->setDecimals(3);

  ui_->spinBoxMinOrder->setRange(0,40);
  ui_->spinBoxMaxOrder->setRange(0,40);

  ui_->spinBoxQxIntervals->setRange(2,10);
  ui_->spinBoxQyIntervals->setRange(2,10);

  // default values 

  data_.qxmin              = 0.0;
  data_.qxmax              = 1.0;
  data_.qymin              = 0.0; 
  data_.qymax              = 1.0;
  data_.minorder           = 0; 
  data_.maxorder           = 5;
  data_.sumresonances      = true; 
  data_.couplingresonances = true; 
  data_.captions           = true;
  data_.qxintervals        = 10; 
  data_.qyintervals        = 10; 
  data_.qx                 = 0.0;
  data_.qy                 = 0.0;

  set(); 
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagramDialog::~TuneDiagramDialog()
{
  delete ui_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagramDialog::accept()
{
 
  data_.qxmin              =  ui_->doubleSpinBoxQxMin->value(); 
  data_.qxmax              =  ui_->doubleSpinBoxQxMax->value();
  data_.qymin              =  ui_->doubleSpinBoxQyMin->value(); 
  data_.qymax              =  ui_->doubleSpinBoxQyMax->value(); 
  data_.minorder           =  ui_->spinBoxMinOrder->value(); 
  data_.maxorder           =  ui_->spinBoxMaxOrder->value(); 
  data_.sumresonances      =  ui_->checkBoxSumResonances->isChecked(); 
  data_.couplingresonances =  ui_->checkBoxCouplingResonances->isChecked();
  data_.captions           =  ui_->checkBoxCaptions->isChecked();
  data_.qxintervals        =  ui_->spinBoxQxIntervals->value();
  data_.qyintervals        =  ui_->spinBoxQyIntervals->value();
  data_.qx                 =  ui_->doubleSpinBoxQx->value();
  data_.qy                 =  ui_->doubleSpinBoxQy->value();

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagramDialog::set()
{

  ui_->doubleSpinBoxQxMin->setValue(data_.qxmin);
  ui_->doubleSpinBoxQxMax->setValue(data_.qxmax);
  ui_->doubleSpinBoxQyMin->setValue(data_.qymin);
  ui_->doubleSpinBoxQyMax->setValue(data_.qymax);

  ui_->spinBoxMinOrder->setValue(data_.minorder);
  ui_->spinBoxMaxOrder->setValue(data_.maxorder);

  ui_->checkBoxSumResonances->setChecked(data_.sumresonances);
  ui_->checkBoxCouplingResonances->setChecked(data_.couplingresonances);
  ui_->checkBoxCaptions->setChecked(data_.captions);

  ui_->spinBoxQxIntervals->setValue(data_.qxintervals); // grid intervals
  ui_->spinBoxQyIntervals->setValue(data_.qyintervals);

  ui_->doubleSpinBoxQx->setValue(data_.qx);
  ui_->doubleSpinBoxQy->setValue(data_.qy);

}
