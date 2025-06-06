//  =================================================================
//
//  OptimPrint.cpp
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
#include <memory>
#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimPlot.h>
#include <OptimTuneDiagram.h>
#include <OptimTrackerNew.h>
#include <TrackerPlot.h>
#include <QMdiSubWindow>
#include <QPlainTextEdit>
#include <QMdiArea>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include <QwtPlotRenderer>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSetupPrinter()
{

  QPrintDialog printDialog(&printer_,0); 
  if (printDialog.exec() == QDialog::Accepted) {
   print(&printer_); 
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::cmdPrintPreview() 
{

  std::unique_ptr<QPrintPreviewDialog> preview( new QPrintPreviewDialog( &printer_, this ));
  connect( preview.get(), SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)) );
  preview->exec();
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdPrint() 
{
  cmdSetupPrinter();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::print(QPrinter* printer) 
{
 
   QwtPlotRenderer renderer;

   QPlainTextEdit*     editor  = 0;
   OptimPlot*          plot    = 0;
   OptimTuneDiagram*   diagram = 0;
   OptimTrackerNew*    tracker = 0;
   QMdiSubWindow*      sw      = 0;

   if ( (sw= mdiArea->currentSubWindow()) ) {
     QWidget* w = mdiArea->currentSubWindow()->widget();  
     if( (editor = qobject_cast<QPlainTextEdit *>(w)) ) { 
       editor->document()->print(printer);
       return;
     }

     if ( (plot = qobject_cast<OptimPlot *>(w)) )  { 
       plot->setTitle( sw->windowTitle());
       renderer.renderTo( plot, *printer);
       plot->setTitle("");
       return; 
     }

     if ( (diagram = qobject_cast<OptimTuneDiagram *>(w)) )  { 
       renderer.renderTo( diagram, *printer);
       return; 
     }

     if ( (tracker = qobject_cast<OptimTrackerNew*>(w)) )  { 
       //renderer.renderTo( tracker->plot6_, *printer);
       return; 
     }
   }
   

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
