/********************************************************************************
** Form generated from reading UI file 'energy_scale_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENERGY_SCALE_DIALOG_H
#define UI_ENERGY_SCALE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EnergyScaleDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *widget;
    QFormLayout *formLayout;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxEnergy;

    void setupUi(QDialog *EnergyScaleDialog)
    {
        if (EnergyScaleDialog->objectName().isEmpty())
            EnergyScaleDialog->setObjectName("EnergyScaleDialog");
        EnergyScaleDialog->resize(285, 100);
        EnergyScaleDialog->setMinimumSize(QSize(285, 100));
        EnergyScaleDialog->setMaximumSize(QSize(285, 100));
        buttonBox = new QDialogButtonBox(EnergyScaleDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(70, 60, 156, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        widget = new QWidget(EnergyScaleDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(60, 20, 206, 24));
        formLayout = new QFormLayout(widget);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        doubleSpinBoxEnergy = new QDoubleSpinBox(widget);
        doubleSpinBoxEnergy->setObjectName("doubleSpinBoxEnergy");
        doubleSpinBoxEnergy->setMaximum(1000000.000000000000000);
        doubleSpinBoxEnergy->setSingleStep(100.000000000000000);

        formLayout->setWidget(0, QFormLayout::FieldRole, doubleSpinBoxEnergy);


        retranslateUi(EnergyScaleDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, EnergyScaleDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, EnergyScaleDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(EnergyScaleDialog);
    } // setupUi

    void retranslateUi(QDialog *EnergyScaleDialog)
    {
        EnergyScaleDialog->setWindowTitle(QCoreApplication::translate("EnergyScaleDialog", "Rescale Energy", nullptr));
        label->setText(QCoreApplication::translate("EnergyScaleDialog", "New Energy [MeV]", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EnergyScaleDialog: public Ui_EnergyScaleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENERGY_SCALE_DIALOG_H
