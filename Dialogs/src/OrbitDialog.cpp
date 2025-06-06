//  =================================================================
//
//  OrbitDialog.cpp
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

#include <OrbitDialog.h>
#include <ui_orbit_dialog.h>

OrbitNStruct::OrbitNStruct()
  : x(0.0),
    y(0.0),
    z(0.0),
    s(0.0), 
    tetax(0.0),
    tetay(0.0),
    ds(0.0),
    FirstLetter(false),
    MatchCase(false),
    StartFromFile(false),
    UseMADFrame(false)
{
  strcpy(Filter, "");
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OrbitNStruct::OrbitNStruct( OrbitNStruct const& o) 
  : x(o.x), y(o.y), z(o.z), s(o.s), 
    tetax(o.tetax), tetay(o.tetay),
    StartFromFile(o.StartFromFile),
    ds(o.ds), FirstLetter(o.FirstLetter), MatchCase(o.MatchCase),
    UseMADFrame(o.UseMADFrame)
{
  strcpy(Filter, o.Filter);
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OrbitNStruct& OrbitNStruct::operator=( OrbitNStruct const& rhs)
{
  if ( &rhs == this) return *this;
  
  x             = rhs.x;
  y             = rhs.y;
  z             = rhs.z;
  s             = rhs.s;
  tetax         = rhs.tetax;
  tetay         = rhs.tetay;
  StartFromFile = rhs.StartFromFile;
  ds            = rhs.ds;
  FirstLetter   = rhs.FirstLetter;
  strcpy(Filter, rhs.Filter);
  MatchCase     = rhs.MatchCase ;
  UseMADFrame   = rhs.UseMADFrame;

  return *this;
};
  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OrbitDialog::OrbitDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::OrbitDialog)
{
   ui_->setupUi(this);

   ui_->doubleSpinBoxX0->setMinimum(-1.0e99);
   ui_->doubleSpinBoxX0->setMaximum( 1.0e99); 

   ui_->doubleSpinBoxY0->setMinimum(-1.0e99);
   ui_->doubleSpinBoxY0->setMaximum( 1.0e99); 

   ui_->doubleSpinBoxZ0->setMinimum(-1.0e99);
   ui_->doubleSpinBoxZ0->setMaximum( 1.0e99); 

   ui_->doubleSpinBoxS0->setMinimum(-1.0e99);
   ui_->doubleSpinBoxS0->setMaximum( 1.0e99); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OrbitDialog::~OrbitDialog()
{
  delete ui_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OrbitDialog::accept()
{
  data_.x             = ui_->doubleSpinBoxX0->value();
  data_.y             = ui_->doubleSpinBoxY0->value();
  data_.z             = ui_->doubleSpinBoxZ0->value();
  data_.tetax         = ui_->doubleSpinBoxXp0->value();
  data_.tetay         = ui_->doubleSpinBoxYp0->value();
  data_.s             = ui_->doubleSpinBoxS0->value();
  data_.ds            = ui_->doubleSpinBoxStepSize->value();
  data_.StartFromFile = ui_->checkBoxInitFromFile->isChecked();
  data_.MatchCase     = ui_->checkBoxInitMatchCase->isChecked();
  data_.FirstLetter   = ui_->checkBoxNoPrintFirst->isChecked();
  data_.UseMADFrame   = ui_->checkBoxUseMADFrame->isChecked();
  strcpy( data_.Filter, ui_->lineEditFilter->text().toUtf8().data());

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OrbitDialog::set()
{
  ui_->doubleSpinBoxX0->setValue(data_.x);
  ui_->doubleSpinBoxY0->setValue(data_.y);
  ui_->doubleSpinBoxZ0->setValue(data_.z);
  ui_->doubleSpinBoxXp0->setValue(data_.tetax);
  ui_->doubleSpinBoxYp0->setValue(data_.tetay);
  ui_->doubleSpinBoxS0->setValue(data_.s);
  ui_->doubleSpinBoxStepSize->setValue(data_.ds);
  ui_->checkBoxInitFromFile->setChecked(data_.StartFromFile);
  ui_->checkBoxInitMatchCase->setChecked(data_.MatchCase);
  ui_->checkBoxNoPrintFirst->setChecked(data_.FirstLetter);
  ui_->checkBoxUseMADFrame->setChecked(data_.UseMADFrame);
  ui_->lineEditFilter->setText(data_.Filter);

}

