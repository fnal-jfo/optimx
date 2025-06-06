/********************************************************************************
** Form generated from reading UI file 'tracker_distribution_dialog_new.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKER_DISTRIBUTION_DIALOG_NEW_H
#define UI_TRACKER_DISTRIBUTION_DIALOG_NEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_TrackerDistributionDialogNew
{
public:
    QDialogButtonBox *buttonBox;
    QSpinBox *spinBoxNpart;
    QLabel *label;
    QGroupBox *groupBox;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxRmin;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxRmax;
    QRadioButton *radioButtonLGaussian;
    QRadioButton *radioButtonLUniform;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_4;
    ScientificDoubleSpinBox *doubleSpinBoxBx1;
    QLabel *label_8;
    ScientificDoubleSpinBox *doubleSpinBoxBy1;
    QLabel *label_12;
    ScientificDoubleSpinBox *doubleSpinBoxEta1;
    QLabel *label_5;
    ScientificDoubleSpinBox *doubleSpinBoxAx1;
    QLabel *label_9;
    ScientificDoubleSpinBox *doubleSpinBoxAy1;
    QLabel *label_13;
    ScientificDoubleSpinBox *doubleSpinBoxEta1p;
    QLabel *label_6;
    ScientificDoubleSpinBox *doubleSpinBoxBx2;
    QLabel *label_10;
    ScientificDoubleSpinBox *doubleSpinBoxBy2;
    QLabel *label_14;
    ScientificDoubleSpinBox *doubleSpinBoxEta2;
    QLabel *label_7;
    ScientificDoubleSpinBox *doubleSpinBoxAx2;
    QLabel *label_11;
    ScientificDoubleSpinBox *doubleSpinBoxAy2;
    QLabel *label_15;
    ScientificDoubleSpinBox *doubleSpinBoxEta2p;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QLabel *label_16;
    ScientificDoubleSpinBox *doubleSpinBoxU;
    QLabel *label_18;
    ScientificDoubleSpinBox *doubleSpinBoxEps1;
    QLabel *label_19;
    ScientificDoubleSpinBox *doubleSpinBoxEps2;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_20;
    ScientificDoubleSpinBox *doubleSpinBoxAlpha_L;
    QLabel *label_21;
    ScientificDoubleSpinBox *doubleSpinBoxSigma_s;
    QLabel *label_17;
    ScientificDoubleSpinBox *doubleSpinBoxSigma_p;
    QCheckBox *deltaFunctionCheckBox;
    QGroupBox *groupBoxTransverse;
    QDoubleSpinBox *doubleSpinBoxRmin_2;
    QLabel *label_22;
    QRadioButton *radioButtonTGaussian;
    QRadioButton *radioButtonTUniform;

    void setupUi(QDialog *TrackerDistributionDialogNew)
    {
        if (TrackerDistributionDialogNew->objectName().isEmpty())
            TrackerDistributionDialogNew->setObjectName("TrackerDistributionDialogNew");
        TrackerDistributionDialogNew->resize(525, 589);
        TrackerDistributionDialogNew->setMinimumSize(QSize(283, 269));
        TrackerDistributionDialogNew->setMaximumSize(QSize(1000, 1000));
        buttonBox = new QDialogButtonBox(TrackerDistributionDialogNew);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(160, 530, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        spinBoxNpart = new QSpinBox(TrackerDistributionDialogNew);
        spinBoxNpart->setObjectName("spinBoxNpart");
        spinBoxNpart->setGeometry(QRect(150, 20, 81, 22));
        label = new QLabel(TrackerDistributionDialogNew);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 20, 121, 16));
        groupBox = new QGroupBox(TrackerDistributionDialogNew);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(270, 290, 251, 211));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 110, 121, 16));
        doubleSpinBoxRmin = new QDoubleSpinBox(groupBox);
        doubleSpinBoxRmin->setObjectName("doubleSpinBoxRmin");
        doubleSpinBoxRmin->setGeometry(QRect(140, 110, 81, 22));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 140, 71, 16));
        doubleSpinBoxRmax = new QDoubleSpinBox(groupBox);
        doubleSpinBoxRmax->setObjectName("doubleSpinBoxRmax");
        doubleSpinBoxRmax->setGeometry(QRect(140, 140, 81, 22));
        radioButtonLGaussian = new QRadioButton(groupBox);
        radioButtonLGaussian->setObjectName("radioButtonLGaussian");
        radioButtonLGaussian->setGeometry(QRect(20, 50, 98, 22));
        radioButtonLUniform = new QRadioButton(groupBox);
        radioButtonLUniform->setObjectName("radioButtonLUniform");
        radioButtonLUniform->setGeometry(QRect(110, 50, 98, 22));
        layoutWidget = new QWidget(TrackerDistributionDialogNew);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 50, 495, 148));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        doubleSpinBoxBx1 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxBx1->setObjectName("doubleSpinBoxBx1");
        doubleSpinBoxBx1->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(doubleSpinBoxBx1, 0, 1, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 0, 2, 1, 1);

        doubleSpinBoxBy1 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxBy1->setObjectName("doubleSpinBoxBy1");
        doubleSpinBoxBy1->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(doubleSpinBoxBy1, 0, 3, 1, 1);

        label_12 = new QLabel(layoutWidget);
        label_12->setObjectName("label_12");

        gridLayout->addWidget(label_12, 0, 4, 1, 1);

        doubleSpinBoxEta1 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxEta1->setObjectName("doubleSpinBoxEta1");
        doubleSpinBoxEta1->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(doubleSpinBoxEta1, 0, 5, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        doubleSpinBoxAx1 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxAx1->setObjectName("doubleSpinBoxAx1");
        doubleSpinBoxAx1->setMinimum(-10000000.000000000000000);
        doubleSpinBoxAx1->setMaximum(100000.000000000000000);

        gridLayout->addWidget(doubleSpinBoxAx1, 1, 1, 1, 1);

        label_9 = new QLabel(layoutWidget);
        label_9->setObjectName("label_9");

        gridLayout->addWidget(label_9, 1, 2, 1, 1);

        doubleSpinBoxAy1 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxAy1->setObjectName("doubleSpinBoxAy1");

        gridLayout->addWidget(doubleSpinBoxAy1, 1, 3, 1, 1);

        label_13 = new QLabel(layoutWidget);
        label_13->setObjectName("label_13");

        gridLayout->addWidget(label_13, 1, 4, 1, 1);

        doubleSpinBoxEta1p = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxEta1p->setObjectName("doubleSpinBoxEta1p");

        gridLayout->addWidget(doubleSpinBoxEta1p, 1, 5, 1, 1);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 2, 0, 1, 1);

        doubleSpinBoxBx2 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxBx2->setObjectName("doubleSpinBoxBx2");

        gridLayout->addWidget(doubleSpinBoxBx2, 2, 1, 1, 1);

        label_10 = new QLabel(layoutWidget);
        label_10->setObjectName("label_10");

        gridLayout->addWidget(label_10, 2, 2, 1, 1);

        doubleSpinBoxBy2 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxBy2->setObjectName("doubleSpinBoxBy2");

        gridLayout->addWidget(doubleSpinBoxBy2, 2, 3, 1, 1);

        label_14 = new QLabel(layoutWidget);
        label_14->setObjectName("label_14");

        gridLayout->addWidget(label_14, 2, 4, 1, 1);

        doubleSpinBoxEta2 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxEta2->setObjectName("doubleSpinBoxEta2");

        gridLayout->addWidget(doubleSpinBoxEta2, 2, 5, 1, 1);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 3, 0, 1, 1);

        doubleSpinBoxAx2 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxAx2->setObjectName("doubleSpinBoxAx2");

        gridLayout->addWidget(doubleSpinBoxAx2, 3, 1, 1, 1);

        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName("label_11");

        gridLayout->addWidget(label_11, 3, 2, 1, 1);

        doubleSpinBoxAy2 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxAy2->setObjectName("doubleSpinBoxAy2");

        gridLayout->addWidget(doubleSpinBoxAy2, 3, 3, 1, 1);

        label_15 = new QLabel(layoutWidget);
        label_15->setObjectName("label_15");

        gridLayout->addWidget(label_15, 3, 4, 1, 1);

        doubleSpinBoxEta2p = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxEta2p->setObjectName("doubleSpinBoxEta2p");

        gridLayout->addWidget(doubleSpinBoxEta2p, 3, 5, 1, 1);

        layoutWidget1 = new QWidget(TrackerDistributionDialogNew);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(60, 200, 413, 34));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_16 = new QLabel(layoutWidget1);
        label_16->setObjectName("label_16");

        horizontalLayout->addWidget(label_16);

        doubleSpinBoxU = new ScientificDoubleSpinBox(layoutWidget1);
        doubleSpinBoxU->setObjectName("doubleSpinBoxU");
        doubleSpinBoxU->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(doubleSpinBoxU);

        label_18 = new QLabel(layoutWidget1);
        label_18->setObjectName("label_18");

        horizontalLayout->addWidget(label_18);

        doubleSpinBoxEps1 = new ScientificDoubleSpinBox(layoutWidget1);
        doubleSpinBoxEps1->setObjectName("doubleSpinBoxEps1");
        doubleSpinBoxEps1->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(doubleSpinBoxEps1);

        label_19 = new QLabel(layoutWidget1);
        label_19->setObjectName("label_19");

        horizontalLayout->addWidget(label_19);

        doubleSpinBoxEps2 = new ScientificDoubleSpinBox(layoutWidget1);
        doubleSpinBoxEps2->setObjectName("doubleSpinBoxEps2");
        doubleSpinBoxEps2->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(doubleSpinBoxEps2);

        layoutWidget2 = new QWidget(TrackerDistributionDialogNew);
        layoutWidget2->setObjectName("layoutWidget2");
        layoutWidget2->setGeometry(QRect(20, 240, 477, 34));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_20 = new QLabel(layoutWidget2);
        label_20->setObjectName("label_20");

        horizontalLayout_2->addWidget(label_20);

        doubleSpinBoxAlpha_L = new ScientificDoubleSpinBox(layoutWidget2);
        doubleSpinBoxAlpha_L->setObjectName("doubleSpinBoxAlpha_L");
        doubleSpinBoxAlpha_L->setMinimumSize(QSize(100, 0));

        horizontalLayout_2->addWidget(doubleSpinBoxAlpha_L);

        label_21 = new QLabel(layoutWidget2);
        label_21->setObjectName("label_21");

        horizontalLayout_2->addWidget(label_21);

        doubleSpinBoxSigma_s = new ScientificDoubleSpinBox(layoutWidget2);
        doubleSpinBoxSigma_s->setObjectName("doubleSpinBoxSigma_s");
        doubleSpinBoxSigma_s->setMinimumSize(QSize(100, 0));

        horizontalLayout_2->addWidget(doubleSpinBoxSigma_s);

        label_17 = new QLabel(layoutWidget2);
        label_17->setObjectName("label_17");

        horizontalLayout_2->addWidget(label_17);

        doubleSpinBoxSigma_p = new ScientificDoubleSpinBox(layoutWidget2);
        doubleSpinBoxSigma_p->setObjectName("doubleSpinBoxSigma_p");
        doubleSpinBoxSigma_p->setMinimumSize(QSize(100, 0));

        horizontalLayout_2->addWidget(doubleSpinBoxSigma_p);

        deltaFunctionCheckBox = new QCheckBox(TrackerDistributionDialogNew);
        deltaFunctionCheckBox->setObjectName("deltaFunctionCheckBox");
        deltaFunctionCheckBox->setGeometry(QRect(280, 20, 191, 20));
        groupBoxTransverse = new QGroupBox(TrackerDistributionDialogNew);
        groupBoxTransverse->setObjectName("groupBoxTransverse");
        groupBoxTransverse->setGeometry(QRect(20, 290, 231, 211));
        doubleSpinBoxRmin_2 = new QDoubleSpinBox(groupBoxTransverse);
        doubleSpinBoxRmin_2->setObjectName("doubleSpinBoxRmin_2");
        doubleSpinBoxRmin_2->setGeometry(QRect(120, 150, 81, 22));
        label_22 = new QLabel(groupBoxTransverse);
        label_22->setObjectName("label_22");
        label_22->setGeometry(QRect(20, 150, 91, 18));
        radioButtonTGaussian = new QRadioButton(groupBoxTransverse);
        radioButtonTGaussian->setObjectName("radioButtonTGaussian");
        radioButtonTGaussian->setGeometry(QRect(20, 50, 98, 22));
        radioButtonTUniform = new QRadioButton(groupBoxTransverse);
        radioButtonTUniform->setObjectName("radioButtonTUniform");
        radioButtonTUniform->setGeometry(QRect(120, 50, 98, 22));

        retranslateUi(TrackerDistributionDialogNew);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TrackerDistributionDialogNew, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TrackerDistributionDialogNew, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TrackerDistributionDialogNew);
    } // setupUi

    void retranslateUi(QDialog *TrackerDistributionDialogNew)
    {
        TrackerDistributionDialogNew->setWindowTitle(QCoreApplication::translate("TrackerDistributionDialogNew", "Distribution Function Parameters", nullptr));
        label->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Number of particles", nullptr));
        groupBox->setTitle(QCoreApplication::translate("TrackerDistributionDialogNew", "Longitudinal Distribution", nullptr));
        label_2->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Rmin [0, Rmax-0.1]", nullptr));
        label_3->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Rmax [0,6]", nullptr));
        radioButtonLGaussian->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Gaussian", nullptr));
        radioButtonLUniform->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Uniform", nullptr));
        label_4->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Beta_x1", nullptr));
        label_8->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Beta_y1", nullptr));
        label_12->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "eta_1", nullptr));
        label_5->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Alpha_x1", nullptr));
        label_9->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Alpha_y1", nullptr));
        label_13->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "eta_1p", nullptr));
        label_6->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Beta_x2", nullptr));
        label_10->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Beta_y2", nullptr));
        label_14->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "eta_2", nullptr));
        label_7->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Alpha_x2", nullptr));
        label_11->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Alpha_y2", nullptr));
        label_15->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "eta_2p", nullptr));
        label_16->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "U", nullptr));
        label_18->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "eps_1", nullptr));
        label_19->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "eps_2", nullptr));
        label_20->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "alpha_L", nullptr));
        label_21->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "sigma_s", nullptr));
        label_17->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "sigma_p", nullptr));
        deltaFunctionCheckBox->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Delta Function Distribution", nullptr));
        groupBoxTransverse->setTitle(QCoreApplication::translate("TrackerDistributionDialogNew", "Transverse Distribution ", nullptr));
        label_22->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Cutoff [sigma]", nullptr));
        radioButtonTGaussian->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Gaussian", nullptr));
        radioButtonTUniform->setText(QCoreApplication::translate("TrackerDistributionDialogNew", "Uniform", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackerDistributionDialogNew: public Ui_TrackerDistributionDialogNew {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACKER_DISTRIBUTION_DIALOG_NEW_H
