/********************************************************************************
** Form generated from reading UI file 'control_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROL_DIALOG_H
#define UI_CONTROL_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_ControlDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_2;
    QLabel *label_4;
    QLabel *label_5;
    QSpinBox *spinBoxNoSteps;
    ScientificDoubleSpinBox *techSpinBoxAccuracy;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_4;
    QRadioButton *radioButtonRing;
    QRadioButton *radioButtonLine;
    QCheckBox *checkBoxUseClosedOrbit;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QCheckBox *checkBoxUseTotalSize;
    QCheckBox *checkBoxRewrite;
    QCheckBox *checkBoxDrawApertures;
    QSpinBox *spinBoxPlotSamples;
    QCheckBox *checkBoxClearPlot;
    QLabel *label_3;
    QLabel *label_2;
    QCheckBox *checkBoxAutoPrint;
    QCheckBox *checkBoxClearText;
    QCheckBox *checkBoxAutoPlot;
    QCheckBox *checkBoxDrawLego;
    ScientificDoubleSpinBox *techSpinBoxCoupling;

    void setupUi(QDialog *ControlDialog)
    {
        if (ControlDialog->objectName().isEmpty())
            ControlDialog->setObjectName("ControlDialog");
        ControlDialog->resize(350, 625);
        ControlDialog->setMinimumSize(QSize(350, 625));
        ControlDialog->setMaximumSize(QSize(350, 625));
        buttonBox = new QDialogButtonBox(ControlDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(60, 570, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox_3 = new QGroupBox(ControlDialog);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(30, 470, 271, 83));
        gridLayout_2 = new QGridLayout(groupBox_3);
        gridLayout_2->setObjectName("gridLayout_2");
        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName("label_4");

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName("label_5");

        gridLayout_2->addWidget(label_5, 1, 0, 1, 1);

        spinBoxNoSteps = new QSpinBox(groupBox_3);
        spinBoxNoSteps->setObjectName("spinBoxNoSteps");
        spinBoxNoSteps->setMinimum(1);
        spinBoxNoSteps->setMaximum(5000);

        gridLayout_2->addWidget(spinBoxNoSteps, 0, 1, 1, 1);

        techSpinBoxAccuracy = new ScientificDoubleSpinBox(groupBox_3);
        techSpinBoxAccuracy->setObjectName("techSpinBoxAccuracy");

        gridLayout_2->addWidget(techSpinBoxAccuracy, 1, 1, 1, 1);

        groupBox = new QGroupBox(ControlDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(30, 10, 271, 121));
        QFont font;
        font.setPointSize(8);
        groupBox->setFont(font);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName("verticalLayout");
        groupBox_4 = new QGroupBox(groupBox);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setFlat(false);
        groupBox_4->setCheckable(false);
        radioButtonRing = new QRadioButton(groupBox_4);
        radioButtonRing->setObjectName("radioButtonRing");
        radioButtonRing->setGeometry(QRect(40, 20, 61, 21));
        radioButtonLine = new QRadioButton(groupBox_4);
        radioButtonLine->setObjectName("radioButtonLine");
        radioButtonLine->setGeometry(QRect(130, 20, 51, 21));
        radioButtonLine->setChecked(true);

        verticalLayout->addWidget(groupBox_4);

        checkBoxUseClosedOrbit = new QCheckBox(groupBox);
        checkBoxUseClosedOrbit->setObjectName("checkBoxUseClosedOrbit");

        verticalLayout->addWidget(checkBoxUseClosedOrbit);

        groupBox_2 = new QGroupBox(ControlDialog);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(30, 140, 271, 311));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName("gridLayout");
        checkBoxUseTotalSize = new QCheckBox(groupBox_2);
        checkBoxUseTotalSize->setObjectName("checkBoxUseTotalSize");

        gridLayout->addWidget(checkBoxUseTotalSize, 8, 0, 1, 2);

        checkBoxRewrite = new QCheckBox(groupBox_2);
        checkBoxRewrite->setObjectName("checkBoxRewrite");

        gridLayout->addWidget(checkBoxRewrite, 5, 0, 1, 2);

        checkBoxDrawApertures = new QCheckBox(groupBox_2);
        checkBoxDrawApertures->setObjectName("checkBoxDrawApertures");

        gridLayout->addWidget(checkBoxDrawApertures, 7, 0, 1, 2);

        spinBoxPlotSamples = new QSpinBox(groupBox_2);
        spinBoxPlotSamples->setObjectName("spinBoxPlotSamples");
        spinBoxPlotSamples->setMaximum(10000000);
        spinBoxPlotSamples->setSingleStep(1000);

        gridLayout->addWidget(spinBoxPlotSamples, 14, 1, 1, 1);

        checkBoxClearPlot = new QCheckBox(groupBox_2);
        checkBoxClearPlot->setObjectName("checkBoxClearPlot");

        gridLayout->addWidget(checkBoxClearPlot, 2, 0, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 15, 0, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 14, 0, 1, 1);

        checkBoxAutoPrint = new QCheckBox(groupBox_2);
        checkBoxAutoPrint->setObjectName("checkBoxAutoPrint");

        gridLayout->addWidget(checkBoxAutoPrint, 1, 0, 1, 2);

        checkBoxClearText = new QCheckBox(groupBox_2);
        checkBoxClearText->setObjectName("checkBoxClearText");

        gridLayout->addWidget(checkBoxClearText, 3, 0, 1, 1);

        checkBoxAutoPlot = new QCheckBox(groupBox_2);
        checkBoxAutoPlot->setObjectName("checkBoxAutoPlot");

        gridLayout->addWidget(checkBoxAutoPlot, 0, 0, 1, 2);

        checkBoxDrawLego = new QCheckBox(groupBox_2);
        checkBoxDrawLego->setObjectName("checkBoxDrawLego");

        gridLayout->addWidget(checkBoxDrawLego, 6, 0, 1, 1);

        techSpinBoxCoupling = new ScientificDoubleSpinBox(groupBox_2);
        techSpinBoxCoupling->setObjectName("techSpinBoxCoupling");

        gridLayout->addWidget(techSpinBoxCoupling, 15, 1, 1, 1);


        retranslateUi(ControlDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ControlDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ControlDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ControlDialog);
    } // setupUi

    void retranslateUi(QDialog *ControlDialog)
    {
        ControlDialog->setWindowTitle(QCoreApplication::translate("ControlDialog", "View Control Dialog", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("ControlDialog", "Trajectory Integration in cavity (W)", nullptr));
        label_4->setText(QCoreApplication::translate("ControlDialog", "No steps / wavelength", nullptr));
        label_5->setText(QCoreApplication::translate("ControlDialog", "Accuracy", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ControlDialog", "Global Settings", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("ControlDialog", "Symmetry", nullptr));
        radioButtonRing->setText(QCoreApplication::translate("ControlDialog", "Rin&g", nullptr));
        radioButtonLine->setText(QCoreApplication::translate("ControlDialog", "&Line", nullptr));
        checkBoxUseClosedOrbit->setText(QCoreApplication::translate("ControlDialog", "Use excited orbit as reference", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("ControlDialog", "View Menu Function Preferences", nullptr));
        checkBoxUseTotalSize->setText(QCoreApplication::translate("ControlDialog", "Include dispersion contribution in rms size", nullptr));
        checkBoxRewrite->setText(QCoreApplication::translate("ControlDialog", "Rewrite text buffer when analyzing ", nullptr));
        checkBoxDrawApertures->setText(QCoreApplication::translate("ControlDialog", "Draw apertures on beam size plots", nullptr));
        checkBoxClearPlot->setText(QCoreApplication::translate("ControlDialog", "Clear plots", nullptr));
        label_3->setText(QCoreApplication::translate("ControlDialog", "Coupling Threshold ", nullptr));
        label_2->setText(QCoreApplication::translate("ControlDialog", "No of samples in plots", nullptr));
        checkBoxAutoPrint->setText(QCoreApplication::translate("ControlDialog", "Interactive lattice function tables", nullptr));
        checkBoxClearText->setText(QCoreApplication::translate("ControlDialog", "Clear text", nullptr));
        checkBoxAutoPlot->setText(QCoreApplication::translate("ControlDialog", "Interactive lattice function plots", nullptr));
        checkBoxDrawLego->setText(QCoreApplication::translate("ControlDialog", "Draw Lego Plots", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ControlDialog: public Ui_ControlDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROL_DIALOG_H
