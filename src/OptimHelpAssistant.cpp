//  =================================================================
//
//  OptimHelpAssistant.cpp
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
#if 0

#include <iostream>
#include <QtCore/QByteArray>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QProcess>

#include <QtGui/QMessageBox>
#include <OptimHelpAssistant.h>

OptimHelpAssistant::OptimHelpAssistant()
: proc_(0)
{}

OptimHelpAssistant::~OptimHelpAssistant()
{
    if (proc_ && proc_->state() == QProcess::Running) {
        proc_->terminate();
        proc_->waitForFinished(3000);
    }
    delete proc_;
}

void OptimHelpAssistant::showDocumentation(QString const& page)
{
    if (!start())
        return;

    QByteArray ba("SetSource ");
    ba.append("qthelp://gov.fnal.apps.optim/doc/");
    
    proc_->write(ba + page.toLocal8Bit() + '\n');
}


bool OptimHelpAssistant::start()
{
    if (!proc_)
        proc_ = new QProcess();

    if (proc_->state() != QProcess::Running) {
        QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#if !defined(Q_OS_MAC)
        app += QLatin1String("assistant");
#else
        app += QLatin1String("OptimHelpAssistant.app/Contents/MacOS/OptimHelpAssistant");    
#endif

        QStringList args;
        args << QLatin1String("-collectionFile")
            << QLibraryInfo::location(QLibraryInfo::ExamplesPath)
            + QLatin1String("/help/OptiM/documentation/optimx.qhc")
            << QLatin1String("-enableRemoteControl");

	args.clear(); 
        proc_->start(app, args);

        if (!proc_->waitForStarted()) {
            OptimCriticalMessage(0, QObject::tr("OptiM"),
                QObject::tr("Unable to launch Qt HelpAssistant (%1)").arg(app));
            return false;
        }    
    }
    std::cout << "ok" << std::endl;
    return true;
}
#endif

