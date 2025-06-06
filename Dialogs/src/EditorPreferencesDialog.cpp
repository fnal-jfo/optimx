//  =================================================================
//
//  EditorPreferencesDialog.cpp
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
#include <EditorPreferencesDialog.h>
#include <Globals.h>
#include <ui_editor_preferences_dialog.h> 

#include <QColorDialog>
#include <QFontDialog>
#include <QList>

//struct EditorPreferences {
//  uint lattice_editor_tabs;
//  uint text_editor_tabs;
//};



EditorPreferencesDialog::EditorPreferencesDialog( QWidget* parent) 
: QDialog(parent), ui_(new Ui::EditorPreferencesDialog)
{
   ui_->setupUi(this);

  connect( ui_->pushButtonSelectFont, SIGNAL(clicked()), this, SLOT(selectFont()) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EditorPreferencesDialog::~EditorPreferencesDialog()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void EditorPreferencesDialog::accept() 
{
  data_.lattice_editor_tabs  = ui_->spinBoxLatticeEditorTabs->value();
  data_.text_editor_tabs     = ui_->spinBoxTextEditorTabs->value();
  data_.editor_hi_fore_r     = ui_->spinBoxForeR->value();
  data_.editor_hi_fore_g     = ui_->spinBoxForeG->value();
  data_.editor_hi_fore_b     = ui_->spinBoxForeB->value();
  data_.editor_hi_back_r     = ui_->spinBoxBackR->value();
  data_.editor_hi_back_g     = ui_->spinBoxBackG->value();
  data_.editor_hi_back_b     = ui_->spinBoxBackB->value();
  data_.editor_curwidth      = ui_->spinBoxCursor->value();
  data_.editor_showlines     = ui_->checkBoxShowLines->isChecked();
  data_.editor_curline       = ui_->checkBoxCurLine->isChecked();

  QDialog::accept();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EditorPreferencesDialog::set() 
{
  QFont fcurrent;
  fcurrent.fromString(Globals::preferences().editor_default_font.specs);

  QString style;
  if (fcurrent.style() == QFont::StyleNormal)  style = "Normal";
  if (fcurrent.style() == QFont::StyleItalic)  style = "Italic";
  if (fcurrent.style() == QFont::StyleOblique) style = "Oblique";
   
  QString weight;

  switch (fcurrent.weight()) {

     case QFont::Light:
       weight = "Light";
       break;
     case QFont::Normal:
       weight = "Normal";
       break;
     case QFont::DemiBold:
       weight = "DemiBold";
       break;
     case QFont::Bold:
       weight = "Bold";
       break;
     case QFont::Black:
       weight = "Black";
       break;
   default:
     break;
   };

  ui_->lineEditFont->setText( fcurrent.family());
  ui_->lineEditStyle->setText(  style );
  ui_->lineEditWeight->setText(weight);
  ui_->spinBoxSize->setValue(fcurrent.pointSize());

  ui_->spinBoxLatticeEditorTabs->setValue(data_.lattice_editor_tabs);
  ui_->spinBoxTextEditorTabs->setValue(data_.text_editor_tabs);
  ui_->spinBoxForeR->setValue(data_.editor_hi_fore_r);
  ui_->spinBoxForeG->setValue(data_.editor_hi_fore_g);
  ui_->spinBoxForeB->setValue(data_.editor_hi_fore_b);
  ui_->spinBoxBackR->setValue(data_.editor_hi_back_r);
  ui_->spinBoxBackG->setValue(data_.editor_hi_back_g);
  ui_->spinBoxBackB->setValue(data_.editor_hi_back_b);
  ui_->spinBoxCursor->setValue(data_.editor_curwidth);
  ui_->checkBoxShowLines->setChecked(data_.editor_showlines);
  ui_->checkBoxCurLine->setChecked(data_.editor_curline);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EditorPreferencesDialog::selectBackgroundHighlightColor() 
{

  QColor color =
  QColorDialog::getColor(QColor(data_.editor_hi_back_r,
				data_.editor_hi_back_g,
				data_.editor_hi_back_b ), 0, "Select Background Highlight Color",  QColorDialog::ColorDialogOptions());

  if (!color.isValid()) return; 

  data_.editor_hi_back_r   =  color.red(); 
  data_.editor_hi_back_g   =  color.green();
  data_.editor_hi_back_b   =  color.blue();
  set();
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EditorPreferencesDialog::selectForegroundHighlightColor() 
{
   QColor color = QColorDialog::getColor(QColor(data_.editor_hi_fore_r,
						data_.editor_hi_fore_g,
						data_.editor_hi_fore_b ), 0, "Select Foreground Highlight Color", QColorDialog::ColorDialogOptions());

   if (!color.isValid()) return; 
  
   data_.editor_hi_fore_r   =  color.red(); 
   data_.editor_hi_fore_g   =  color.green();
   data_.editor_hi_fore_b   =  color.blue();     
   set();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EditorPreferencesDialog::selectFont() 
{
  QFont fcurrent;
  fcurrent.fromString(Globals::preferences().editor_default_font.specs);

  bool ok = false; 
  QFont selected = QFontDialog::getFont(&ok, fcurrent, this, "Select Font", QFontDialog::MonospacedFonts);
  
  if (ok) { 
   Globals::preferences().editor_default_font.specs = selected.toString();
   set();
  }  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EditorPreferencesDialog::showEvent(QShowEvent * event)
{
  set();
  QDialog::showEvent(event);

}  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

