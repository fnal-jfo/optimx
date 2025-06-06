/********************************************************************************
** Form generated from reading UI file 'general_preferences_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENERAL_PREFERENCES_DIALOG_H
#define UI_GENERAL_PREFERENCES_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include "ScientificDoubleSpinBox.h"
#include "UIntSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_GeneralPreferencesDialog
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBoxIgnoreAutoRepeat;
    QCheckBox *checkBoxQuadFringeOff;
    ScientificDoubleSpinBox *doubleSpinBoxPhaseThreshold;
    QLabel *label;
    QGroupBox *randomGenGroupBox;
    QLabel *label_2;
    UIntSpinBox *seedSpinBox;
    QCheckBox *userSeedCheckBox;
    QCheckBox *checkBoxParallelTracking;

    void setupUi(QDialog *GeneralPreferencesDialog)
    {
        if (GeneralPreferencesDialog->objectName().isEmpty())
            GeneralPreferencesDialog->setObjectName("GeneralPreferencesDialog");
        GeneralPreferencesDialog->resize(549, 419);
        buttonBox = new QDialogButtonBox(GeneralPreferencesDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(195, 326, 156, 25));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        checkBoxIgnoreAutoRepeat = new QCheckBox(GeneralPreferencesDialog);
        checkBoxIgnoreAutoRepeat->setObjectName("checkBoxIgnoreAutoRepeat");
        checkBoxIgnoreAutoRepeat->setGeometry(QRect(60, 20, 361, 20));
        checkBoxQuadFringeOff = new QCheckBox(GeneralPreferencesDialog);
        checkBoxQuadFringeOff->setObjectName("checkBoxQuadFringeOff");
        checkBoxQuadFringeOff->setGeometry(QRect(60, 60, 361, 20));
        doubleSpinBoxPhaseThreshold = new ScientificDoubleSpinBox(GeneralPreferencesDialog);
        doubleSpinBoxPhaseThreshold->setObjectName("doubleSpinBoxPhaseThreshold");
        doubleSpinBoxPhaseThreshold->setGeometry(QRect(290, 100, 151, 29));
        doubleSpinBoxPhaseThreshold->setDecimals(8);
        doubleSpinBoxPhaseThreshold->setMaximum(0.010000000000000);
        doubleSpinBoxPhaseThreshold->setSingleStep(0.000001000000000);
        label = new QLabel(GeneralPreferencesDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(60, 110, 211, 16));
        randomGenGroupBox = new QGroupBox(GeneralPreferencesDialog);
        randomGenGroupBox->setObjectName("randomGenGroupBox");
        randomGenGroupBox->setGeometry(QRect(52, 200, 421, 111));
        label_2 = new QLabel(randomGenGroupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 70, 161, 16));
        seedSpinBox = new UIntSpinBox(randomGenGroupBox);
        seedSpinBox->setObjectName("seedSpinBox");
        seedSpinBox->setGeometry(QRect(260, 60, 151, 29));
        seedSpinBox->setMaximum(999999999);
        userSeedCheckBox = new QCheckBox(randomGenGroupBox);
        userSeedCheckBox->setObjectName("userSeedCheckBox");
        userSeedCheckBox->setGeometry(QRect(10, 30, 171, 23));
        checkBoxParallelTracking = new QCheckBox(GeneralPreferencesDialog);
        checkBoxParallelTracking->setObjectName("checkBoxParallelTracking");
        checkBoxParallelTracking->setGeometry(QRect(62, 159, 354, 20));

        retranslateUi(GeneralPreferencesDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, GeneralPreferencesDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, GeneralPreferencesDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(GeneralPreferencesDialog);
    } // setupUi

    void retranslateUi(QDialog *GeneralPreferencesDialog)
    {
        GeneralPreferencesDialog->setWindowTitle(QCoreApplication::translate("GeneralPreferencesDialog", "Computation Preferences", nullptr));
        checkBoxIgnoreAutoRepeat->setText(QCoreApplication::translate("GeneralPreferencesDialog", "Ignore auto-repeat for CTRL-Left/Right arrow recalc", nullptr));
        checkBoxQuadFringeOff->setText(QCoreApplication::translate("GeneralPreferencesDialog", "Turn off quad fringe field aberrations.  ", nullptr));
        label->setText(QCoreApplication::translate("GeneralPreferencesDialog", "Negative Phase Advance Threshold ", nullptr));
        randomGenGroupBox->setTitle(QCoreApplication::translate("GeneralPreferencesDialog", "Random Number Generation", nullptr));
        label_2->setText(QCoreApplication::translate("GeneralPreferencesDialog", "Random Generator Seed", nullptr));
        userSeedCheckBox->setText(QCoreApplication::translate("GeneralPreferencesDialog", "User-Defined Seed", nullptr));
        checkBoxParallelTracking->setText(QCoreApplication::translate("GeneralPreferencesDialog", "Enable Parallel (Multi-Threaded) Tracking", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GeneralPreferencesDialog: public Ui_GeneralPreferencesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERAL_PREFERENCES_DIALOG_H
