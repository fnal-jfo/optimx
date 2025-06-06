//  =================================================================
//
//  ExternalPlotDialog.cpp
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
#include <ExternalPlotDialog.h>
#include <ui_external_plot_dialog.h> 
#include <algorithm> 
#include <cstring> 
#include <QFileDialog>


   ExterPlotDlgStruct::ExterPlotDlgStruct():
   xmin(0 ),    xmax(1),   gridx(10),
   ymin1(-1),  ymax1(1),   gridy1(10),
   ymin2(-1),  ymax2(1),   gridy2(10),
   col{1, 2, 0, 0, 0 },
   axis{0, 0, 0, 0, 0 },
   line{true, true, true, true, true},
   cross{true, true, true, true, true} 
  {
    strcpy(filename, "");
    strcpy(capture,  "");
    strcpy(legend[1], "Y1");
    strcpy(legend[2], "Y2");  
    strcpy(legend[3], "Y3");
    strcpy(legend[4], "Y4");
  } 

  ExterPlotDlgStruct::ExterPlotDlgStruct( ExterPlotDlgStruct const& o):
   xmin(o.xmin ),    xmax(o.xmax ),   gridx(o.gridx),
   ymin1(o.ymin1 ), ymax1(o.ymax1),  gridy1(o.gridy1),
   ymin2(o.ymin2 ), ymax2(o.ymax2),  gridy2(o.gridy2)
  {
    std::copy(  &o.line[0], &o.line[4], &line[0] ); 
    std::copy(  &o.axis[0], &o.axis[4], &axis[0] ); 
    strcpy(filename, o.filename);
    strcpy(capture, o.capture);
    strcpy(legend[0], o.legend[0]);
    strcpy(legend[1], o.legend[1]);
    strcpy(legend[2], o.legend[2]);  
    strcpy(legend[3], o.legend[3]);
    strcpy(legend[4], o.legend[4]);
  }

 ExterPlotDlgStruct& ExterPlotDlgStruct::operator=( ExterPlotDlgStruct const& rhs)
 {
    if (this == &rhs) return *this;
    xmin   = rhs.xmin;   xmax   = rhs.xmax;  gridx  = rhs.gridx;
    ymin1  = rhs.ymin1;  ymax1  = rhs.ymax1; gridy1 = rhs.gridy1;
    ymin2  = rhs.ymin2;  ymax2  = rhs.ymax2; gridy2 = rhs.gridy2;
    
    std::copy(  &rhs.line[0], &rhs.line[4], &line[0] ); 
    std::copy(  &rhs.axis[0], &rhs.axis[4], &axis[0] ); 

    strcpy(filename, rhs.filename);
    strcpy(capture,   rhs.capture);
    strcpy(legend[0], rhs.legend[0]);
    strcpy(legend[1], rhs.legend[1]);
    strcpy(legend[2], rhs.legend[2]);  
    strcpy(legend[3], rhs.legend[3]);
    strcpy(legend[4], rhs.legend[4]);

    return *this;
  }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ExternalPlotDialog::ExternalPlotDialog( QWidget* parent) 
  : QDialog(parent), ui_(new Ui::ExternalPlotDialog)
{
 
   ui_->setupUi(this);

  ui_->spinBoxXMin->setRange(-1,20);     
  ui_->spinBoxXMax->setRange(-1,20);
  ui_->spinBoxXGridSize->setRange(1,1000);  

  ui_->spinBoxYLeftMin->setRange(-1,20);     
  ui_->spinBoxYLeftMax->setRange(-1,20);  
  ui_->spinBoxYLeftGridSize->setRange(1,1000);    

  ui_->spinBoxYRightMin->setRange(-1,20);      
  ui_->spinBoxYRightMax->setRange(-1,20);  
  ui_->spinBoxYRightGridSize->setRange(1,1000);
 
  ui_->spinBoxCurve1Col->setRange(-1,20);  
  ui_->spinBoxCurve2Col->setRange(-1,20);  
  ui_->spinBoxCurve3Col->setRange(-1,20);  
  ui_->spinBoxCurve4Col->setRange(-1,20);  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ExternalPlotDialog::~ExternalPlotDialog()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ExternalPlotDialog::accept()
{

  strcpy(data_.filename, ui_->lineEditFileName->text().toUtf8().data()     );     
  strcpy(data_.capture,  ui_->lineEditPlotCapture->text().toUtf8().data()  );    

  data_.xmin   = ui_->spinBoxXMin->value();     
  data_.xmax   = ui_->spinBoxXMax->value();
  data_.gridx  = ui_->spinBoxXGridSize->value();  

  data_.ymin1  = ui_->spinBoxYLeftMin->value();     
  data_.ymax1  = ui_->spinBoxYLeftMax->value();  
  data_.gridy1 = ui_->spinBoxYLeftGridSize->value();    

  data_.ymin2  = ui_->spinBoxYRightMin->value();      
  data_.ymax2  = ui_->spinBoxYRightMax->value();  
  data_.gridy2 = ui_->spinBoxYRightGridSize->value();    

  data_.col[0]   = ui_->spinBoxXCol->value();  

  data_.col[1]   = ui_->spinBoxCurve1Col->value();  
  data_.axis[1]  = ui_->comboBoxCurve1YSide->currentIndex();  
  strncpy(data_.legend[1], ui_->lineEditCurve1Legend->text().toUtf8().data(), sizeof(data_.legend[1]));
  data_.line[1]  = ui_->checkBoxCurve1Line->isChecked();  
  data_.cross[1] = ui_->checkBoxCurve1Cross->isChecked();

  data_.col[2]   = ui_->spinBoxCurve2Col->value();
  data_.axis[2]  = ui_->comboBoxCurve2YSide->currentIndex();
  strncpy(data_.legend[2],  ui_->lineEditCurve2Legend->text().toUtf8().data(), sizeof(data_.legend[2]));
  data_.line[2]  = ui_->checkBoxCurve2Line->isChecked();  
  data_.cross[2] = ui_->checkBoxCurve2Cross->isChecked();  

  data_.col[3]    = ui_->spinBoxCurve3Col->value();
  data_.axis[3]   = ui_->comboBoxCurve3YSide->currentIndex();
  strncpy(data_.legend[3], ui_->lineEditCurve3Legend->text().toUtf8().data(),sizeof(data_.legend[3]));
  data_.line[3]   = ui_->checkBoxCurve3Line->isChecked();
  data_.cross[3]  = ui_->checkBoxCurve3Cross->isChecked();

  data_.col[4]    = ui_->spinBoxCurve4Col->value();
  data_.axis[4]   = ui_->comboBoxCurve4YSide->currentIndex();
  strncpy(data_.legend[4],  ui_->lineEditCurve4Legend->text().toUtf8().data(),sizeof(data_.legend[4]));
  data_.line[4]   = ui_->checkBoxCurve4Line->isChecked();
  data_.cross[4]  = ui_->checkBoxCurve4Cross->isChecked();

 
  QDialog::accept();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ExternalPlotDialog::set()
{
  
  ui_->lineEditFileName->setText( QString(data_.filename) );
  ui_->lineEditPlotCapture->setText( QString( data_.capture) );

  ui_->spinBoxXMin->setValue(data_.xmin );     
  ui_->spinBoxXMax->setValue(data_.xmax );
  ui_->spinBoxXGridSize->setValue(data_.gridx);  

  ui_->spinBoxYLeftMin->setValue(data_.ymin1);     
  ui_->spinBoxYLeftMax->setValue(data_.ymax1 );  
  ui_->spinBoxYLeftGridSize->setValue(data_.gridy1);    

  ui_->spinBoxYRightMin->setValue( data_.ymin2);      
  ui_->spinBoxYRightMax->setValue(data_.ymax2 );  
  ui_->spinBoxYRightGridSize->setValue(data_.gridy2);    

  ui_->spinBoxXCol->setValue(data_.col[0] );  

  ui_->spinBoxCurve1Col->setValue( data_.col[1]);  
  ui_->comboBoxCurve1YSide->setCurrentIndex( data_.axis[1]);  
  ui_->lineEditCurve1Legend->setText(data_.legend[1]);
  ui_->checkBoxCurve1Line->setChecked(data_.line[1]);  
  ui_->checkBoxCurve1Cross->setChecked(data_.cross[1]);

  ui_->spinBoxCurve2Col->setValue( data_.col[2]);
  ui_->comboBoxCurve2YSide->setCurrentIndex(data_.axis[2]);
  ui_->lineEditCurve2Legend->setText(data_.legend[2]);
  ui_->checkBoxCurve2Line->setChecked(data_.line[2] );  
  ui_->checkBoxCurve2Cross->setChecked(data_.cross[2]);  

  ui_->spinBoxCurve3Col->setValue( data_.col[3] );
  ui_->comboBoxCurve3YSide->setCurrentIndex(data_.axis[3]);
  ui_->lineEditCurve3Legend->setText(data_.legend[3]);
  ui_->checkBoxCurve3Line->setChecked(data_.line[3]);
  ui_->checkBoxCurve3Cross->setChecked(data_.cross[3]);

  ui_->spinBoxCurve4Col->setValue( data_.col[4] );
  ui_->comboBoxCurve4YSide->setCurrentIndex(data_.axis[4] );
  ui_->lineEditCurve4Legend->setText(data_.legend[4]);
  ui_->checkBoxCurve4Line->setChecked(data_.line[4]);
  ui_->checkBoxCurve4Cross->setChecked(data_.cross[4] );


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ExternalPlotDialog::fileSelect() 
{

  QString filename = QFileDialog::getOpenFileName(0, "Select Data File for External Plot"); 
  ui_->lineEditFileName->setText( filename );
}  
    
  
