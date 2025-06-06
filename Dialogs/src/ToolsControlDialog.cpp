//  =================================================================
//
//  ToolsControlDialog.cpp
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
#include <ToolsControlDialog.h>
#include <ui_tools_control_dialog.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ToolsControlDialog::ToolsControlDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::ToolsControlDialog() )
{
  ui_->setupUi(this);
  ui_->spinBoxMaxIterations->setRange(1,10000);
  connect(ui_->radioButtoncm,  SIGNAL(toggled(bool)), this, SLOT(enableRFFrequency(bool))); 
  connect(ui_->radioButtonns,  SIGNAL(toggled(bool)), this, SLOT(enableRFFrequency(bool))); 
  connect(ui_->radioButtondeg, SIGNAL(toggled(bool)), this, SLOT(enableRFFrequency(bool))); 
  connect(ui_->radioButtonrd,  SIGNAL(toggled(bool)), this, SLOT(enableRFFrequency(bool))); 
  connect(ui_->radioButtonLinearClosure,   SIGNAL(toggled(bool)), this, SLOT(enableClosureParameters(bool))); 
  connect(ui_->radioButtonGeneralClosure,  SIGNAL(toggled(bool)), this, SLOT(enableClosureParameters(bool))); 
  connect(ui_->radioButtonNormalPlot,                    SIGNAL(toggled(bool)), this, SLOT(enableLongPosition(bool))); 
  connect(ui_->radioButtonPlotNormalizedDisplacement,    SIGNAL(toggled(bool)), this, SLOT(enableLongPosition(bool))); 
  connect(ui_->radioButtonPlotDisplacementAndBoundaries, SIGNAL(toggled(bool)), this, SLOT(enableLongPosition(bool))); 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ToolsControlDialog::~ToolsControlDialog()
{
  delete ui_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ToolsControlDialog::enableLongPosition(bool ischecked)
{
    QObject* o = sender();
    if ( o == ui_->radioButtonNormalPlot) {
      if ( ischecked ) {
	ui_->groupBoxLongitudinal->setEnabled(true);
	return;
      }
    }      
    else {
      if ( ischecked ) {
	ui_->groupBoxLongitudinal->setEnabled(false);
	return;
      }
    };     
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ToolsControlDialog::enableClosureParameters(bool ischecked)
{
    QObject* o = sender();
    if ( o == ui_->radioButtonLinearClosure) {
      if ( ischecked ) {
	ui_->groupBoxGeneralClosure->setEnabled(false);
	return;
      }
    };      

    if ( o == ui_->radioButtonGeneralClosure) {
      if ( ischecked ) {
	ui_->groupBoxGeneralClosure->setEnabled(true);
	return;
      }
    };     
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ToolsControlDialog::enableRFFrequency(bool ischecked)
{
    QObject* o = sender();
    if ( o == ui_->radioButtoncm)  { if ( ischecked ) {ui_->doubleSpinBoxFrequency->setEnabled(false); return;} }       
    if ( o == ui_->radioButtonns)  { if ( ischecked ) {ui_->doubleSpinBoxFrequency->setEnabled(false); return;} }       
    if ( o == ui_->radioButtondeg) { if ( ischecked ) {ui_->doubleSpinBoxFrequency->setEnabled(true);  return;} }       
    if ( o == ui_->radioButtonrd)  { if ( ischecked ) {ui_->doubleSpinBoxFrequency->setEnabled(true);  return;} }       
    // if we get here, this is an error ...
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	  
void ToolsControlDialog::accept()
{

  data_.LinClosure        = ui_->radioButtonLinearClosure->isChecked();
  data_.FullClosure       = ui_->radioButtonGeneralClosure->isChecked();
  data_.ConvParam         = ui_->techSpinBoxConvergenceParameter->value();
  data_.Nmax              = ui_->spinBoxMaxIterations->value();
  data_.ConvErr           = ui_->techSpinBoxConvergenceAccuracy->value();
  data_.Closure4D         = ui_->radioButton4D->isChecked();
  data_.NormalPlot        = ui_->radioButtonNormalPlot->isChecked();
  data_.NormBySigma       = ui_->radioButtonPlotNormalizedDisplacement->isChecked();
  data_.TogetherWithEnvel = ui_->radioButtonPlotDisplacementAndBoundaries->isChecked();
 
  data_.longposcm         = ui_->radioButtoncm->isChecked();
  data_.longposns         = ui_->radioButtonns->isChecked();
  data_.longposdeg        = ui_->radioButtondeg->isChecked();
  data_.longposrd         = ui_->radioButtonrd->isChecked();
  data_.rffrequency       = ui_->doubleSpinBoxFrequency->value();

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ToolsControlDialog::set()
{
  ui_->radioButtonLinearClosure->setChecked(data_.LinClosure);
  ui_->radioButtonGeneralClosure->setChecked( data_.FullClosure);
  ui_->techSpinBoxConvergenceParameter->setValue(data_.ConvParam);
  ui_->spinBoxMaxIterations->setValue(data_.Nmax);
  ui_->techSpinBoxConvergenceAccuracy->setValue(data_.ConvErr);
  ui_->radioButton4D->setChecked(data_.Closure4D);
  ui_->radioButtonNormalPlot->setChecked(data_.NormalPlot);
  ui_->radioButtonPlotNormalizedDisplacement->setChecked( data_.NormBySigma);
  ui_->radioButtonPlotDisplacementAndBoundaries->setChecked(data_.TogetherWithEnvel);
  ui_->radioButtoncm->setChecked(data_.longposcm);
  ui_->radioButtonns->setChecked(data_.longposns);
  ui_->radioButtondeg->setChecked(data_.longposdeg);
  ui_->radioButtonrd->setChecked(data_.longposrd);
  ui_->doubleSpinBoxFrequency->setValue( data_.rffrequency );


}
