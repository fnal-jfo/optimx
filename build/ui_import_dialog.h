/********************************************************************************
** Form generated from reading UI file 'import_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMPORT_DIALOG_H
#define UI_IMPORT_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_ImportDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditOutputFile;
    QPushButton *pushButtonOutputFile;
    QLabel *label_4;
    QComboBox *comboBoxParticles;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label;
    ScientificDoubleSpinBox *techSpinBoxTotEnergy;
    QLabel *label_3;
    ScientificDoubleSpinBox *techSpinBoxKinEnergy;
    QLabel *label_2;
    QLineEdit *lineEditMass;

    void setupUi(QDialog *ImportDialog)
    {
        if (ImportDialog->objectName().isEmpty())
            ImportDialog->setObjectName("ImportDialog");
        ImportDialog->resize(466, 225);
        buttonBox = new QDialogButtonBox(ImportDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(160, 190, 157, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEditOutputFile = new QLineEdit(ImportDialog);
        lineEditOutputFile->setObjectName("lineEditOutputFile");
        lineEditOutputFile->setGeometry(QRect(90, 20, 291, 21));
        pushButtonOutputFile = new QPushButton(ImportDialog);
        pushButtonOutputFile->setObjectName("pushButtonOutputFile");
        pushButtonOutputFile->setGeometry(QRect(400, 20, 31, 22));
        label_4 = new QLabel(ImportDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 20, 81, 16));
        comboBoxParticles = new QComboBox(ImportDialog);
        comboBoxParticles->setObjectName("comboBoxParticles");
        comboBoxParticles->setGeometry(QRect(360, 130, 85, 22));
        widget = new QWidget(ImportDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(41, 75, 299, 76));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        techSpinBoxTotEnergy = new ScientificDoubleSpinBox(widget);
        techSpinBoxTotEnergy->setObjectName("techSpinBoxTotEnergy");

        gridLayout->addWidget(techSpinBoxTotEnergy, 0, 1, 1, 1);

        label_3 = new QLabel(widget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        techSpinBoxKinEnergy = new ScientificDoubleSpinBox(widget);
        techSpinBoxKinEnergy->setObjectName("techSpinBoxKinEnergy");

        gridLayout->addWidget(techSpinBoxKinEnergy, 1, 1, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        lineEditMass = new QLineEdit(widget);
        lineEditMass->setObjectName("lineEditMass");

        gridLayout->addWidget(lineEditMass, 2, 1, 1, 1);


        retranslateUi(ImportDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ImportDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ImportDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButtonOutputFile, SIGNAL(clicked()), ImportDialog, SLOT(setOutputFilename()));
        QObject::connect(comboBoxParticles, SIGNAL(currentIndexChanged(QString)), ImportDialog, SLOT(setParticleMass(QString)));
        QObject::connect(lineEditMass, SIGNAL(textChanged(QString)), ImportDialog, SLOT(updatedMass(QString)));
        QObject::connect(techSpinBoxTotEnergy, SIGNAL(valueChanged(double)), ImportDialog, SLOT(updatedTotEnergy(double)));
        QObject::connect(techSpinBoxKinEnergy, SIGNAL(valueChanged(double)), ImportDialog, SLOT(updatedKinEnergy(double)));

        QMetaObject::connectSlotsByName(ImportDialog);
    } // setupUi

    void retranslateUi(QDialog *ImportDialog)
    {
        ImportDialog->setWindowTitle(QCoreApplication::translate("ImportDialog", "Import Lattice File", nullptr));
        pushButtonOutputFile->setText(QCoreApplication::translate("ImportDialog", "...", nullptr));
        label_4->setText(QCoreApplication::translate("ImportDialog", "Output File", nullptr));
        label->setText(QCoreApplication::translate("ImportDialog", "Particle Total  Energy [MeV]", nullptr));
        label_3->setText(QCoreApplication::translate("ImportDialog", "Particle Kinetic Energy [MeV]", nullptr));
        label_2->setText(QCoreApplication::translate("ImportDialog", "Particle Mass [MeV/c**2]", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImportDialog: public Ui_ImportDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMPORT_DIALOG_H
