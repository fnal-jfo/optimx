//  =================================================================
//
//  OptimMessages.h
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

#ifndef  OPTIMMESSAGES_H
#define  OPTIMMESSAGES_H

#include <QMessageBox>

#ifndef Q_NULLPTR
#define Q_NULLPTR  nullptr
#endif

class OptimMessageBox : public QMessageBox {

 public:


  OptimMessageBox(QWidget *parent = Q_NULLPTR);

  OptimMessageBox(Icon icon, const QString &title, const QString &text, StandardButtons buttons = NoButton,
		  QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);


  static StandardButton warning( QWidget* parent, char const* title, char const* text, 
				 QMessageBox::StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
};


extern  QMessageBox::StandardButton OptimCriticalMessage( QWidget* parent, char const* title, char const* text, 
                                                  QMessageBox::StandardButtons buttons=QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton); 

extern  QMessageBox::StandardButton OptimInformationMessage(QWidget* parent, char const* title, char const* text, 
                                                    QMessageBox::StandardButtons buttons=QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );


extern  int OptimQuestionMessage(QWidget* parent, char const* title, char const* text, 
						 QMessageBox::StandardButtons buttons=QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );


#endif // OPTIMMESSAGES_H
