/********************************************************************************
** Form generated from reading UI file 'fit_control_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FIT_CONTROL_DIALOG_H
#define UI_FIT_CONTROL_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_FitControlDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_6;
    QRadioButton *radioButtonFullTune;
    QRadioButton *radioButtonFractionalTune;

    void setupUi(QDialog *FitControlDialog)
    {
        if (FitControlDialog->objectName().isEmpty())
            FitControlDialog->setObjectName("FitControlDialog");
        FitControlDialog->resize(355, 231);
        buttonBox = new QDialogButtonBox(FitControlDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(90, 140, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox_6 = new QGroupBox(FitControlDialog);
        groupBox_6->setObjectName("groupBox_6");
        groupBox_6->setGeometry(QRect(60, 20, 251, 91));
        radioButtonFullTune = new QRadioButton(groupBox_6);
        radioButtonFullTune->setObjectName("radioButtonFullTune");
        radioButtonFullTune->setGeometry(QRect(10, 30, 191, 21));
        radioButtonFractionalTune = new QRadioButton(groupBox_6);
        radioButtonFractionalTune->setObjectName("radioButtonFractionalTune");
        radioButtonFractionalTune->setGeometry(QRect(10, 60, 191, 21));

        retranslateUi(FitControlDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, FitControlDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, FitControlDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(FitControlDialog);
    } // setupUi

    void retranslateUi(QDialog *FitControlDialog)
    {
        FitControlDialog->setWindowTitle(QCoreApplication::translate("FitControlDialog", "Dialog", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("FitControlDialog", "Tune / Phase advance constraints on", nullptr));
        radioButtonFullTune->setText(QCoreApplication::translate("FitControlDialog", "   In&teger + fractional part", nullptr));
        radioButtonFractionalTune->setText(QCoreApplication::translate("FitControlDialog", "  Fractiona&l part only", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FitControlDialog: public Ui_FitControlDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FIT_CONTROL_DIALOG_H
