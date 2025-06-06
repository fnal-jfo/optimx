//  =================================================================
//
//  OptimEditor.h
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

 #ifndef OPTIMEDITOR_H
 #define OPTIMEDITOR_H

 #include <QPlainTextEdit>
 #include <QFileInfo>
 #include <QFont>
 #include <QPoint>
 #include <cstdarg>

 class QFocusEvent;
 class QCloseEvent;

 class OptimEditor : public QPlainTextEdit {

     Q_OBJECT

 public:
     OptimEditor( QWidget* parent=0);
    ~OptimEditor();

     void newFile();
     bool loadFile(const QString &fileName);
     bool save();
     bool saveAs();
     bool saveFile(const QString &fileName);
     QString userFriendlyCurrentFile();
     QString currentFile() { return curFile; }

     QString getLine(int line) const; // compatibility 
     void    insert( char* str);      // compatibility

     static void setDefaultFont( QFont const& font);   

     QFileInfo const& fileInfo() const;

     void highlightCurrent();
     void highlightCurrentBlock(int lineno=-1);
     void clearHighlightedBlocks();
     void  setCloseAllowed( bool enable);

     void lineNumberAreaPaintEvent(QPaintEvent *event);
     int lineNumberAreaWidth();

 signals:

     void editorClosed(bool);    
     void increment(int);
     void cursorPosition(QPoint pos); 
 
 protected slots:

     void documentWasModified();

 protected:
     void resizeEvent(QResizeEvent *event)    override;
     void keyPressEvent ( QKeyEvent * event ) override;
     void closeEvent(QCloseEvent *event)      override;
     void focusInEvent(QFocusEvent *e)        override;
     void focusOutEvent(QFocusEvent *e)       override;

 private slots:
  
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

     
 private:

     bool    maybeSave();
     void    setCurrentFile(const QString &fileName);
     QString strippedName(const QString &fullFileName);

     QString curFile;
     bool    isUntitled;

     void    iterateText() const;

      bool      close_is_allowed_;
      QFileInfo fileinfo_; 

     static QFont default_font_;     

     QWidget* lineNumberArea_;
 };

class LineNumberArea : public QWidget {

 public:
 LineNumberArea(OptimEditor* editor)
   : QWidget(editor), editor_(editor) {}

    QSize sizeHint() const override {
        return QSize(editor_->lineNumberAreaWidth(), 0);
    }

 protected:
    void paintEvent(QPaintEvent *event) override {
        editor_->lineNumberAreaPaintEvent(event);
    }

 private:
    OptimEditor* editor_;
};

#endif //  OPTIMEDITOR_H


