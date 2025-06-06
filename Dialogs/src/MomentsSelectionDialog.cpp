//  =================================================================
//
//  MomentsSelectionDialog.cpp
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

#include <MomentsSelectionDialog.h>
#include <ui_moments_selection_dialog.h>

#include <algorithm>
#include <iostream>



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MomentsSelectionDialog::MomentsSelectionDialog(QWidget* parent)
  : QDialog(parent), ui_(new Ui::MomentsSelectionDialog() )
{
  ui_->setupUi(this);
  ui_->buttonGroup->setExclusive(false);
  auto buttons = ui_->buttonGroup->buttons(); 

  auto lt = [this](QAbstractButton *left, QAbstractButton *right)
  {
    auto lname = left->objectName();
    auto rname = right->objectName();
    return lname < rname;		    
  };
  
  std::sort(buttons.begin(), buttons.end(), lt);

  QAbstractButton* pbuttons[6][6];
  
  for (int irow=0; irow<6; ++irow) {
    for (int icol=0; icol<=irow; ++icol) {
      data_[irow][icol]     = data_[icol][irow] = false;
      auto cb = buttons.front(); 
      buttons.pop_front();	
      pbuttons[irow][icol]  = pbuttons[icol][irow] = cb;
      ui_->buttonGroup->setId(cb, irow*10+icol);
      connect(cb, &QAbstractButton::toggled,  [irow, icol,this](bool state) { setState(irow,icol, state);});
     }
  }

  // default selection ... 
  pbuttons[0][0]->setChecked(true);
  pbuttons[2][2]->setChecked(true);
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MomentsSelectionDialog::~MomentsSelectionDialog()
{
  delete ui_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void MomentsSelectionDialog::accept()
{
  return QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MomentsSelectionDialog::setState(int row, int col, bool state)
{
  data_[row][col] = data_[col][row] = state;
}
