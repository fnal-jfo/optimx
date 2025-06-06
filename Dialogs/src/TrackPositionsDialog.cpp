//  =================================================================
//
//  TrackPositionsDialog.cpp
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

#include <TrackPositionsDialog.h>
#include <ui_track_positions_dialog.h> 
#include <algorithm>

PltAverPosDlgStruct& PltAverPosDlgStruct::operator=(  PltAverPosDlgStruct const& rhs)
{
  if (&rhs == this) return *this;
   
  std::copy( rhs.X1, rhs.X1+7,  X1 ); 
  std::copy( rhs.X2, rhs.X2+7,  X2 ); 
  
  return *this;
}

PltAverPosDlgStruct::PltAverPosDlgStruct( PltAverPosDlgStruct const& rhs)
{
  std::copy( rhs.X1, rhs.X1+7,  X1 ); 
  std::copy( rhs.X2, rhs.X2+7,  X2 ); 
}

PltAverPosDlgStruct::PltAverPosDlgStruct()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackPositionsDialog::TrackPositionsDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::TrackPositionsDialog)
{
  ui_->setupUi(this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackPositionsDialog::~TrackPositionsDialog()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackPositionsDialog::accept()
{

  data_.X1[0] =  ui_->radioButtonHx->isChecked();
  data_.X1[1] =  ui_->radioButtonHPx->isChecked();
  data_.X1[2] =  ui_->radioButtonHy->isChecked();
  data_.X1[3] =  ui_->radioButtonHPy->isChecked();
  data_.X1[4] =  ui_->radioButtonHs->isChecked();
  data_.X1[5] =  ui_->radioButtonHPs->isChecked();
  data_.X1[6] =  ui_->radioButtonHTurn->isChecked();

  data_.X2[0] =  ui_->radioButtonVx->isChecked();
  data_.X2[1] =  ui_->radioButtonVPx->isChecked();
  data_.X2[2] =  ui_->radioButtonVy->isChecked();
  data_.X2[3] =  ui_->radioButtonVPy->isChecked();
  data_.X2[4] =  ui_->radioButtonVs->isChecked();
  data_.X2[5] =  ui_->radioButtonVPs->isChecked();
  data_.X2[6] =  ui_->radioButtonVTurn->isChecked();

  QDialog::accept();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackPositionsDialog::set()
{
   ui_->radioButtonHx->setChecked(    data_.X1[0]);
   ui_->radioButtonHPx->setChecked(   data_.X1[1]);
   ui_->radioButtonHy->setChecked(    data_.X1[2]);
   ui_->radioButtonHPy->setChecked(   data_.X1[3]);
   ui_->radioButtonHs->setChecked(    data_.X1[4]);
   ui_->radioButtonHPs->setChecked(   data_.X1[5]);
   ui_->radioButtonHTurn->setChecked( data_.X1[6]);

  ui_->radioButtonVx->setChecked(    data_.X2[0]);
  ui_->radioButtonVPx->setChecked(   data_.X2[1]);
  ui_->radioButtonVy->setChecked(    data_.X2[2]);
  ui_->radioButtonVPy->setChecked(   data_.X2[3]);
  ui_->radioButtonVs->setChecked(    data_.X2[4]);
  ui_->radioButtonVPs->setChecked(   data_.X2[5]);
  ui_->radioButtonVTurn->setChecked( data_.X2[6]);

}

