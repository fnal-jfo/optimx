/********************************************************************************
** Form generated from reading UI file 'compress_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPRESS_DIALOG_H
#define UI_COMPRESS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CompressDialog
{
public:
    QDialogButtonBox *buttonBox;
    QSpinBox *spinBoxOffset;
    QLabel *label;
    QLineEdit *lineEditInputFile;
    QLineEdit *lineEditOutputFile;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *pushButtonInput;
    QPushButton *pushButtonOutput;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBoxCorrectors;
    QCheckBox *checkBoxSextupoles;
    QCheckBox *checkBoxSolenoids;
    QCheckBox *checkBoxInstruments;
    QCheckBox *checkBoxRemoveApertures;
    QCheckBox *checkBoxRemoveWField;
    QCheckBox *checkBoxRemoveScatter;
    QCheckBox *checkBoxRemoveVars;

    void setupUi(QDialog *CompressDialog)
    {
        if (CompressDialog->objectName().isEmpty())
            CompressDialog->setObjectName("CompressDialog");
        CompressDialog->resize(431, 460);
        buttonBox = new QDialogButtonBox(CompressDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(153, 414, 174, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        spinBoxOffset = new QSpinBox(CompressDialog);
        spinBoxOffset->setObjectName("spinBoxOffset");
        spinBoxOffset->setGeometry(QRect(188, 89, 111, 22));
        label = new QLabel(CompressDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 90, 131, 16));
        lineEditInputFile = new QLineEdit(CompressDialog);
        lineEditInputFile->setObjectName("lineEditInputFile");
        lineEditInputFile->setGeometry(QRect(120, 20, 231, 21));
        lineEditOutputFile = new QLineEdit(CompressDialog);
        lineEditOutputFile->setObjectName("lineEditOutputFile");
        lineEditOutputFile->setGeometry(QRect(120, 50, 231, 21));
        label_2 = new QLabel(CompressDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 20, 81, 16));
        label_3 = new QLabel(CompressDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(30, 50, 91, 16));
        pushButtonInput = new QPushButton(CompressDialog);
        pushButtonInput->setObjectName("pushButtonInput");
        pushButtonInput->setGeometry(QRect(360, 20, 31, 22));
        pushButtonOutput = new QPushButton(CompressDialog);
        pushButtonOutput->setObjectName("pushButtonOutput");
        pushButtonOutput->setGeometry(QRect(360, 50, 31, 22));
        widget = new QWidget(CompressDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(41, 129, 255, 268));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBoxCorrectors = new QCheckBox(widget);
        checkBoxCorrectors->setObjectName("checkBoxCorrectors");

        verticalLayout->addWidget(checkBoxCorrectors);

        checkBoxSextupoles = new QCheckBox(widget);
        checkBoxSextupoles->setObjectName("checkBoxSextupoles");

        verticalLayout->addWidget(checkBoxSextupoles);

        checkBoxSolenoids = new QCheckBox(widget);
        checkBoxSolenoids->setObjectName("checkBoxSolenoids");

        verticalLayout->addWidget(checkBoxSolenoids);

        checkBoxInstruments = new QCheckBox(widget);
        checkBoxInstruments->setObjectName("checkBoxInstruments");

        verticalLayout->addWidget(checkBoxInstruments);

        checkBoxRemoveApertures = new QCheckBox(widget);
        checkBoxRemoveApertures->setObjectName("checkBoxRemoveApertures");

        verticalLayout->addWidget(checkBoxRemoveApertures);

        checkBoxRemoveWField = new QCheckBox(widget);
        checkBoxRemoveWField->setObjectName("checkBoxRemoveWField");

        verticalLayout->addWidget(checkBoxRemoveWField);

        checkBoxRemoveScatter = new QCheckBox(widget);
        checkBoxRemoveScatter->setObjectName("checkBoxRemoveScatter");

        verticalLayout->addWidget(checkBoxRemoveScatter);

        checkBoxRemoveVars = new QCheckBox(widget);
        checkBoxRemoveVars->setObjectName("checkBoxRemoveVars");

        verticalLayout->addWidget(checkBoxRemoveVars);


        retranslateUi(CompressDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, CompressDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, CompressDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButtonInput, SIGNAL(clicked()), CompressDialog, SLOT(selectInputFile()));
        QObject::connect(pushButtonOutput, SIGNAL(clicked()), CompressDialog, SLOT(selectOutputFile()));

        QMetaObject::connectSlotsByName(CompressDialog);
    } // setupUi

    void retranslateUi(QDialog *CompressDialog)
    {
        CompressDialog->setWindowTitle(QCoreApplication::translate("CompressDialog", "Compress", nullptr));
        label->setText(QCoreApplication::translate("CompressDialog", "Offset for drift indexing", nullptr));
        label_2->setText(QCoreApplication::translate("CompressDialog", "Input file", nullptr));
        label_3->setText(QCoreApplication::translate("CompressDialog", "Output file", nullptr));
        pushButtonInput->setText(QCoreApplication::translate("CompressDialog", "...", nullptr));
        pushButtonOutput->setText(QCoreApplication::translate("CompressDialog", "...", nullptr));
        checkBoxCorrectors->setText(QCoreApplication::translate("CompressDialog", "Remove Correctors", nullptr));
        checkBoxSextupoles->setText(QCoreApplication::translate("CompressDialog", "Remove Sextupoles", nullptr));
        checkBoxSolenoids->setText(QCoreApplication::translate("CompressDialog", "Remove Solenoids", nullptr));
        checkBoxInstruments->setText(QCoreApplication::translate("CompressDialog", "Remove Instruments", nullptr));
        checkBoxRemoveApertures->setText(QCoreApplication::translate("CompressDialog", "Remove Apertures", nullptr));
        checkBoxRemoveWField->setText(QCoreApplication::translate("CompressDialog", "Remove Wakefield Elements", nullptr));
        checkBoxRemoveScatter->setText(QCoreApplication::translate("CompressDialog", "Remove Scattering Elements", nullptr));
        checkBoxRemoveVars->setText(QCoreApplication::translate("CompressDialog", "Remove variables from element definitions", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CompressDialog: public Ui_CompressDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPRESS_DIALOG_H
