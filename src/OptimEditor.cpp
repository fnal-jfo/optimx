//  =================================================================
//
//  OptimEditor.cpp
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
#include <OptimStateMachine.h>
#include <CustomEvents.h>
#include <Globals.h>

#include <QtGlobal>
#include <QApplication>
#include <QPalette>
#include <QFileDialog>
#include <QString>
#include <QtGui>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFont>
#include <QFontMetrics>

#include <OptimPlot.h>
#include <OptimMainWindow.h>


QFont OptimEditor::default_font_;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimEditor::OptimEditor(QWidget* parent)
  : QPlainTextEdit(parent), close_is_allowed_(true) 
 {
     setAttribute(Qt::WA_DeleteOnClose);
     isUntitled = true;
     setObjectName("OptimEditor");
     setWindowTitle("Lattice Editor");
     setReadOnly(false);	 

     default_font_.fromString(Globals::preferences().editor_default_font.specs);
     setFont(default_font_);

     setLineWrapMode ( NoWrap );
     QFontMetrics fm = fontMetrics();
#if QT_VERSION < 0x050A00
     setTabStopWidth( Globals::preferences().lattice_editor_tabsp*fm.maxWidth() );
#else
     setTabStopDistance( Globals::preferences().lattice_editor_tabsp*fm.maxWidth() );
#endif     
     QPalette p = palette();
     p.setColor(QPalette::Highlight, QColor(Globals::preferences().editor_hi_back_r,
					    Globals::preferences().editor_hi_back_g,
					    Globals::preferences().editor_hi_back_b));


     p.setColor(QPalette::HighlightedText, QColor(Globals::preferences().editor_hi_fore_r,
				           Globals::preferences().editor_hi_fore_g,
				           Globals::preferences().editor_hi_fore_b));
     setPalette(p);
     setCursorWidth(Globals::preferences().editor_curwidth);
     ensureCursorVisible();

    if (Globals::preferences().editor_showlines) {
      lineNumberArea_ = new LineNumberArea(this);
    }
    else {
      lineNumberArea_ =0;
    }

    if ( Globals::preferences().editor_showlines) { 
      connect(this, &OptimEditor::blockCountChanged,      this, &OptimEditor::updateLineNumberAreaWidth );
      connect(this, &OptimEditor::updateRequest,          this, &OptimEditor::updateLineNumberArea );
    }


    if ( Globals::preferences().editor_curline) { 
      connect(this, &OptimEditor::cursorPositionChanged,  this, &OptimEditor::highlightCurrentLine );
    }
    
    updateLineNumberAreaWidth(0);

    emit cursorPositionChanged(); // highlightCurrentLine();

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
       OptimMessageBox::warning( this, tr("MDI").toUtf8().data(),
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

     ensureCursorVisible();
     
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

 QString OptimEditor::userFriendlyCurrentFile()
 {
     return strippedName(curFile);
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimEditor::closeEvent(QCloseEvent* event)
 {
   // if the lattice editor is closed, emit the editorClosed signal to notify the
   // state machine and detach all the related plot windows.
   // If the user cancels the close action, ignore the QCloseEvent and
   // emit a custom CloseCancelledEvent no notify the state machine.  

   if (maybeSave() && close_is_allowed_ ) {
       emit editorClosed(true); 
       OptimMainWindow* mw = qobject_cast<OptimMainWindow*> ( window() );
       mw->detachAll();
       event->accept();
       
     } 
     else {

       QEvent* e = new CloseCancelledEvent(); // (Eventstatic_cast<QEvent::Type>(2000));
       QCoreApplication::postEvent(this, e);
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
         ret = OptimMessageBox::warning(this, tr("OptiMX").toUtf8().data(),
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
     setWindowTitle("LATTICE EDITOR: "+ userFriendlyCurrentFile() + "[*]");
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
  Globals::preferences().editor_default_font.specs = font.toString();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::highlightCurrent()
{
  QTextEdit::ExtraSelection highlight;
  highlight.cursor = textCursor();
  highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
  highlight.format.setBackground( Qt::yellow );
  highlight.format.setForeground( Qt::blue );

  QList<QTextEdit::ExtraSelection> extras;
  extras << highlight;
  setExtraSelections( extras );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::highlightCurrentBlock(int nline)
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
  else if ( (modifiers == Qt::AltModifier) ) {
    event->ignore(); // Alt-Key combinations should be ignored by the text editor, but they are not
                     // this looks like a QPlainTextEdit bug. 
  }
  else {
    QPlainTextEdit::keyPressEvent( event);
  }  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::setCloseAllowed(bool enable)
{
  close_is_allowed_ = enable;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::focusInEvent(QFocusEvent *e)
{
  //std::cout << "focusInEvent objectid = " << (long) this << std::endl;
  //auto tc = textCursor();
  //std::cout << "tc.blockNumber() = " <<  tc.blockNumber() << std::endl;
  // ensureCursorVisible();
  QPlainTextEdit::focusInEvent(e); 
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::focusOutEvent(QFocusEvent *e)
{
  //std::cout << "focusOutEvent objectid = " << (long) this << std::endl;
  QPlainTextEdit::focusOutEvent(e); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    if (lineNumberArea_) {
      lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::updateLineNumberArea(QRect const& rect, int dy)
{
    if (dy)
        lineNumberArea_->scroll(0, dy);
    else
        lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimEditor::highlightCurrentLine()
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

void OptimEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea_);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
     while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea_->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int OptimEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
#if QT_VERSION < 0x050b00
    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
#else
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
#endif
    return space;
}

