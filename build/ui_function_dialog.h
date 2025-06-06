/********************************************************************************
** Form generated from reading UI file 'function_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUNCTION_DIALOG_H
#define UI_FUNCTION_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FunctionDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditFilter;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QCheckBox *checkBoxStartSAtZero;
    QDoubleSpinBox *doubleSpinBoxStep;
    QSpinBox *spinBoxStart;
    QSpinBox *spinBoxStop;
    QWidget *widget;
    QGridLayout *gridLayout;
    QCheckBox *checkBoxMatchCase;
    QCheckBox *checkBoxPrintName;
    QCheckBox *checkBoxOmitFirst;
    QCheckBox *checkBoxPrintEnergy;

    void setupUi(QDialog *FunctionDialog)
    {
        if (FunctionDialog->objectName().isEmpty())
            FunctionDialog->setObjectName("FunctionDialog");
        FunctionDialog->resize(267, 331);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FunctionDialog->sizePolicy().hasHeightForWidth());
        FunctionDialog->setSizePolicy(sizePolicy);
        FunctionDialog->setMinimumSize(QSize(267, 331));
        FunctionDialog->setMaximumSize(QSize(267, 331));
        QFont font;
        font.setPointSize(8);
        FunctionDialog->setFont(font);
        buttonBox = new QDialogButtonBox(FunctionDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(40, 290, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEditFilter = new QLineEdit(FunctionDialog);
        lineEditFilter->setObjectName("lineEditFilter");
        lineEditFilter->setGeometry(QRect(88, 16, 166, 22));
        label = new QLabel(FunctionDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 91, 16));
        label_2 = new QLabel(FunctionDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 170, 71, 16));
        label_3 = new QLabel(FunctionDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 200, 121, 20));
        label_4 = new QLabel(FunctionDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 230, 111, 16));
        checkBoxStartSAtZero = new QCheckBox(FunctionDialog);
        checkBoxStartSAtZero->setObjectName("checkBoxStartSAtZero");
        checkBoxStartSAtZero->setGeometry(QRect(10, 260, 221, 21));
        doubleSpinBoxStep = new QDoubleSpinBox(FunctionDialog);
        doubleSpinBoxStep->setObjectName("doubleSpinBoxStep");
        doubleSpinBoxStep->setGeometry(QRect(130, 170, 111, 22));
        spinBoxStart = new QSpinBox(FunctionDialog);
        spinBoxStart->setObjectName("spinBoxStart");
        spinBoxStart->setGeometry(QRect(130, 200, 85, 22));
        spinBoxStop = new QSpinBox(FunctionDialog);
        spinBoxStop->setObjectName("spinBoxStop");
        spinBoxStop->setGeometry(QRect(130, 230, 82, 22));
        widget = new QWidget(FunctionDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(10, 50, 175, 100));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        checkBoxMatchCase = new QCheckBox(widget);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");

        gridLayout->addWidget(checkBoxMatchCase, 0, 0, 1, 1);

        checkBoxPrintName = new QCheckBox(widget);
        checkBoxPrintName->setObjectName("checkBoxPrintName");

        gridLayout->addWidget(checkBoxPrintName, 1, 0, 1, 1);

        checkBoxOmitFirst = new QCheckBox(widget);
        checkBoxOmitFirst->setObjectName("checkBoxOmitFirst");

        gridLayout->addWidget(checkBoxOmitFirst, 2, 0, 1, 1);

        checkBoxPrintEnergy = new QCheckBox(widget);
        checkBoxPrintEnergy->setObjectName("checkBoxPrintEnergy");

        gridLayout->addWidget(checkBoxPrintEnergy, 3, 0, 1, 1);


        retranslateUi(FunctionDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, FunctionDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, FunctionDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(FunctionDialog);
    } // setupUi

    void retranslateUi(QDialog *FunctionDialog)
    {
        FunctionDialog->setWindowTitle(QCoreApplication::translate("FunctionDialog", "Function Dialog", nullptr));
        label->setText(QCoreApplication::translate("FunctionDialog", "Filter [*,%,...]", nullptr));
        label_2->setText(QCoreApplication::translate("FunctionDialog", "Step [cm]", nullptr));
        label_3->setText(QCoreApplication::translate("FunctionDialog", "Start from element", nullptr));
        label_4->setText(QCoreApplication::translate("FunctionDialog", "Stop at element", nullptr));
        checkBoxStartSAtZero->setText(QCoreApplication::translate("FunctionDialog", "Start s-coordinates from zero", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("FunctionDialog", "match case", nullptr));
        checkBoxPrintName->setText(QCoreApplication::translate("FunctionDialog", "Print element name", nullptr));
        checkBoxOmitFirst->setText(QCoreApplication::translate("FunctionDialog", "Omit 1st  character of name", nullptr));
        checkBoxPrintEnergy->setText(QCoreApplication::translate("FunctionDialog", "Print Energy", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FunctionDialog: public Ui_FunctionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUNCTION_DIALOG_H
