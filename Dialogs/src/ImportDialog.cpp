//  =================================================================
//
//  ImportDialog.cpp
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
#include <ImportDialog.h>
#include <ui_import_dialog.h>
#include <QFileDialog>
#include <QComboBox>
#include <QVariant>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ImportDialog::ImportDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::ImportDialog)
{
   ui_->setupUi(this);
   ui_->comboBoxParticles->addItem("proton",    938.272046 );
   ui_->comboBoxParticles->addItem("positron",  0.510998928);
   ui_->comboBoxParticles->addItem("anti-muon", 105.6583715);
   ui_->comboBoxParticles->addItem("custom",    938.272046 );
   ui_->comboBoxParticles->setCurrentIndex(0);
   setParticleMass(  ui_->comboBoxParticles->itemText(0) );

   //  ui_->techSpinBoxTotEnergy->installEventFilter(this); 
   //  ui_->techSpinBoxKinEnergy->installEventFilter(this); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ImportDialog::~ImportDialog()
{
  delete ui_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void ImportDialog::accept() 
{
  data_.energy          = ui_->techSpinBoxTotEnergy->value();
  data_.kenergy         = ui_->techSpinBoxKinEnergy->value();
  data_.mass            = ui_->lineEditMass->text();
  data_.filenameto      = ui_->lineEditOutputFile->text();

  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ImportDialog::set()
{
  ui_->techSpinBoxTotEnergy->setValue(data_.energy );
  ui_->lineEditMass->setText(data_.mass );
  ui_->techSpinBoxKinEnergy->setValue(data_.energy - ui_->lineEditMass->text().toDouble() );
  ui_->lineEditOutputFile->setText( data_.filenameto);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ImportDialog::setOutputFilename()
{
  QString filenameto  =  QFileDialog::getSaveFileName(0, "Import from MAD: output file");   // (0, "(*.opt)|*.opt|(*.*)|*.*", 0, 0, 0);
  if ( filenameto == "" )  return;  
  ui_->lineEditOutputFile->setText(filenameto);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ImportDialog::setParticleMass( QString const& selection )
{
  int     index = ui_->comboBoxParticles->findText( selection);

  ui_->lineEditMass->setText( QString("%1").arg( ui_->comboBoxParticles->itemData(index).toDouble()) ); 

  if (  ui_->comboBoxParticles->itemText(index) == "custom" ) {
    ui_->lineEditMass->setEnabled(true);
  }
  else {
    ui_->lineEditMass->setEnabled(false);
  }
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ImportDialog::updatedKinEnergy(double ekin)
{
  double mass = atof(ui_->lineEditMass->text().toUtf8().data());
  ui_->techSpinBoxTotEnergy->blockSignals(true);
  ui_->techSpinBoxTotEnergy->setValue(ekin + mass );  
  ui_->techSpinBoxTotEnergy->blockSignals(false);
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ImportDialog::updatedMass(QString strval)
{
  double mass = atof(ui_->lineEditMass->text().toUtf8().data());
  ui_->techSpinBoxKinEnergy->blockSignals(true);
  double etot =  ui_->techSpinBoxTotEnergy->value();
  ui_->techSpinBoxKinEnergy->setValue(etot-mass );
  ui_->techSpinBoxKinEnergy->blockSignals(false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ImportDialog::updatedTotEnergy(double etot)
{
  double mass = atof(ui_->lineEditMass->text().toUtf8().data());
  ui_->techSpinBoxKinEnergy->blockSignals(true);
  ui_->techSpinBoxKinEnergy->setValue(etot-mass );
  ui_->techSpinBoxKinEnergy->blockSignals(false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
