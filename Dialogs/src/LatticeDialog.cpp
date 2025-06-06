//  =================================================================
//
//  LatticeDialog.cpp
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

#include <LatticeDialog.h>
#include <ui_lattice_dialog.h> 


//struct LatticeDlgStruct {
//  char  Filter[12];     // Filter
//  bool  MatchCase;
//  int   MltpOrder;      // order for multipoles
//  bool  printBdL;
//  bool  printFirstLetter;
//  bool  StartSFromFile;
//};

LatticeDlgStruct::LatticeDlgStruct()
  : MatchCase(false),   MltpOrder(10), printBdL(true),
    printFirstLetter(true), StartSFromFile(false)
{
  strcpy(Filter,"*"); 
}
  

LatticeDlgStruct::LatticeDlgStruct(  LatticeDlgStruct const& o)
:  MatchCase(o.MatchCase),   MltpOrder(o.MltpOrder), printBdL(o.printBdL), 
   printFirstLetter(o.printFirstLetter), StartSFromFile(o.StartSFromFile)
{
  strcpy(Filter, o.Filter);
}


LatticeDlgStruct& LatticeDlgStruct::operator=( LatticeDlgStruct const& rhs)
{
  if (&rhs == this) return *this;
  strcpy(Filter, rhs.Filter);
  MatchCase        = rhs.MatchCase;
  MltpOrder        = rhs.MltpOrder;
  printBdL         = rhs.printBdL;
  printFirstLetter = rhs.printFirstLetter;
  StartSFromFile   = rhs.StartSFromFile;
  return *this;
}
  

LatticeDialog::LatticeDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::LatticeDialog)
{
   ui_->setupUi(this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LatticeDialog::~LatticeDialog()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void LatticeDialog::accept() 
{
  strcpy(data_.Filter, ui_->lineEditFilter->text().toUtf8().data());
  data_.MatchCase         =  ui_->checkBoxMatchCase->isChecked();
  data_.MltpOrder         =  ui_->spinBoxMaxPoleOrder->value();
  data_.printBdL          =  ui_->checkBoxPrintBdl->isChecked();
  data_.printFirstLetter  =  !ui_->checkBoxOmitFirstChar->isChecked();
  data_.StartSFromFile    =  ui_->checkBoxUseSFromFile->isChecked();

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LatticeDialog::set() 
{
  ui_->lineEditFilter->setText( data_.Filter);
  ui_->checkBoxMatchCase->setChecked(data_.MatchCase);
  ui_->spinBoxMaxPoleOrder->setValue( data_.MltpOrder);
  ui_->checkBoxPrintBdl->setChecked( data_.printBdL  );
  ui_->checkBoxOmitFirstChar->setChecked(data_.printFirstLetter);
  ui_->checkBoxUseSFromFile->setChecked(data_.StartSFromFile);

}

