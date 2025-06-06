//  =================================================================
//
//  EditReadDialog.cpp
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

#include <QFileDialog>
#include <EditReadDialog.h>
#include <ui_edit_read_dialog.h>

EditReadDlgStruct::EditReadDlgStruct()
  {}

EditReadDlgStruct::EditReadDlgStruct(EditReadDlgStruct const& o)
  : MatchCase(o.MatchCase), CompareOnly(o.CompareOnly), Scale(o.Scale)
{
  strcpy( Filter,      o.Filter);
  strcpy( FirstLetter, o.FirstLetter);
  strcpy( FileNam,     o.FileNam);

} 

EditReadDlgStruct& EditReadDlgStruct::operator=( EditReadDlgStruct const& rhs)
{
  if (this == &rhs) return (*this);

  strcpy(Filter, rhs.Filter);
  MatchCase   = rhs.MatchCase;
  strcpy(FirstLetter, rhs.FirstLetter);
  CompareOnly = rhs.CompareOnly;
  Scale       = rhs.Scale;  
  strcpy(FileNam, rhs.FileNam);

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EditReadDialog::EditReadDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::EditReadDialog)
{
   ui_->setupUi(this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EditReadDialog::~EditReadDialog()
{
  delete ui_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void EditReadDialog::accept()
{

  strcpy(data_.Filter,   ui_->lineEditFilter->text().toUtf8().data());     
  data_.MatchCase     =  ui_->checkBoxMatchCase->isChecked();
  strcpy(data_.FirstLetter,  ui_->lineEditNamePrefix->text().toUtf8().data());
  data_.MatchCase     =  ui_->checkBoxMatchCase->isChecked(); 
  data_.CompareOnly   =  ui_->checkBoxCompareOnly->isChecked();
  data_.Scale         =  ui_->doubleSpinBoxScale->value();

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void EditReadDialog::set()
{
  ui_->lineEditFilter->setText(data_.Filter);     
  ui_->checkBoxMatchCase->setChecked(data_.MatchCase);
  ui_->lineEditNamePrefix->setText(data_.FirstLetter);
  ui_->checkBoxMatchCase->setChecked(data_.MatchCase); 
  ui_->checkBoxCompareOnly->setChecked(data_.CompareOnly);
  ui_->doubleSpinBoxScale->setValue(data_.Scale);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EditReadDialog::selectFile()
{

  QString filename = QFileDialog::getOpenFileName(0, "Select File");  

  if (filename == "") return;

  strcpy(data_.FileNam, filename.toUtf8().data());
  ui_->lineEditFileName->setText(data_.FileNam);
}  
  
