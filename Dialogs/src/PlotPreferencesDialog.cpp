//  =================================================================
//
//  PlotPreferencesDialog.cpp
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
#include <PlotPreferencesDialog.h>
#include <QFontDialog>
#include <QSettings>
#include <Globals.h>
#include <ui_plot_preferences_dialog.h>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotPreferencesDialog::PlotPreferencesDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::PlotPreferencesDialog() )
{
  ui_->setupUi(this);

  data_.resolution  = ui_->spinBoxResolution->value();
  data_.hsize       = ui_->doubleSpinBoxHSize->value();
  data_.vsize       = ui_->doubleSpinBoxVSize->value();

  ui_->spinBoxResolution->setRange(72,1200);

  ui_->doubleSpinBoxHSize->setRange(0.5,48.0); 
  ui_->doubleSpinBoxVSize->setRange(0.5,48.0); 
  ui_->doubleSpinBoxLWidth->setRange(1.0,10.0);
  ui_->spinBoxAxesLWidth->setRange(1 ,10);
  ui_->spinBoxFontSize->setRange(1,100);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotPreferencesDialog::~PlotPreferencesDialog()
{
  delete ui_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QFont PlotPreferencesDialog::selectAxisFont()
{
  
#if 0 
  QSettings settings;
  OptimPreferences& p =  Globals::preferences();
  
  bool ok = false;
  QFont initial = p.plot_preferences.font     =   settings.value("fontsize",      p.plot_preferences.fontsize ).toInt();
  QFont initial = p.plot_preferences.axisfont =   settings.value("axisfont",      p.plot_preferences.axisfont).value<QFont>();
  QFont font = QFontDialog::getFont( &ok, initial, this, "Axis Font", QFontDialog::MonospacedFonts); 
  if (ok) { settings.setValue("axisfont",      p.plot_preferences.axisfont, font);}

  return font;
#endif
  return QFont();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotPreferencesDialog::accept()
{

  data_.resolution   = ui_->spinBoxResolution->value();
  data_.hsize        = ui_->doubleSpinBoxHSize->value();
  data_.vsize        = ui_->doubleSpinBoxVSize->value();
  data_.lwidth       = ui_->doubleSpinBoxLWidth->value();
  data_.alwidth      = ui_->spinBoxAxesLWidth->value();
  data_.fontsize     = ui_->spinBoxFontSize->value();
  data_.timestamp    = ui_->checkBoxTimeStamp->isChecked();
  data_.compact      = 0x0;
  data_.compact    |= ( ui_->checkBoxCompactScales->isChecked() ? 0x1 : 0x0);
  data_.compact    |= ( ui_->checkBoxCompactLegend->isChecked() ? 0x2 : 0x0);
  data_.title        = ui_->checkBoxTitle->isChecked();
  data_.coloredframe = ui_->checkBoxColoredFrame->isChecked();

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotPreferencesDialog::set()
{

  ui_->spinBoxResolution->setValue(  data_.resolution  );
  ui_->doubleSpinBoxHSize->setValue(data_.hsize    );
  ui_->doubleSpinBoxVSize->setValue(data_.vsize    );
  ui_->doubleSpinBoxLWidth->setValue(data_.lwidth        );
  ui_->spinBoxAxesLWidth->setValue(data_.alwidth        );
  ui_->spinBoxFontSize->setValue( data_.fontsize      );
  ui_->checkBoxTimeStamp->setChecked( data_.timestamp );
  ui_->checkBoxCompactScales->setChecked( data_.compact & 0x1 );
  ui_->checkBoxCompactLegend->setChecked( data_.compact & 0x2 );
  ui_->checkBoxTitle->setChecked( data_.title);
  ui_->checkBoxColoredFrame->setChecked(data_.coloredframe);
}

