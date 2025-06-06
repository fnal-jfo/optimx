//  =================================================================
//
//  MeasuredDataDialog.cpp
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
#include <MeasuredDataDialog.h>
#include <ui_measured_data_dialog.h>
#include <QFileDialog>
#include <QString>
#include <QVariant>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MeasuredDataDialog::MeasuredDataDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::MeasuredDataDialog)
{
   ui_->setupUi(this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MeasuredDataDialog::~MeasuredDataDialog()
{
  delete ui_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void MeasuredDataDialog::accept()
{
  
     data_.fname         = ui_->lineEditDataFile->text(); 
     data_.Scol          = ui_->spinBoxScol->value();
     data_.DataCol       = ui_->spinBoxDataCol->value();
     data_.ErrpCol       = ui_->spinBoxErrpCol->value();
     data_.ErrmCol       = ui_->spinBoxErrmCol->value();
     data_.Color         = ui_->spinBoxColor->value();
     data_.Cross         = ui_->spinBoxCross->value();
     data_.Scaling       = ui_->doubleSpinBoxScaling->value();
     data_.Xoffs         = ui_->doubleSpinBoxXoffs->value();
     data_.RefToS0       = ui_->checkBoxRefToS0->isChecked();
     data_.SfromOptics   = ui_->checkBoxSfromOptics->isChecked();
     data_.NameCol       = ui_->spinBoxNameCol->value();;
     strcpy(data_.NamePrefix, ui_->lineEditNamePrefix->text().toUtf8().data());
     strcpy(data_.filter,     ui_->lineEditfilter->text().toUtf8().data());
     data_.MatchCase     = ui_->checkBoxMatchCase->isChecked();

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void MeasuredDataDialog::set()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MeasuredDataDialog::getFileName()
{
   // try optdataworkdir
   // if it does not exist, try optworkdir;
   // if that fails too, use CWD

   QDir dir(".");
   if (  settings_.value("optdataworkdir") != QVariant() ) {
      dir.setPath( settings_.value("optdataworkdir").toString() );
   }
   else if( settings_.value("optworkdir") != QVariant() ) {
       dir.setPath( settings_.value("optworkdir").toString() );
   }

   if (!dir.exists()) dir.setPath("."); 
   
   QString fname = QFileDialog::getOpenFileName(0, "Read Data File", dir.absolutePath() ); 
   ui_->lineEditDataFile->setText(fname);

   if (fname == "" ) return;

     
   QFileInfo fi(fname);
   if (fi.exists()) settings_.setValue("optdataworkdir", fi.absolutePath() );

   
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
