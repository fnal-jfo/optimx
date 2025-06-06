//  =================================================================
//
//  TrackingParametersDialog.cpp
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

#include <TrackingParametersDialog.h> 
#include <ui_tracking_parameters.h>  


  TrackParamStr::TrackParamStr() {};

  TrackParamStr::TrackParamStr( TrackParamStr const& o)
  :
    NTurn(o.NTurn),
    NElem(o.NElem),
    MatchCase(o.MatchCase),
    IncrementTurns(o.IncrementTurns),
    FastTracking(o.FastTracking),
    PrintResults(o.PrintResults)
  {
    strcpy(Filter, o.Filter);
  }
 

  TrackParamStr& TrackParamStr::operator=( TrackParamStr const& rhs)
  {
    if  (&rhs == this) return *this;
 
    NTurn          = rhs.NTurn;
    NElem          = rhs.NElem;
    MatchCase      = rhs.MatchCase;
    IncrementTurns = rhs.IncrementTurns;
    FastTracking   = rhs.FastTracking;
    PrintResults   = rhs.PrintResults;
    strcpy(Filter, rhs.Filter);
    return *this;
  } 


TrackingParametersDialog::TrackingParametersDialog( QWidget* parent)
  : QDialog(parent), ui_(new Ui::TrackingParametersDialog)
{
  ui_->setupUi(this);

  ui_->spinBoxNTurn->setRange(1,100000 );
  ui_->spinBoxNElm->setRange(-1,100000 );
  ui_->spinBoxNElm->setValue(data_.NElem  );;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackingParametersDialog::~TrackingParametersDialog()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackingParametersDialog::accept() 
{

  data_.NTurn          =  ui_->spinBoxNTurn->value();
  data_.NElem          =  ui_->spinBoxNElm->value();;
  data_.MatchCase      =  ui_->checkBoxMatchCase->isChecked();
  data_.IncrementTurns =  ui_->checkBoxIncrementTurn->isChecked();
  data_.FastTracking   =  ui_->checkBoxFastTracking->isChecked();
  data_.PrintResults   =  ui_->checkBoxPrintResults->isChecked();
  strcpy(data_.Filter, ui_->lineEditFilter->text().toUtf8().data());
 
  QDialog::accept();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackingParametersDialog::set() 
{
  ui_->spinBoxNTurn->setValue(data_.NTurn );
  ui_->spinBoxNElm->setValue(data_.NElem  );;
  ui_->checkBoxMatchCase->setChecked(data_.MatchCase );  
  ui_->checkBoxFastTracking->setChecked( data_.FastTracking );
  ui_->checkBoxPrintResults->setChecked( data_.PrintResults );
  ui_->lineEditFilter->setText(data_.Filter); 
  
}

