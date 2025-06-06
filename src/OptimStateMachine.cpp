//  =================================================================
//
//  OptimStateMachine.cpp
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

#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimEditor.h>
#include <OptimTextEditor.h>
#include <OptimTrackerNew.h>
#include <OptimStateMachine.h>
#include <CustomEvents.h>

#include <QList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <OptimTextEditor.h>


OptimStateMachine::OptimStateMachine(OptimMainWindow* mw)
  :  QStateMachine(mw), mw_(mw)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool OptimStateMachine::event(QEvent* e)
{
 return QStateMachine::event(e);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OptimStateMachine::eventFilter(QObject* watched, QEvent* e)
{


   if (qobject_cast<QMdiSubWindow*>(watched)) { 
      if (e->type() == QEvent::Show ) {
        emit subWindowPresent(); 
      }
      if (e->type() == QEvent::Close ) {
	if  (mw_->mdiArea->subWindowList().count() == 1 ) emit noSubWindow(); 
      }
   }

   if (qobject_cast<QTextEdit*>(watched)) { 
     if (e->type() == QEvent::Show || (e->type() ==  CloseCancelledEvent::customtype )) {
        emit editorPresent(); 
        emit subWindowPresent(); 
      }
      if (e->type() == QEvent::Close ) {
       uint count = 0;
 	auto swlst = mw_->mdiArea->subWindowList();
	for ( auto it = swlst.begin(); it != swlst.end(); ++it) {
	  if ( qobject_cast<QTextEdit*>((*it)->widget())) ++count;
        }		  
	if (count == 1) emit noEditor();
      }
   }
	
   if (qobject_cast<OptimEditor*>(watched)) { 
     if (e->type() == QEvent::Show  || (e->type() ==  CloseCancelledEvent::customtype )) {
        emit latticeLoaded(); 
        emit subWindowPresent(); 
      }
      if (e->type() == QEvent::Close ) {
        emit noLattice(); 
      }
   }

   if (qobject_cast<OptimTextEditor*>(watched)) { 
     if (e->type() == QEvent::Show  || (e->type() ==  CloseCancelledEvent::customtype )) {
	emit textEditorLoaded(); 
        emit subWindowPresent(); 
     }
     if (e->type() == QEvent::Close ) {
       uint count = 0;
 	auto swlst = mw_->mdiArea->subWindowList();
	for ( auto it = swlst.begin(); it != swlst.end(); ++it) {
	  if ( qobject_cast<OptimTextEditor*>((*it)->widget())) ++count;
        }		  
	if (count == 1)  emit noTextEditor();
     }
   }

   if (qobject_cast<OptimTrackerNew*>(watched)) { 
     if (e->type() == QEvent::Show ) {
       emit trackerLoaded(); 
     }
     if (e->type() == QEvent::Close ) {
       emit noTracker(); 
     }
   }

   return QStateMachine::eventFilter(watched,e);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

