/********************************************************************************
** Form generated from reading UI file 'space_charge_control_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPACE_CHARGE_CONTROL_DIALOG_H
#define UI_SPACE_CHARGE_CONTROL_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_SpaceChargeControlDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxStepSize;
    QGroupBox *groupBox;
    QLabel *label_3;
    QSpinBox *spinBoxMaxIterations;
    QLabel *label_4;
    QLabel *label_5;
    QSpinBox *spinBoxCurrentSteps;
    QCheckBox *checkBoxUse4DBlock;
    QComboBox *comboBoxAlgo;
    QLabel *label_2;
    ScientificDoubleSpinBox *techSpinBoxEps;

    void setupUi(QDialog *SpaceChargeControlDialog)
    {
        if (SpaceChargeControlDialog->objectName().isEmpty())
            SpaceChargeControlDialog->setObjectName("SpaceChargeControlDialog");
        SpaceChargeControlDialog->resize(352, 301);
        SpaceChargeControlDialog->setMinimumSize(QSize(285, 100));
        SpaceChargeControlDialog->setMaximumSize(QSize(100000, 100000));
        buttonBox = new QDialogButtonBox(SpaceChargeControlDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(100, 260, 156, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(SpaceChargeControlDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 20, 151, 16));
        doubleSpinBoxStepSize = new QDoubleSpinBox(SpaceChargeControlDialog);
        doubleSpinBoxStepSize->setObjectName("doubleSpinBoxStepSize");
        doubleSpinBoxStepSize->setGeometry(QRect(190, 20, 59, 22));
        doubleSpinBoxStepSize->setDecimals(3);
        doubleSpinBoxStepSize->setSingleStep(0.100000000000000);
        groupBox = new QGroupBox(SpaceChargeControlDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 50, 321, 201));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 30, 151, 16));
        spinBoxMaxIterations = new QSpinBox(groupBox);
        spinBoxMaxIterations->setObjectName("spinBoxMaxIterations");
        spinBoxMaxIterations->setGeometry(QRect(200, 30, 111, 22));
        spinBoxMaxIterations->setMinimum(1);
        spinBoxMaxIterations->setMaximum(1000000);
        spinBoxMaxIterations->setSingleStep(100);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(20, 90, 151, 21));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(20, 60, 181, 16));
        spinBoxCurrentSteps = new QSpinBox(groupBox);
        spinBoxCurrentSteps->setObjectName("spinBoxCurrentSteps");
        spinBoxCurrentSteps->setGeometry(QRect(200, 60, 111, 22));
        spinBoxCurrentSteps->setMinimum(1);
        spinBoxCurrentSteps->setMaximum(1000000);
        spinBoxCurrentSteps->setSingleStep(100);
        checkBoxUse4DBlock = new QCheckBox(groupBox);
        checkBoxUse4DBlock->setObjectName("checkBoxUse4DBlock");
        checkBoxUse4DBlock->setGeometry(QRect(20, 160, 251, 31));
        comboBoxAlgo = new QComboBox(groupBox);
        comboBoxAlgo->setObjectName("comboBoxAlgo");
        comboBoxAlgo->setGeometry(QRect(170, 120, 141, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 120, 71, 16));
        techSpinBoxEps = new ScientificDoubleSpinBox(groupBox);
        techSpinBoxEps->setObjectName("techSpinBoxEps");
        techSpinBoxEps->setGeometry(QRect(200, 90, 111, 22));

        retranslateUi(SpaceChargeControlDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, SpaceChargeControlDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, SpaceChargeControlDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(SpaceChargeControlDialog);
    } // setupUi

    void retranslateUi(QDialog *SpaceChargeControlDialog)
    {
        SpaceChargeControlDialog->setWindowTitle(QCoreApplication::translate("SpaceChargeControlDialog", "Space Charge", nullptr));
        label->setText(QCoreApplication::translate("SpaceChargeControlDialog", "Integration Step Size[cm]", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SpaceChargeControlDialog", "Perodic Lattice Functions Solver", nullptr));
        label_3->setText(QCoreApplication::translate("SpaceChargeControlDialog", "Maximum no of iterations", nullptr));
        label_4->setText(QCoreApplication::translate("SpaceChargeControlDialog", "Residual norm (absolute)", nullptr));
        label_5->setText(QCoreApplication::translate("SpaceChargeControlDialog", "Ramp beam current in n steps", nullptr));
        checkBoxUse4DBlock->setText(QCoreApplication::translate("SpaceChargeControlDialog", "Use current  4D beta block \n"
" values as initial guess", nullptr));
        label_2->setText(QCoreApplication::translate("SpaceChargeControlDialog", "Algorithm", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SpaceChargeControlDialog: public Ui_SpaceChargeControlDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPACE_CHARGE_CONTROL_DIALOG_H
