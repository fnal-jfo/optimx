/********************************************************************************
** Form generated from reading UI file 'steps_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STEPS_DIALOG_H
#define UI_STEPS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include <ScientificDoubleSpinBox.h>

QT_BEGIN_NAMESPACE

class Ui_StepsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    ScientificDoubleSpinBox *doubleSpinBoxdL;
    QLabel *label_9;
    ScientificDoubleSpinBox *doubleSpinBoxdOfsx;
    QLabel *label_7;
    ScientificDoubleSpinBox *doubleSpinBoxdOfsy;
    QLabel *label_2;
    ScientificDoubleSpinBox *doubleSpinBoxdB;
    QLabel *label_3;
    ScientificDoubleSpinBox *doubleSpinBoxdG;
    QLabel *label_4;
    ScientificDoubleSpinBox *doubleSpinBoxdS;
    QLabel *label_5;
    ScientificDoubleSpinBox *doubleSpinBoxVar;
    QLabel *label_8;
    ScientificDoubleSpinBox *doubleSpinBoxVolts;
    QLabel *label_6;
    ScientificDoubleSpinBox *doubleSpinBoxPhase;

    void setupUi(QDialog *StepsDialog)
    {
        if (StepsDialog->objectName().isEmpty())
            StepsDialog->setObjectName("StepsDialog");
        StepsDialog->setWindowModality(Qt::ApplicationModal);
        StepsDialog->resize(344, 400);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(StepsDialog->sizePolicy().hasHeightForWidth());
        StepsDialog->setSizePolicy(sizePolicy);
        StepsDialog->setMinimumSize(QSize(344, 400));
        StepsDialog->setMaximumSize(QSize(344, 400));
        StepsDialog->setModal(true);
        buttonBox = new QDialogButtonBox(StepsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(91, 319, 156, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(StepsDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(50, 50, 251, 258));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        doubleSpinBoxdL = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxdL->setObjectName("doubleSpinBoxdL");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(doubleSpinBoxdL->sizePolicy().hasHeightForWidth());
        doubleSpinBoxdL->setSizePolicy(sizePolicy1);
        doubleSpinBoxdL->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxdL, 0, 1, 1, 1);

        label_9 = new QLabel(layoutWidget);
        label_9->setObjectName("label_9");

        gridLayout->addWidget(label_9, 1, 0, 1, 1);

        doubleSpinBoxdOfsx = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxdOfsx->setObjectName("doubleSpinBoxdOfsx");
        sizePolicy1.setHeightForWidth(doubleSpinBoxdOfsx->sizePolicy().hasHeightForWidth());
        doubleSpinBoxdOfsx->setSizePolicy(sizePolicy1);
        doubleSpinBoxdOfsx->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxdOfsx, 1, 1, 1, 1);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 2, 0, 1, 1);

        doubleSpinBoxdOfsy = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxdOfsy->setObjectName("doubleSpinBoxdOfsy");
        sizePolicy1.setHeightForWidth(doubleSpinBoxdOfsy->sizePolicy().hasHeightForWidth());
        doubleSpinBoxdOfsy->setSizePolicy(sizePolicy1);
        doubleSpinBoxdOfsy->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxdOfsy, 2, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        doubleSpinBoxdB = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxdB->setObjectName("doubleSpinBoxdB");
        sizePolicy1.setHeightForWidth(doubleSpinBoxdB->sizePolicy().hasHeightForWidth());
        doubleSpinBoxdB->setSizePolicy(sizePolicy1);
        doubleSpinBoxdB->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxdB, 3, 1, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        doubleSpinBoxdG = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxdG->setObjectName("doubleSpinBoxdG");
        sizePolicy1.setHeightForWidth(doubleSpinBoxdG->sizePolicy().hasHeightForWidth());
        doubleSpinBoxdG->setSizePolicy(sizePolicy1);
        doubleSpinBoxdG->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxdG, 4, 1, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 5, 0, 1, 1);

        doubleSpinBoxdS = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxdS->setObjectName("doubleSpinBoxdS");
        sizePolicy1.setHeightForWidth(doubleSpinBoxdS->sizePolicy().hasHeightForWidth());
        doubleSpinBoxdS->setSizePolicy(sizePolicy1);
        doubleSpinBoxdS->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxdS, 5, 1, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 6, 0, 1, 1);

        doubleSpinBoxVar = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxVar->setObjectName("doubleSpinBoxVar");
        sizePolicy1.setHeightForWidth(doubleSpinBoxVar->sizePolicy().hasHeightForWidth());
        doubleSpinBoxVar->setSizePolicy(sizePolicy1);
        doubleSpinBoxVar->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxVar, 6, 1, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 7, 0, 1, 1);

        doubleSpinBoxVolts = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxVolts->setObjectName("doubleSpinBoxVolts");
        sizePolicy1.setHeightForWidth(doubleSpinBoxVolts->sizePolicy().hasHeightForWidth());
        doubleSpinBoxVolts->setSizePolicy(sizePolicy1);
        doubleSpinBoxVolts->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxVolts, 7, 1, 1, 1);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 8, 0, 1, 1);

        doubleSpinBoxPhase = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxPhase->setObjectName("doubleSpinBoxPhase");
        sizePolicy1.setHeightForWidth(doubleSpinBoxPhase->sizePolicy().hasHeightForWidth());
        doubleSpinBoxPhase->setSizePolicy(sizePolicy1);
        doubleSpinBoxPhase->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(doubleSpinBoxPhase, 8, 1, 1, 1);


        retranslateUi(StepsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, StepsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, StepsDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(StepsDialog);
    } // setupUi

    void retranslateUi(QDialog *StepsDialog)
    {
        StepsDialog->setWindowTitle(QCoreApplication::translate("StepsDialog", "Increment/Decrement Steps", nullptr));
        label->setText(QCoreApplication::translate("StepsDialog", "length, dL [cm]", nullptr));
        label_9->setText(QCoreApplication::translate("StepsDialog", "Hor  offset [cm]", nullptr));
        label_7->setText(QCoreApplication::translate("StepsDialog", "Ver  offset [cm]", nullptr));
        label_2->setText(QCoreApplication::translate("StepsDialog", "magnetic field, dB [kG]", nullptr));
        label_3->setText(QCoreApplication::translate("StepsDialog", "quadrupole, dG [kG/cm]", nullptr));
        label_4->setText(QCoreApplication::translate("StepsDialog", "sextupole, dS [kg/cm**2] ", nullptr));
        label_5->setText(QCoreApplication::translate("StepsDialog", "variables", nullptr));
        label_8->setText(QCoreApplication::translate("StepsDialog", "voltage [MV]", nullptr));
        label_6->setText(QCoreApplication::translate("StepsDialog", "phase [deg]", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StepsDialog: public Ui_StepsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STEPS_DIALOG_H
