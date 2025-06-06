//  =================================================================
//
//  ControlDialog.cpp
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

#include <ControlDialog.h>
#include <ui_control_dialog.h>

#include <QIntValidator>
#include <iostream>


ControlStruct::ControlStruct()
{
  CompAtExcitedOrb = false; 
  AutoBeta         = false; 
  AutoLattice      = false; 
  ClearPlot        = false;
  ClearText        = false; 
  RewriteBuf       = false; 
  PlotBoxes        = false;
  PlotApertures    = false; 
  PlotTotalSize    = false;
  ArrayLen         = 0;
  CouplThreshold   = 0.0;
  NStep            = 0;
  Accuracy         = 0.0;
  IsRingCh         = false;
  AccuracyL           = 0.0;
  use_fractional_tune = false;


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ControlStruct& ControlStruct::operator=( ControlStruct const& rhs )
{

  if (&rhs == this ) return *this;

  IsRingCh         = rhs.IsRingCh;
  CompAtExcitedOrb = rhs.CompAtExcitedOrb;
  AutoBeta         = rhs.AutoBeta; 
  AutoLattice      = rhs.AutoLattice; 
  ClearPlot        = rhs.ClearPlot;
  ClearText        = rhs.ClearText; 
  RewriteBuf       = rhs.RewriteBuf; 
  PlotBoxes        = rhs.PlotBoxes;
  PlotApertures    = rhs.PlotApertures; 
  PlotTotalSize    = rhs.PlotTotalSize;
  ArrayLen         = rhs.ArrayLen;
  CouplThreshold   = rhs.CouplThreshold;
  NStep            = rhs.NStep;
  Accuracy         = rhs.Accuracy;
  AccuracyL        = rhs.AccuracyL;
  use_fractional_tune = rhs.use_fractional_tune;

  return *this; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ControlDialog::ControlDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::ControlDialog() )
{
  ui_->setupUi(this);

  ui_->spinBoxPlotSamples->setMinimum(10);
  ui_->techSpinBoxCoupling->setMinimum(0.0);
  ui_->techSpinBoxAccuracy->setMinimum(1.0e-12);
  ui_->spinBoxNoSteps->setMinimum(1);

  // default values 

  data_.IsRingCh         = false;
  data_.CompAtExcitedOrb = false; 
  data_.AutoBeta         = true;
  data_.AutoLattice      = true;
  data_.ClearPlot        = true;
  data_.ClearText        = true;
  data_.RewriteBuf       = true;
  data_.PlotBoxes        = true;
  data_.PlotApertures    = true;
  data_.PlotTotalSize    = true;
  data_.ArrayLen         = 5000;
  data_.CouplThreshold   = 0.01;
  data_.NStep            = 10;
  data_.Accuracy         = 1.0e-7;
  data_.AccuracyL        = 1.0e-7;
  data_.use_fractional_tune    = false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ControlDialog::~ControlDialog()
{
  delete ui_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ControlDialog::accept()
{

  data_.IsRingCh         = !ui_->radioButtonLine->isChecked();
  data_.CompAtExcitedOrb = ui_->checkBoxUseClosedOrbit->isChecked();
  data_.AutoBeta         = ui_->checkBoxAutoPlot->isChecked();
  data_.AutoLattice      = ui_->checkBoxAutoPrint->isChecked();
  data_.ClearPlot        = ui_->checkBoxClearPlot->isChecked();
  data_.ClearText        = ui_->checkBoxClearText->isChecked();
  data_.RewriteBuf       = ui_->checkBoxRewrite->isChecked();
  data_.PlotBoxes        = ui_->checkBoxDrawLego->isChecked();
  data_.PlotApertures    = ui_->checkBoxDrawApertures->isChecked();
  data_.PlotTotalSize    = ui_->checkBoxUseTotalSize->isChecked();

  data_.ArrayLen         = ui_->spinBoxPlotSamples->value();
  data_.CouplThreshold   = ui_->techSpinBoxCoupling->value();
  data_.NStep            = ui_->spinBoxNoSteps->value();
  data_.Accuracy         = ui_->techSpinBoxAccuracy->value();
  
  QDialog::accept(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ControlDialog::set()
{
  ui_->radioButtonRing->setChecked(data_.IsRingCh); // R or L: ring or line  
  ui_->checkBoxUseClosedOrbit->setChecked( data_.CompAtExcitedOrb );
  ui_->checkBoxAutoPlot->setChecked(  data_.AutoBeta );
  ui_->checkBoxAutoPrint->setChecked(  data_.AutoLattice );
  ui_->checkBoxClearPlot->setChecked(  data_.ClearPlot );
  ui_->checkBoxClearText->setChecked( data_.ClearText );
  ui_->checkBoxRewrite->setChecked( data_.RewriteBuf );
  ui_->checkBoxDrawLego->setChecked( data_.PlotBoxes );
  ui_->checkBoxDrawApertures->setChecked( data_.PlotApertures );
  ui_->checkBoxUseTotalSize->setChecked(data_.PlotTotalSize );

  ui_->spinBoxPlotSamples->setValue( data_.ArrayLen );
  ui_->techSpinBoxCoupling->setValue( data_.CouplThreshold );
  ui_->spinBoxNoSteps->setValue(data_.NStep );
  ui_->techSpinBoxAccuracy->setValue( data_.Accuracy );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ControlDialog::showFullsize( bool set)
{
  // this slot could be used to force a redraw all size plots.
  // std::cout <<  "ControlDialog::showFullsize " << set << std::endl;
}
