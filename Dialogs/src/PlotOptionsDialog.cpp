//  =================================================================
//
//  PlotOptionsDialog.cpp
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
#include <PlotOptionsDialog.h>
#include <functional>
#include <ui_plot_options_dialog.h>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotOptionsDialog::PlotOptionsDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::PlotOptionsDialog() )
{
  ui_->setupUi(this);

  ui_->doubleSpinBoxMinBottom->setRange(-100000,100000);
  ui_->doubleSpinBoxMaxBottom->setRange(-100000,100000); 

  ui_->doubleSpinBoxMinLeft->setRange(-100000,100000); 
  ui_->doubleSpinBoxMaxLeft->setRange(-100000,100000);

  ui_->doubleSpinBoxMinRight->setRange(-100000,100000);
  ui_->doubleSpinBoxMaxRight->setRange(-100000,100000);
  setWindowModality(Qt::NonModal);

  connect(ui_->radioButtonFixedRight, SIGNAL(toggled(bool)), ui_->doubleSpinBoxMaxRight, SLOT(setEnabled(bool)));  
  connect(ui_->radioButtonFixedRight, SIGNAL(toggled(bool)), ui_->doubleSpinBoxMinRight, SLOT(setEnabled(bool)));  
  connect(ui_->radioButtonFixedLeft,  SIGNAL(toggled(bool)), ui_->doubleSpinBoxMaxLeft, SLOT(setEnabled(bool)));  
  connect(ui_->radioButtonFixedLeft,  SIGNAL(toggled(bool)), ui_->doubleSpinBoxMinLeft, SLOT(setEnabled(bool)));  

  connect(ui_->checkBoxRightActive,  &QCheckBox::stateChanged, this,
	  [this](int state) {switch (state) {
	    case Qt::Unchecked:
	      ui_->checkBoxZoomYRight->setDisabled(true);
	      break;
	    case Qt::Checked:
	      ui_->checkBoxZoomYRight->setEnabled(true);
	      ui_->checkBoxLeftActive->setCheckState(Qt::Unchecked);
	      ui_->checkBoxZoomYLeft->setEnabled(false);
	    }
	  });  

  connect(ui_->checkBoxLeftActive,   &QCheckBox::stateChanged, this,  
	  [this](int state) {switch (state) {
	    case Qt::Unchecked:
	      ui_->checkBoxZoomYLeft->setDisabled(true);
	      break;
	    case Qt::Checked:
	      ui_->checkBoxZoomYLeft->setEnabled(true);
	      ui_->checkBoxRightActive->setCheckState(Qt::Unchecked);
	      ui_->checkBoxZoomYRight->setEnabled(false);
	    }
	  });  
  
  
  callback_ = [](){};

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotOptionsDialog::~PlotOptionsDialog()
{
  delete ui_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotOptionsDialog::setCallback( std::function<void()> const& cb)
{
  callback_ = cb;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotOptionsDialog::enableLegoOptions( bool flag)
{
  ui_->groupBoxLego->setEnabled(flag);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotOptionsDialog::accept()
{
  data_.auto_hor   = ui_->radioButtonAutoBottom->isChecked();
  data_.min_hor    = ui_->doubleSpinBoxMinBottom->value();
  data_.max_hor    = ui_->doubleSpinBoxMaxBottom->value();

  data_.auto_left  = ui_->radioButtonAutoLeft->isChecked();
  data_.min_left   = ui_->doubleSpinBoxMinLeft->value();
  data_.max_left   = ui_->doubleSpinBoxMaxLeft->value();

  data_.auto_right = ui_->radioButtonAutoRight->isChecked();
  data_.min_right  = ui_->doubleSpinBoxMinRight->value();
  data_.max_right  = ui_->doubleSpinBoxMaxRight->value();

  data_.hide_lego  = ui_->radioButtonLegoHide->isChecked();
  data_.bottom_lego= ui_->radioButtonLegoBottom->isChecked();
  data_.top_lego   = ui_->radioButtonLegoTop->isChecked();

  data_.title            = QwtText(ui_->lineEditTitle->text());
  data_.axis_title_hor   = QwtText(ui_->lineEditBottomAxisLabel->text());
  data_.axis_title_left  = QwtText(ui_->lineEditLeftAxisLabel->text());
  data_.axis_title_right = QwtText(ui_->lineEditRightAxisLabel->text());

  data_.leftActive   = ui_->checkBoxLeftActive->isChecked();
  data_.rightActive  = ui_->checkBoxRightActive->isChecked();

  data_.leftZoomOn  = ui_->checkBoxZoomYLeft->isChecked();
  data_.rightZoomOn = ui_->checkBoxZoomYRight->isChecked();
  data_.horZoomOn   = ui_->checkBoxZoomXBottom->isChecked();

  callback_();
  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotOptionsDialog::set()
{

  data_.auto_hor ? ui_->radioButtonAutoBottom->setChecked(true)
    :  ui_->radioButtonFixedBottom->setChecked(true);
  ui_->doubleSpinBoxMinBottom->setValue(data_.min_hor );
  ui_->doubleSpinBoxMaxBottom->setValue(data_.max_hor); 
  ui_->doubleSpinBoxMinBottom->setEnabled(!data_.auto_hor);
  ui_->doubleSpinBoxMaxBottom->setEnabled(!data_.auto_hor); 

  
  data_.auto_left ? ui_->radioButtonAutoLeft->setChecked(true)
    :  ui_->radioButtonFixedLeft->setChecked(true);
  ui_->doubleSpinBoxMinLeft->setValue( data_.min_left); 
  ui_->doubleSpinBoxMaxLeft->setValue( data_.max_left);  
  ui_->doubleSpinBoxMinLeft->setEnabled(!data_.auto_left);
  ui_->doubleSpinBoxMaxLeft->setEnabled(!data_.auto_left); 

  data_.auto_right ? ui_->radioButtonAutoRight->setChecked(true)
    :  ui_->radioButtonFixedRight->setChecked(true);
  ui_->doubleSpinBoxMinRight->setValue(data_.min_right);
  ui_->doubleSpinBoxMaxRight->setValue(data_.max_right);
  ui_->doubleSpinBoxMinRight->setEnabled(!data_.auto_right);
  ui_->doubleSpinBoxMaxRight->setEnabled(!data_.auto_right); 

  if (data_.bottom_lego) ui_->radioButtonLegoBottom->setChecked(  data_.bottom_lego );
  if (data_.top_lego)    ui_->radioButtonLegoTop->setChecked(     data_.top_lego    );
  if (data_.hide_lego)   ui_->radioButtonLegoHide->setChecked(    data_.hide_lego   );

  ui_->lineEditTitle->setText(data_.title.text());
  ui_->lineEditBottomAxisLabel->setText(data_.axis_title_hor.text());
  ui_->lineEditLeftAxisLabel->setText(data_.axis_title_left.text());
  ui_->lineEditRightAxisLabel->setText(data_.axis_title_right.text());

  ui_->checkBoxLeftActive->setChecked( data_.leftActive);
  ui_->checkBoxRightActive->setChecked(data_.rightActive);

  ui_->checkBoxZoomYLeft->setChecked( data_.leftZoomOn);
  ui_->checkBoxZoomYRight->setChecked(data_.rightZoomOn);
  ui_->checkBoxZoomXBottom->setChecked(data_.horZoomOn);

}

