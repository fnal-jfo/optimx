//  =================================================================
//
//  FunctionDialog.cpp
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

#include <FunctionDialog.h>
#include <iostream>

#include <ui_function_dialog.h>

  FunctionDlgStruct::FunctionDlgStruct()
  {}

  FunctionDlgStruct::FunctionDlgStruct( FunctionDlgStruct const& o)
    : MatchCase(o.MatchCase),
      printname(o.printname),
      NoFirstLetter(o.NoFirstLetter),
      PrintEnerg(o.PrintEnerg),
      Step(o.Step),    
      Start(o.Start),
      Stop(o.Stop),    
      StartSfrom0(o.StartSfrom0)
  { 
    strcpy(Filter, o.Filter);
  }


  FunctionDlgStruct& FunctionDlgStruct::operator=(  FunctionDlgStruct const& rhs)
  {
    if (this == &rhs) return *this;

    MatchCase     = rhs.MatchCase;
    printname     = rhs.printname;
    NoFirstLetter = rhs.NoFirstLetter;
    PrintEnerg    = rhs.PrintEnerg;
    Step          = rhs.Step;
    Start         = rhs.Start;
    Stop          = rhs.Stop;    
    StartSfrom0   = rhs.StartSfrom0;

    strcpy(Filter, rhs.Filter);
    return *this;
 }
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FunctionDialog::FunctionDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::FunctionDialog() )
{
  ui_->setupUi(this);
  ui_->spinBoxStart->setRange(0,10000);
  ui_->spinBoxStop->setRange(0,10000);
  ui_->doubleSpinBoxStep->setDecimals(4);
  ui_->doubleSpinBoxStep->setMaximum(1000);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FunctionDialog::~FunctionDialog()
{
  delete ui_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FunctionDialog::accept()
{
  
  data_.MatchCase = ui_->checkBoxMatchCase->isChecked();
  data_.printname = ui_->checkBoxPrintName->isChecked();
  data_.NoFirstLetter =  ui_->checkBoxOmitFirst->isChecked();
  data_.PrintEnerg    =  ui_->checkBoxPrintEnergy->isChecked();
  data_.Step          =  ui_->doubleSpinBoxStep->value();
  data_.Start         =  ui_->spinBoxStart->value();
  data_.Stop          =  ui_->spinBoxStop->value();
  data_.StartSfrom0   =  ui_->checkBoxStartSAtZero->isChecked();

  strcpy(data_.Filter,  ui_->lineEditFilter->text().toUtf8().data()); 

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FunctionDialog::set()
{

  ui_->checkBoxMatchCase->setChecked(data_.MatchCase);
  ui_->checkBoxPrintName->setChecked(data_.printname);
  ui_->checkBoxOmitFirst->setChecked(data_.NoFirstLetter);
  ui_->checkBoxPrintEnergy->setChecked(data_.PrintEnerg );
  ui_->doubleSpinBoxStep->setValue(data_.Step  );
  ui_->spinBoxStart->setValue( data_.Start);
  ui_->spinBoxStop->setValue(data_.Stop);
  ui_->checkBoxStartSAtZero->setChecked(data_.StartSfrom0);
  ui_->lineEditFilter->setText(data_.Filter);

};

