//  =================================================================
//
//  OptimTextEditor.cpp
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
#include <OptimTextEditor.h>
#include <OptimMessages.h>
#include <Globals.h>
#include <CustomEvents.h>

#include <QApplication>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QString>
#include <QtGui>
#include <QPalette>
#include <QFontMetrics>

QFont OptimTextEditor::default_font_;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 OptimTextEditor::OptimTextEditor(QWidget* parent)
   : QPlainTextEdit(parent), close_is_allowed_(true) 
 {
     auto& gp = Globals::preferences();

     setAttribute(Qt::WA_DeleteOnClose);
     isUntitled = true;
     setObjectName("OptimTextEditor");
     setWindowTitle("OptiM Output Window");

     default_font_.fromString(gp.editor_default_font.specs);
     setFont( default_font_);

     curFile = "";
     setLineWrapMode ( NoWrap );
     installEventFilter(this);
     QFontMetrics fm (default_font_);
#if QT_VERSION < 0x050a00
     setTabStopWidth( Globals::preferences().text_output_tabsp*fm.maxWidth() );
#else
     setTabStopDistance( Globals::preferences().text_output_tabsp*fm.maxWidth() );
#endif
     QPalette p = palette();

     p.setColor(QPalette::Highlight, QColor(gp.editor_hi_back_r,
					    gp.editor_hi_back_g,
					    gp.editor_hi_back_b));


     p.setColor(QPalette::HighlightedText, QColor(gp.editor_hi_fore_r,
				                  gp.editor_hi_fore_g,
				                  gp.editor_hi_fore_b));
     setPalette(p);
     setCursorWidth(gp.editor_curwidth);

     if ( gp.editor_curline) { 
      connect(this, &OptimTextEditor::cursorPositionChanged,  this, &OptimTextEditor::highlightCurrentLine );
     }

     ensureCursorVisible();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimTextEditor::~OptimTextEditor() 
{
  emit destroyed();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimTextEditor::newFile()
 {
     static int sequenceNumber = 1;

     isUntitled = true;
     curFile = tr("unnamed%1.txt").arg(sequenceNumber++);
     setWindowTitle(curFile + "[*]");

     connect(document(), SIGNAL(contentsChanged()),
             this, SLOT(documentWasModified()));
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimTextEditor::loadFile(const QString &fileName)
 {
     QFile file(fileName);
     if (!file.open(QFile::ReadOnly | QFile::Text)) {
       OptimMessageBox::warning(this, tr("MDI").toUtf8().data(),
                              tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
			   .arg(file.errorString()).toUtf8().data());
         return false;
     }

     QTextStream in(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     setPlainText(in.readAll());
     QApplication::restoreOverrideCursor();

     setCurrentFile(fileName);

     connect(document(), SIGNAL(contentsChanged()),
             this, SLOT(documentWasModified()));

     return true;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimTextEditor::save()
 {
     if (isUntitled) {
         return saveAs();
     } else {
         return saveFile(curFile);
     }
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimTextEditor::saveAs()
 {
     QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                     curFile);
     if (fileName.isEmpty())
         return false;

     return saveFile(fileName);
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimTextEditor::saveFile(const QString &fileName)
 {
     QFile file(fileName);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
       OptimMessageBox::warning(this, tr("MDI").toUtf8().data(),
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

 QString OptimTextEditor::userFriendlyCurrentFile()
 {
     return strippedName(curFile);
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimTextEditor::closeEvent(QCloseEvent *event)
 {
     if (maybeSave() && close_is_allowed_) {
         event->accept();
     } else {

       // post a custom event to notify the state machine that
       // the window close action has been cancelled by the user.
       
       QEvent* e = new CloseCancelledEvent();
       QCoreApplication::postEvent(this, e);
       event->ignore();
     }
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimTextEditor::documentWasModified()
 {
     setWindowModified(document()->isModified());
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bool OptimTextEditor::maybeSave()
 {
     if (document()->isModified()) {
         QMessageBox::StandardButton ret;
         QString name = (curFile == "") ? windowTitle() : userFriendlyCurrentFile();  
         ret = OptimMessageBox::warning(this, tr("MDI").toUtf8().data(),
                      tr("'%1' has been modified.\n"
                         "Do you want to save your changes?")
				   .arg(name).toUtf8().data(),
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


 QFileInfo const& OptimTextEditor::fileInfo() const
 {
   return fileinfo_;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 QString OptimTextEditor::strippedName(const QString &fullFileName)
 {
     return QFileInfo(fullFileName).fileName();
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimTextEditor::setCurrentFile(const QString &fileName)
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

QString OptimTextEditor::getLine(int line) const 
{
  // OWL compatibility: returns the contents of line no   

  return document()->findBlockByLineNumber(line).text(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::iterateText() const
{

  QTextCursor cursor = textCursor();

  do { 
       //std::cout << cursor.block().text().toUtf8().constData() << std::endl; 
       cursor.movePosition( QTextCursor::NextBlock); 
  } while (!cursor. atEnd());      
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::showEvent(QShowEvent* event) 
{
  emit editorOpened(); 
  QPlainTextEdit::showEvent(event);

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::setDefaultFont(QFont const& font)
{
  default_font_ = font; 
  Globals::preferences().editor_default_font.family     = font.family().toLatin1();
  Globals::preferences().editor_default_font.point_size = (uint) font.pointSize();
  Globals::preferences().editor_default_font.weight     = (uint) font.weight();
  Globals::preferences().editor_default_font.italics    = font.italic();
  //update();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OptimTextEditor::eventFilter(QObject *watched, QEvent* event) 
{

  // Display line and column.
  // this code should be moved to the editor widget

   if ( event->type() ==  QEvent::MouseButtonPress ) {
      
    QMouseEvent*      mevent = dynamic_cast<QMouseEvent*>(event);
    QTextCursor       cursor = cursorForPosition( mevent->pos());

     // mouse button press events inside a QTextEdit widget are associated with the viewport (child) widget. 

     emit cursorPosition( QPoint( cursor.columnNumber(), cursor.blockNumber()) );
   }
   
   return QObject::eventFilter(watched, event);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::repaint() 
{
  QMdiSubWindow* sw = qobject_cast<QMdiSubWindow*>(parent());
  if (!sw) {
    QPlainTextEdit::repaint();
    return;
  }
  else {
    if (sw->property("attached") == QVariant()) { QPlainTextEdit::repaint(); return;}
  }
  
  QPalette p = palette();
  if  ( parent()->property("attached").toBool() ) { 
      p.setColor(QPalette::Base, QColor("white"));
      sw->setWindowTitle( sw->windowTitle().replace(" [Detached]", ""));
  }
  else {
     p.setColor(QPalette::Base, QColor("whitesmoke"));
     sw->setWindowTitle( sw->windowTitle().replace(" [Detached]", "") + " [Detached]"); 
  }
  setPalette(p); 
  QPlainTextEdit::repaint();

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::highlightCurrentBlock(int nline)
{
  QTextEdit::ExtraSelection highlight;
  highlight.cursor = textCursor();
  if (nline <= -1 ) {  // move the cusor to line nline  
    highlight.cursor = textCursor();
  }
  else {
    highlight.cursor.movePosition( QTextCursor::Start,QTextCursor::MoveAnchor);
    highlight.cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, nline);
  }
    
  highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
  highlight.format.setBackground( Qt::red );

  QList<QTextEdit::ExtraSelection> extras;
  extras << highlight;
  setExtraSelections( extras );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::highlightCurrent()
{
  QTextEdit::ExtraSelection highlight;
  highlight.cursor = textCursor();
  highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
  highlight.format.setBackground( Qt::yellow );
  highlight.format.setBackground( Qt::blue );

  QList<QTextEdit::ExtraSelection> extras;
  extras << highlight;
  setExtraSelections( extras );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::clearHighlightedBlocks()
{
  auto extras = extraSelections();
  extras.clear();
  setExtraSelections(extras);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 	 OptimTextEditor::keyPressEvent ( QKeyEvent * event )
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
  else if ( (modifiers == Qt::AltModifier) ) {
         event->ignore();  
  }
  else {
    QPlainTextEdit::keyPressEvent( event);
  }  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::setCloseAllowed(bool enable)
{
  close_is_allowed_ = enable;
}

void OptimTextEditor::focusInEvent(QFocusEvent *e)
{
  // std::cout << "focusInEvent objectid = " << (long) this << std::endl;
  QPlainTextEdit::focusInEvent(e);
}
  
void OptimTextEditor::focusOutEvent(QFocusEvent *e)
{
  // std::cout << "focusOutEvent objectid = " << (long) this << std::endl;
  QPlainTextEdit::focusOutEvent(e);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTextEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setForeground(Qt::blue);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
