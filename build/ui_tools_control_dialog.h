/********************************************************************************
** Form generated from reading UI file 'tools_control_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOOLS_CONTROL_DIALOG_H
#define UI_TOOLS_CONTROL_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_ToolsControlDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBoxOrbitClosure;
    QGroupBox *groupBoxGeneralClosure;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    ScientificDoubleSpinBox *techSpinBoxConvergenceAccuracy;
    ScientificDoubleSpinBox *techSpinBoxConvergenceParameter;
    QLabel *label_3;
    QSpinBox *spinBoxMaxIterations;
    QLabel *label;
    QLabel *label_2;
    QRadioButton *radioButton6D;
    QRadioButton *radioButton4D;
    QGroupBox *groupBox;
    QRadioButton *radioButtonLinearClosure;
    QRadioButton *radioButtonGeneralClosure;
    QGroupBox *groupBox_2;
    QGroupBox *groupBoxLongitudinal;
    QRadioButton *radioButtoncm;
    QRadioButton *radioButtonns;
    QRadioButton *radioButtondeg;
    QRadioButton *radioButtonrd;
    QDoubleSpinBox *doubleSpinBoxFrequency;
    QLabel *label_4;
    QGroupBox *groupBox_4;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout;
    QRadioButton *radioButtonPlotNormalizedDisplacement;
    QRadioButton *radioButtonPlotDisplacementAndBoundaries;
    QRadioButton *radioButtonNormalPlot;

    void setupUi(QDialog *ToolsControlDialog)
    {
        if (ToolsControlDialog->objectName().isEmpty())
            ToolsControlDialog->setObjectName("ToolsControlDialog");
        ToolsControlDialog->resize(400, 653);
        ToolsControlDialog->setMinimumSize(QSize(400, 500));
        ToolsControlDialog->setMaximumSize(QSize(16777215, 16777215));
        buttonBox = new QDialogButtonBox(ToolsControlDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(119, 612, 161, 32));
        buttonBox->setAcceptDrops(false);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBoxOrbitClosure = new QGroupBox(ToolsControlDialog);
        groupBoxOrbitClosure->setObjectName("groupBoxOrbitClosure");
        groupBoxOrbitClosure->setGeometry(QRect(11, 12, 372, 303));
        groupBoxGeneralClosure = new QGroupBox(groupBoxOrbitClosure);
        groupBoxGeneralClosure->setObjectName("groupBoxGeneralClosure");
        groupBoxGeneralClosure->setGeometry(QRect(17, 129, 344, 162));
        layoutWidget = new QWidget(groupBoxGeneralClosure);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(16, 51, 287, 104));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        techSpinBoxConvergenceAccuracy = new ScientificDoubleSpinBox(layoutWidget);
        techSpinBoxConvergenceAccuracy->setObjectName("techSpinBoxConvergenceAccuracy");
        techSpinBoxConvergenceAccuracy->setDecimals(323);
        techSpinBoxConvergenceAccuracy->setMinimum(0.000000000000000);
        techSpinBoxConvergenceAccuracy->setMaximum(0.100000000000000);
        techSpinBoxConvergenceAccuracy->setSingleStep(0.000010000000000);
        techSpinBoxConvergenceAccuracy->setValue(0.000000000000000);

        gridLayout_2->addWidget(techSpinBoxConvergenceAccuracy, 2, 1, 1, 1);

        techSpinBoxConvergenceParameter = new ScientificDoubleSpinBox(layoutWidget);
        techSpinBoxConvergenceParameter->setObjectName("techSpinBoxConvergenceParameter");

        gridLayout_2->addWidget(techSpinBoxConvergenceParameter, 1, 1, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        gridLayout_2->addWidget(label_3, 2, 0, 1, 1);

        spinBoxMaxIterations = new QSpinBox(layoutWidget);
        spinBoxMaxIterations->setObjectName("spinBoxMaxIterations");

        gridLayout_2->addWidget(spinBoxMaxIterations, 0, 1, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        radioButton6D = new QRadioButton(groupBoxGeneralClosure);
        radioButton6D->setObjectName("radioButton6D");
        radioButton6D->setGeometry(QRect(18, 17, 94, 20));
        radioButton4D = new QRadioButton(groupBoxGeneralClosure);
        radioButton4D->setObjectName("radioButton4D");
        radioButton4D->setGeometry(QRect(117, 20, 94, 20));
        groupBox = new QGroupBox(groupBoxOrbitClosure);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(21, 33, 341, 84));
        radioButtonLinearClosure = new QRadioButton(groupBox);
        radioButtonLinearClosure->setObjectName("radioButtonLinearClosure");
        radioButtonLinearClosure->setGeometry(QRect(9, 18, 311, 21));
        radioButtonGeneralClosure = new QRadioButton(groupBox);
        radioButtonGeneralClosure->setObjectName("radioButtonGeneralClosure");
        radioButtonGeneralClosure->setGeometry(QRect(8, 43, 191, 21));
        radioButtonGeneralClosure->setCheckable(true);
        groupBox_2 = new QGroupBox(ToolsControlDialog);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(10, 321, 373, 274));
        groupBoxLongitudinal = new QGroupBox(groupBox_2);
        groupBoxLongitudinal->setObjectName("groupBoxLongitudinal");
        groupBoxLongitudinal->setGeometry(QRect(17, 131, 344, 129));
        radioButtoncm = new QRadioButton(groupBoxLongitudinal);
        radioButtoncm->setObjectName("radioButtoncm");
        radioButtoncm->setGeometry(QRect(13, 10, 182, 20));
        radioButtonns = new QRadioButton(groupBoxLongitudinal);
        radioButtonns->setObjectName("radioButtonns");
        radioButtonns->setGeometry(QRect(11, 36, 182, 20));
        radioButtondeg = new QRadioButton(groupBoxLongitudinal);
        radioButtondeg->setObjectName("radioButtondeg");
        radioButtondeg->setGeometry(QRect(12, 64, 182, 20));
        radioButtonrd = new QRadioButton(groupBoxLongitudinal);
        radioButtonrd->setObjectName("radioButtonrd");
        radioButtonrd->setGeometry(QRect(11, 90, 171, 20));
        doubleSpinBoxFrequency = new QDoubleSpinBox(groupBoxLongitudinal);
        doubleSpinBoxFrequency->setObjectName("doubleSpinBoxFrequency");
        doubleSpinBoxFrequency->setGeometry(QRect(203, 85, 131, 30));
        doubleSpinBoxFrequency->setDecimals(3);
        doubleSpinBoxFrequency->setMaximum(100000.000000000000000);
        label_4 = new QLabel(groupBoxLongitudinal);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(206, 64, 126, 20));
        groupBox_4 = new QGroupBox(groupBox_2);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setGeometry(QRect(17, 29, 342, 96));
        layoutWidget1 = new QWidget(groupBox_4);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(17, 12, 311, 74));
        gridLayout = new QGridLayout(layoutWidget1);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        radioButtonPlotNormalizedDisplacement = new QRadioButton(layoutWidget1);
        radioButtonPlotNormalizedDisplacement->setObjectName("radioButtonPlotNormalizedDisplacement");

        gridLayout->addWidget(radioButtonPlotNormalizedDisplacement, 2, 0, 1, 1);

        radioButtonPlotDisplacementAndBoundaries = new QRadioButton(layoutWidget1);
        radioButtonPlotDisplacementAndBoundaries->setObjectName("radioButtonPlotDisplacementAndBoundaries");

        gridLayout->addWidget(radioButtonPlotDisplacementAndBoundaries, 3, 0, 1, 1);

        radioButtonNormalPlot = new QRadioButton(layoutWidget1);
        radioButtonNormalPlot->setObjectName("radioButtonNormalPlot");

        gridLayout->addWidget(radioButtonNormalPlot, 1, 0, 1, 1);


        retranslateUi(ToolsControlDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ToolsControlDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ToolsControlDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ToolsControlDialog);
    } // setupUi

    void retranslateUi(QDialog *ToolsControlDialog)
    {
        ToolsControlDialog->setWindowTitle(QCoreApplication::translate("ToolsControlDialog", "Tools Control ", nullptr));
        groupBoxOrbitClosure->setTitle(QCoreApplication::translate("ToolsControlDialog", "Orbit Closure", nullptr));
        groupBoxGeneralClosure->setTitle(QString());
        label_3->setText(QCoreApplication::translate("ToolsControlDialog", "Convergence (relative) ", nullptr));
        label->setText(QCoreApplication::translate("ToolsControlDialog", "Relaxation parameter", nullptr));
        label_2->setText(QCoreApplication::translate("ToolsControlDialog", "Maximum no of Iterations", nullptr));
        radioButton6D->setText(QCoreApplication::translate("ToolsControlDialog", "&6D", nullptr));
        radioButton4D->setText(QCoreApplication::translate("ToolsControlDialog", "&4D", nullptr));
        groupBox->setTitle(QString());
        radioButtonLinearClosure->setText(QCoreApplication::translate("ToolsControlDialog", " Linear closure (includes dispersion)", nullptr));
        radioButtonGeneralClosure->setText(QCoreApplication::translate("ToolsControlDialog", "&General Orbit Closure", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("ToolsControlDialog", "Trajectory Plot", nullptr));
        groupBoxLongitudinal->setTitle(QString());
        radioButtoncm->setText(QCoreApplication::translate("ToolsControlDialog", "Longit&udinal Position in cm", nullptr));
        radioButtonns->setText(QCoreApplication::translate("ToolsControlDialog", "&Longitudinal Position in ns", nullptr));
        radioButtondeg->setText(QCoreApplication::translate("ToolsControlDialog", "Longi&tudinal Position in deg", nullptr));
        radioButtonrd->setText(QCoreApplication::translate("ToolsControlDialog", "Longitu&dinal Position in rd", nullptr));
        label_4->setText(QCoreApplication::translate("ToolsControlDialog", "Rf Frequency [MHz]", nullptr));
        groupBox_4->setTitle(QString());
        radioButtonPlotNormalizedDisplacement->setText(QCoreApplication::translate("ToolsControlDialog", "Plot beam displacement normali&zed w/r to beam size", nullptr));
        radioButtonPlotDisplacementAndBoundaries->setText(QCoreApplication::translate("ToolsControlDialog", "Plot beam &displacement and beam boundaries", nullptr));
        radioButtonNormalPlot->setText(QCoreApplication::translate("ToolsControlDialog", "&Normal plot", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ToolsControlDialog: public Ui_ToolsControlDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOOLS_CONTROL_DIALOG_H
