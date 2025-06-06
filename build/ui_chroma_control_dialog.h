/********************************************************************************
** Form generated from reading UI file 'chroma_control_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHROMA_CONTROL_DIALOG_H
#define UI_CHROMA_CONTROL_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_ChromaControlDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBoxOrbitClosure;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLabel *label;
    ScientificDoubleSpinBox *techSpinBoxStepSize;
    QSpinBox *spinBoxNsamples;
    QRadioButton *radioButtonRing;
    QRadioButton *radioButtonLine;

    void setupUi(QDialog *ChromaControlDialog)
    {
        if (ChromaControlDialog->objectName().isEmpty())
            ChromaControlDialog->setObjectName("ChromaControlDialog");
        ChromaControlDialog->resize(400, 300);
        ChromaControlDialog->setMinimumSize(QSize(400, 300));
        ChromaControlDialog->setMaximumSize(QSize(16777215, 16777215));
        buttonBox = new QDialogButtonBox(ChromaControlDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(103, 248, 161, 32));
        buttonBox->setAcceptDrops(false);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBoxOrbitClosure = new QGroupBox(ChromaControlDialog);
        groupBoxOrbitClosure->setObjectName("groupBoxOrbitClosure");
        groupBoxOrbitClosure->setGeometry(QRect(12, 13, 372, 226));
        layoutWidget = new QWidget(groupBoxOrbitClosure);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(37, 46, 287, 132));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout_2->addWidget(label, 4, 0, 1, 1);

        techSpinBoxStepSize = new ScientificDoubleSpinBox(layoutWidget);
        techSpinBoxStepSize->setObjectName("techSpinBoxStepSize");
        techSpinBoxStepSize->setMaximum(1.000000000000000);

        gridLayout_2->addWidget(techSpinBoxStepSize, 1, 1, 1, 1);

        spinBoxNsamples = new QSpinBox(layoutWidget);
        spinBoxNsamples->setObjectName("spinBoxNsamples");
        spinBoxNsamples->setMaximum(250);

        gridLayout_2->addWidget(spinBoxNsamples, 4, 1, 1, 1);

        radioButtonRing = new QRadioButton(groupBoxOrbitClosure);
        radioButtonRing->setObjectName("radioButtonRing");
        radioButtonRing->setGeometry(QRect(42, 190, 94, 20));
        radioButtonLine = new QRadioButton(groupBoxOrbitClosure);
        radioButtonLine->setObjectName("radioButtonLine");
        radioButtonLine->setGeometry(QRect(186, 189, 94, 20));

        retranslateUi(ChromaControlDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ChromaControlDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ChromaControlDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ChromaControlDialog);
    } // setupUi

    void retranslateUi(QDialog *ChromaControlDialog)
    {
        ChromaControlDialog->setWindowTitle(QCoreApplication::translate("ChromaControlDialog", "Chromaticity", nullptr));
        groupBoxOrbitClosure->setTitle(QCoreApplication::translate("ChromaControlDialog", "Chromaticity ", nullptr));
        label_2->setText(QCoreApplication::translate("ChromaControlDialog", "dp/p step size", nullptr));
        label->setText(QCoreApplication::translate("ChromaControlDialog", "no of samples (2n+1) ", nullptr));
        radioButtonRing->setText(QCoreApplication::translate("ChromaControlDialog", "Ring Mode", nullptr));
        radioButtonLine->setText(QCoreApplication::translate("ChromaControlDialog", "Line Mode", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChromaControlDialog: public Ui_ChromaControlDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHROMA_CONTROL_DIALOG_H
