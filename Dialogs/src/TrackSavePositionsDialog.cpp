//  =================================================================
//
//  TrackSavePositionsDialog.cpp
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

#include <TrackSavePositionsDialog.h>
#include <ui_track_save_positions_dialog.h> 
#include <cstring>
#include <QFileDialog>


  TracSaveStruct::TracSaveStruct(){}

  TracSaveStruct::TracSaveStruct(TracSaveStruct const& o) 
  : x(o.x), y(o.y), s(o.s), px(o.px), py(o.py), ps(o.ps)
  {
    strcpy(  Filter ,   o.Filter);
    strcpy(  fileName,  o.fileName);
  } 

   
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TracSaveStruct& TracSaveStruct::operator=(TracSaveStruct const& rhs)
  {

    if (this == &rhs) return *this;

    x  = rhs.x;
    y  = rhs.y;
    s  = rhs.s;
    px = rhs.px; 
    py = rhs.py; 
    ps = rhs.ps; 

    strcpy(  Filter,    rhs.Filter    );
    strcpy(  fileName,  rhs.fileName  );
    
    return *this;
  }   


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackSavePositionsDialog::TrackSavePositionsDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::TrackSavePositionsDialog)
{
  ui_->setupUi(this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackSavePositionsDialog::~TrackSavePositionsDialog()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackSavePositionsDialog::accept()
{

  strcpy(data_.Filter,   ui_->lineEditFilter->text().toUtf8().data());
  strcpy(data_.fileName, ui_->lineEditFileName->text().toUtf8().data());
  data_.MatchCase =  ui_->checkBoxX->isChecked(); 
  data_.x  = ui_->checkBoxX->isChecked();
  data_.y  = ui_->checkBoxY->isChecked();
  data_.s  = ui_->checkBoxZ->isChecked();
  data_.px = ui_->checkBoxThetaX->isChecked();
  data_.py = ui_->checkBoxThetaY->isChecked();
  data_.ps = ui_->checkBoxdPP->isChecked();

  QDialog::accept();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackSavePositionsDialog::set()
{
  ui_->lineEditFilter->setText(data_.Filter );
  ui_->lineEditFileName->setText(data_.fileName );
  ui_->checkBoxX->setChecked(data_.MatchCase ); 
  ui_->checkBoxX->setChecked(data_.x  );
  ui_->checkBoxY->setChecked( data_.y );
  ui_->checkBoxZ->setChecked( data_.s );
  ui_->checkBoxThetaX->setChecked(data_.px);
  ui_->checkBoxThetaY->setChecked(data_.py);
  ui_->checkBoxdPP->setChecked(data_.ps);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackSavePositionsDialog::selectFile()
{
  QString filename = QFileDialog::getSaveFileName(0, "Save Positions");
  if (filename != "" ) ui_->lineEditFileName->setText(filename);
}
