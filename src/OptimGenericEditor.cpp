//  =================================================================
//
//  OptimGenericEditor.cpp
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
#include <OptimEditor.h>
#include <OptimMainWindow.h> // ??? is this needed ???
#include <OptimMessages.h>

#include <QApplication>
#include <QFileDialog>
#include <QString>
#include <QtGui>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <OptimPlot.h>
#include <OptimMainWindow.h>

QFont OptimEditor::default_font_;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 OptimEditor::OptimEditor()
   : QTextEdit() 
 {
     setAttribute(Qt::WA_DeleteOnClose);
     isUntitled = true;
     setObjectName("OptimEditor");
     setWindowTitle("Lattice Editor");
     setReadOnly(false);	 
     setFont(default_font_);
     setLineWrapMode ( NoWrap );
     
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimEditor::~OptimEditor() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::newFile()
{
     static int sequenceNumber = 1;

     isUntitled = true;
     curFile = tr("unnamed%1.opt").arg(sequenceNumber++);
     setWindowTitle(curFile + "[*]");

     connect(document(), SIGNAL(contentsChanged()),
             this, SLOT(documentWasModified()));
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimEditor::loadFile(const QString &fileName)
 {
     QFile file(fileName);
     if (!file.open(QFile::ReadOnly | QFile::Text)) {
       OptimWarningMessage( this, tr("MDI").toUtf8().data(),
                              (tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
			      .arg(file.errorString()).toUtf8().data()) );
         return false;
     }

     QTextStream in(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     setPlainText(in.readAll());
     QApplication::restoreOverrideCursor();

     setCurrentFile( fileName ); 

     connect(document(), SIGNAL(contentsChanged()),
             this, SLOT(documentWasModified()));

     return true;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimEditor::save()
 {
     if (isUntitled) {
         return saveAs();
     } else {
         return saveFile(curFile);
     }
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimEditor::saveAs()
 {
     QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                     curFile);
     if (fileName.isEmpty())
         return false;

     return saveFile(fileName);
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimEditor::saveFile(const QString &fileName)
 {
     QFile file(fileName);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
       OptimWarningMessage(this, tr("MDI").toUtf8().data(),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
			     .arg(file.errorString()).toUtf8().data());
         return false;
     }

     QTextStream out(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     out << toPlainText();
     QApplication::restoreOverrideCursor();

     setCurrentFile(fileName);
     return true;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 QString OptimEditor::userFriendlyCurrentFile()
 {
     return strippedName(curFile);
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimEditor::closeEvent(QCloseEvent *event)
 {
 
     if (maybeSave()) {
       emit editorClosed(true); 
       OptimMainWindow* mw = qobject_cast<OptimMainWindow*> ( window() );
       mw->detachAll();
       event->accept();
     } 
     else {
       event->ignore();
     }
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimEditor::documentWasModified()
 {
     setWindowModified(document()->isModified());
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimEditor::maybeSave()
 {
     if (document()->isModified()) {
         QMessageBox::StandardButton ret;
         ret = OptimWarningMessage(this, tr("MDI").toUtf8().data(),
                      tr("'%1' has been modified.\n"
                         "Do you want to save your changes?")
				   .arg(userFriendlyCurrentFile()).toUtf8().data(),
                      QMessageBox::Save | QMessageBox::Discard
                      | QMessageBox::Cancel);
         if (ret == QMessageBox::Save)
             return save();
         else if (ret == QMessageBox::Cancel)
             return false;
     }
     return true;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimEditor::setCurrentFile(const QString &fileName)
 {
  
     fileinfo_ = QFileInfo(fileName);
     curFile = QFileInfo(fileName).canonicalFilePath();
     isUntitled = false;
     document()->setModified(false);
     setWindowModified(false);
     setWindowTitle(userFriendlyCurrentFile() + "[*]");
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 QFileInfo const& OptimEditor::fileInfo() const
 {
   return fileinfo_;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 QString OptimEditor::strippedName(const QString &fullFileName)
 {
     return QFileInfo(fullFileName).fileName();
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QString OptimEditor::getLine(int line) const 
{
  // OWL compatibility: returns the contents of line no   

  return document()->findBlockByLineNumber(line).text(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::iterateText() const
{
  QTextCursor cursor = textCursor();
  do { 
       cursor.movePosition( QTextCursor::NextBlock); 
  } while (!cursor. atEnd());      
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::setDefaultFont(QFont const& font)
{
  default_font_ = font;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::highlightCurrentBlock()
{
  QTextEdit::ExtraSelection highlight;
  highlight.cursor = textCursor();
  highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
  highlight.format.setBackground( Qt::red );

  QList<QTextEdit::ExtraSelection> extras;
  extras << highlight;
  setExtraSelections( extras );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::clearHighlightedBlocks()
{
  auto extras = extraSelections();
  extras.clear();
  setExtraSelections(extras);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 	 OptimEditor::keyPressEvent ( QKeyEvent * event )
{   

  auto key       = event->key();
  auto modifiers = event->modifiers(); 


  if ( ( key == Qt::Key_Delete) && (modifiers & (Qt::ShiftModifier | Qt::KeypadModifier)) )  {
         cut();
         event->accept();                                   
  }
  else if ( ( key == Qt::Key_Delete) && (modifiers & Qt::ShiftModifier) )  {
         cut();
         event->accept();                                   
  }
  else if ( ( ( key == Qt::Key_Insert) && (modifiers & Qt::ControlModifier) )  ||  
	    ( ( key == Qt::Key_Insert) && (modifiers & (Qt::ControlModifier | Qt::KeypadModifier ) ) ) )  {
	 event->accept();
	 copy();
  }
  else if ( ( ( key == Qt::Key_Insert) && (modifiers == Qt::ShiftModifier) )    ||
            ( ( key == Qt::Key_Insert) && (modifiers == (Qt::ShiftModifier | Qt::KeypadModifier) ) ) )  {  
         paste(); 
         event->accept();                                   
  }
  else if ( ( ( key == Qt::Key_Insert) && (modifiers == Qt::NoModifier) )    ||
            ( ( key == Qt::Key_Insert) && (modifiers == Qt::KeypadModifier) ) )  {  
         setOverwriteMode(!overwriteMode());    
         event->accept();                                   
  }
  else {
    QTextEdit::keyPressEvent( event);
  }  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

