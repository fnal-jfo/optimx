/********************************************************************************
** Form generated from reading UI file 'tracking_parameters.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKING_PARAMETERS_H
#define UI_TRACKING_PARAMETERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_TrackingParametersDialog
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBoxFastTracking;
    QCheckBox *checkBoxPrintResults;
    QGroupBox *groupBox;
    QLabel *label;
    QSpinBox *spinBoxNTurn;
    QLabel *label_2;
    QSpinBox *spinBoxNElm;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *lineEditFilter;
    QCheckBox *checkBoxMatchCase;
    QCheckBox *checkBoxIncrementTurn;

    void setupUi(QDialog *TrackingParametersDialog)
    {
        if (TrackingParametersDialog->objectName().isEmpty())
            TrackingParametersDialog->setObjectName("TrackingParametersDialog");
        TrackingParametersDialog->resize(446, 344);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TrackingParametersDialog->sizePolicy().hasHeightForWidth());
        TrackingParametersDialog->setSizePolicy(sizePolicy);
        TrackingParametersDialog->setMinimumSize(QSize(446, 344));
        TrackingParametersDialog->setMaximumSize(QSize(446, 344));
        buttonBox = new QDialogButtonBox(TrackingParametersDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(130, 300, 171, 31));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        checkBoxFastTracking = new QCheckBox(TrackingParametersDialog);
        checkBoxFastTracking->setObjectName("checkBoxFastTracking");
        checkBoxFastTracking->setGeometry(QRect(40, 240, 241, 21));
        checkBoxPrintResults = new QCheckBox(TrackingParametersDialog);
        checkBoxPrintResults->setObjectName("checkBoxPrintResults");
        checkBoxPrintResults->setGeometry(QRect(40, 270, 271, 21));
        groupBox = new QGroupBox(TrackingParametersDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(30, 10, 401, 211));
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 40, 101, 16));
        spinBoxNTurn = new QSpinBox(groupBox);
        spinBoxNTurn->setObjectName("spinBoxNTurn");
        spinBoxNTurn->setGeometry(QRect(150, 40, 52, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 80, 131, 16));
        spinBoxNElm = new QSpinBox(groupBox);
        spinBoxNElm->setObjectName("spinBoxNElm");
        spinBoxNElm->setGeometry(QRect(160, 80, 52, 22));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(220, 70, 161, 16));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(220, 90, 151, 16));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(20, 120, 55, 13));
        lineEditFilter = new QLineEdit(groupBox);
        lineEditFilter->setObjectName("lineEditFilter");
        lineEditFilter->setGeometry(QRect(100, 110, 113, 21));
        checkBoxMatchCase = new QCheckBox(groupBox);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(20, 140, 111, 21));
        checkBoxIncrementTurn = new QCheckBox(groupBox);
        checkBoxIncrementTurn->setObjectName("checkBoxIncrementTurn");
        checkBoxIncrementTurn->setGeometry(QRect(20, 170, 291, 21));

        retranslateUi(TrackingParametersDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TrackingParametersDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TrackingParametersDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TrackingParametersDialog);
    } // setupUi

    void retranslateUi(QDialog *TrackingParametersDialog)
    {
        TrackingParametersDialog->setWindowTitle(QCoreApplication::translate("TrackingParametersDialog", "Tracking Parameters", nullptr));
        checkBoxFastTracking->setText(QCoreApplication::translate("TrackingParametersDialog", "  Use fast tracking", nullptr));
        checkBoxPrintResults->setText(QCoreApplication::translate("TrackingParametersDialog", "  Display tracking results in a text window", nullptr));
        groupBox->setTitle(QCoreApplication::translate("TrackingParametersDialog", "Multi-turn Tracking", nullptr));
        label->setText(QCoreApplication::translate("TrackingParametersDialog", "Number of Turns", nullptr));
        label_2->setText(QCoreApplication::translate("TrackingParametersDialog", "Output at Element no ", nullptr));
        label_3->setText(QCoreApplication::translate("TrackingParametersDialog", "[ 0: output at lattice end ]", nullptr));
        label_4->setText(QCoreApplication::translate("TrackingParametersDialog", "[-1: multi-element output ]", nullptr));
        label_5->setText(QCoreApplication::translate("TrackingParametersDialog", "Filter", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("TrackingParametersDialog", "Match Case", nullptr));
        checkBoxIncrementTurn->setText(QCoreApplication::translate("TrackingParametersDialog", "Increment $_Nturn and re-parse for each turn", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackingParametersDialog: public Ui_TrackingParametersDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACKING_PARAMETERS_H
