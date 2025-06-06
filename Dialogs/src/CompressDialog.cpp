//  =================================================================
//
//  CompressDialog.cpp
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

#include <QSettings>
#include <QFileDialog>
#include <CompressDialog.h>
#include <ui_compress_dialog.h>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CompressDialog::CompressDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::CompressDialog)
{
   ui_->setupUi(this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CompressDialog::~CompressDialog()
{
  delete ui_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CompressDialog::accept()
{
 
  data_.offset         = ui_->spinBoxOffset->value();
  data_.cor            = ui_->checkBoxCorrectors->isChecked();
  data_.sext           = ui_->checkBoxSextupoles->isChecked();
  data_.sol            = ui_->checkBoxSolenoids->isChecked();
  data_.inst           = ui_->checkBoxInstruments->isChecked();
  data_.apert          = ui_->checkBoxRemoveApertures->isChecked();
  data_.wkfld          = ui_->checkBoxRemoveWField->isChecked();
  data_.scatter        = ui_->checkBoxRemoveScatter->isChecked();
  data_.vrbls          = ui_->checkBoxRemoveVars->isChecked();

  data_.filenamefrom   = ui_->lineEditInputFile->text();
  data_.filenameto     = ui_->lineEditOutputFile->text();
;
  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CompressDialog::set()
{
  ui_->spinBoxOffset->setValue(data_.offset );
  ui_->checkBoxCorrectors->setChecked(data_.cor );
  ui_->checkBoxSextupoles->setChecked(data_.sext);
  ui_->checkBoxSolenoids->setChecked(data_.sol);
  ui_->checkBoxInstruments->setChecked(data_.inst);
  ui_->checkBoxRemoveApertures->setChecked( data_.apert    );
  ui_->checkBoxRemoveWField->setChecked(data_.wkfld);
  ui_->checkBoxRemoveScatter->setChecked(data_.scatter );
  ui_->checkBoxRemoveVars->setChecked(data_.vrbls );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CompressDialog::selectInputFile()
{
  QSettings settings;
  QString filenamefrom = QFileDialog::getOpenFileName(0, "Compress Source", settings.value("optworkdir").toString(),
			 "OptimX Lattice Files (*.opt *.opc *.OPC *.OPT)"); // TOpenSaveDialog::TData fd1(0, "(*.opt)|*.opt|(*.*)|*.*", 0, 0, 0);
  if (filenamefrom == "") return;
  ui_->lineEditInputFile->setText(filenamefrom);
  QFileInfo info(filenamefrom);
  ui_->lineEditOutputFile->setText(info.path() + "/" + info.completeBaseName() + ".opc");
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CompressDialog::selectOutputFile()
{
  QSettings settings;
  QString filenameto   = QFileDialog::getSaveFileName(0, "Compress Target", settings.value("optworkdir").toString(),
				  "OptimX Compressed Lattice File (*.opc *.OPC)" ); // TOpenSaveDialog::TData fd2(0, "(*.opc)|*.opc|(*.*)|*.*", 0, 0, 0);
  if (filenameto == "") return;
  ui_->lineEditOutputFile->setText(filenameto);
}
