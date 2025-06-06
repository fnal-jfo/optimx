//  =================================================================
//
//  EditMeasuredDataDialog.cpp
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

#include <EditMeasuredDataDialog.h>
#include <ui_edit_measured_data_dialog.h>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QColorDialog>
#include <QPalette>
#include <QtDebug>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EditMeasuredDataDialog::EditMeasuredDataDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::EditMeasuredDataDialog)
{
   ui_->setupUi(this);
   ui_->labelColor->setText("");
   ui_->labelColor->setBackgroundRole(QPalette::Window);
   connect( ui_->radioButtonSetColor, SIGNAL(clicked()), SLOT(setColor()) ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EditMeasuredDataDialog::~EditMeasuredDataDialog()
{
  delete ui_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void EditMeasuredDataDialog::accept()
{
  
  data_.show_legend     = ui_->checkBoxShowLegend->isChecked(); 
  data_.show_full_path  = ui_->checkBoxShowFullPath->isChecked(); 
  data_.data_only       = ui_->checkBoxDataPoints->isChecked(); 
  data_.remove_set      = ui_->radioButtonRemoveSet->isChecked();
  data_.current_txt     = ui_->comboBoxDataSets->currentText();
  data_.current_idx     = ui_->comboBoxDataSets->currentIndex();
  data_.setcolor        = ui_->radioButtonSetColor->isChecked();

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void EditMeasuredDataDialog::set()
{
  ui_->checkBoxShowLegend->setChecked(data_.show_legend ); 
  ui_->checkBoxShowFullPath->setChecked(data_.show_full_path ); 
  ui_->checkBoxDataPoints->setChecked( data_.data_only);
  ui_->comboBoxDataSets->setCurrentIndex(data_.current_idx);
  ui_->buttonGroup->setExclusive(false);
  ui_->radioButtonRemoveSet->setChecked( false );
  ui_->radioButtonSetColor->setChecked(false);
  ui_->buttonGroup->setExclusive(true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EditMeasuredDataDialog::setDataSets(QStringList const& list) 
{

  // first, remove all existing items
  ui_->comboBoxDataSets->clear();
    ui_->comboBoxDataSets->addItems(list);
  ui_->comboBoxDataSets->setCurrentIndex(0);

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EditMeasuredDataDialog::setColor()
{
  qDebug() << "setColor()";
  data_.color = QColorDialog::getColor(); 
  QPalette palette = ui_->labelColor->palette();
  palette.setColor(QPalette::Window, data_.color);
  palette.setBrush(QPalette::Window, QBrush(data_.color));
  ui_->labelColor->setAutoFillBackground(true);
  ui_->labelColor->setPalette(palette);
  ui_->labelColor->update();
}
