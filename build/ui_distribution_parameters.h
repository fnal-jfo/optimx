/********************************************************************************
** Form generated from reading UI file 'distribution_parameters.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISTRIBUTION_PARAMETERS_H
#define UI_DISTRIBUTION_PARAMETERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackerDistributionDialog
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
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QDoubleSpinBox *doubleSpinBoxU;
    QLabel *label_16;
    QDoubleSpinBox *doubleSpinBoxDpp;
    QLabel *label_17;
    QWidget *widget1;
    QGridLayout *gridLayout;
    QDoubleSpinBox *doubleSpinBoxBx1;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBoxBy1;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBoxEta1;
    QLabel *label_12;
    QDoubleSpinBox *doubleSpinBoxAx1;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxAy1;
    QLabel *label_9;
    QDoubleSpinBox *doubleSpinBoxEta1p;
    QLabel *label_13;
    QDoubleSpinBox *doubleSpinBoxBx2;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxBy2;
    QLabel *label_10;
    QDoubleSpinBox *doubleSpinBoxEta2;
    QLabel *label_14;
    QDoubleSpinBox *doubleSpinBoxAx2;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBoxAy2;
    QLabel *label_11;
    QDoubleSpinBox *doubleSpinBoxEta2p;
    QLabel *label_15;

    void setupUi(QDialog *TrackerDistributionDialog)
    {
        if (TrackerDistributionDialog->objectName().isEmpty())
            TrackerDistributionDialog->setObjectName("TrackerDistributionDialog");
        TrackerDistributionDialog->resize(526, 511);
        TrackerDistributionDialog->setMinimumSize(QSize(283, 269));
        TrackerDistributionDialog->setMaximumSize(QSize(1000, 1000));
        buttonBox = new QDialogButtonBox(TrackerDistributionDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(160, 440, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        spinBoxNpart = new QSpinBox(TrackerDistributionDialog);
        spinBoxNpart->setObjectName("spinBoxNpart");
        spinBoxNpart->setGeometry(QRect(150, 20, 81, 22));
        label = new QLabel(TrackerDistributionDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 20, 121, 16));
        groupBox = new QGroupBox(TrackerDistributionDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(130, 290, 251, 111));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 50, 121, 16));
        doubleSpinBoxRmin = new QDoubleSpinBox(groupBox);
        doubleSpinBoxRmin->setObjectName("doubleSpinBoxRmin");
        doubleSpinBoxRmin->setGeometry(QRect(150, 50, 81, 22));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 80, 71, 16));
        doubleSpinBoxRmax = new QDoubleSpinBox(groupBox);
        doubleSpinBoxRmax->setObjectName("doubleSpinBoxRmax");
        doubleSpinBoxRmax->setGeometry(QRect(150, 80, 81, 22));
        widget = new QWidget(TrackerDistributionDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(130, 230, 254, 31));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        doubleSpinBoxU = new QDoubleSpinBox(widget);
        doubleSpinBoxU->setObjectName("doubleSpinBoxU");
        doubleSpinBoxU->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(doubleSpinBoxU);

        label_16 = new QLabel(widget);
        label_16->setObjectName("label_16");

        horizontalLayout->addWidget(label_16);

        doubleSpinBoxDpp = new QDoubleSpinBox(widget);
        doubleSpinBoxDpp->setObjectName("doubleSpinBoxDpp");
        doubleSpinBoxDpp->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(doubleSpinBoxDpp);

        label_17 = new QLabel(widget);
        label_17->setObjectName("label_17");

        horizontalLayout->addWidget(label_17);

        widget1 = new QWidget(TrackerDistributionDialog);
        widget1->setObjectName("widget1");
        widget1->setGeometry(QRect(21, 58, 479, 136));
        gridLayout = new QGridLayout(widget1);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        doubleSpinBoxBx1 = new QDoubleSpinBox(widget1);
        doubleSpinBoxBx1->setObjectName("doubleSpinBoxBx1");
        doubleSpinBoxBx1->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(doubleSpinBoxBx1, 0, 0, 1, 1);

        label_4 = new QLabel(widget1);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 0, 1, 1, 1);

        doubleSpinBoxBy1 = new QDoubleSpinBox(widget1);
        doubleSpinBoxBy1->setObjectName("doubleSpinBoxBy1");
        doubleSpinBoxBy1->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(doubleSpinBoxBy1, 0, 2, 1, 1);

        label_8 = new QLabel(widget1);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 0, 3, 1, 1);

        doubleSpinBoxEta1 = new QDoubleSpinBox(widget1);
        doubleSpinBoxEta1->setObjectName("doubleSpinBoxEta1");
        doubleSpinBoxEta1->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(doubleSpinBoxEta1, 0, 4, 1, 1);

        label_12 = new QLabel(widget1);
        label_12->setObjectName("label_12");

        gridLayout->addWidget(label_12, 0, 5, 1, 1);

        doubleSpinBoxAx1 = new QDoubleSpinBox(widget1);
        doubleSpinBoxAx1->setObjectName("doubleSpinBoxAx1");

        gridLayout->addWidget(doubleSpinBoxAx1, 1, 0, 1, 1);

        label_5 = new QLabel(widget1);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 1, 1, 1, 1);

        doubleSpinBoxAy1 = new QDoubleSpinBox(widget1);
        doubleSpinBoxAy1->setObjectName("doubleSpinBoxAy1");

        gridLayout->addWidget(doubleSpinBoxAy1, 1, 2, 1, 1);

        label_9 = new QLabel(widget1);
        label_9->setObjectName("label_9");

        gridLayout->addWidget(label_9, 1, 3, 1, 1);

        doubleSpinBoxEta1p = new QDoubleSpinBox(widget1);
        doubleSpinBoxEta1p->setObjectName("doubleSpinBoxEta1p");

        gridLayout->addWidget(doubleSpinBoxEta1p, 1, 4, 1, 1);

        label_13 = new QLabel(widget1);
        label_13->setObjectName("label_13");

        gridLayout->addWidget(label_13, 1, 5, 1, 1);

        doubleSpinBoxBx2 = new QDoubleSpinBox(widget1);
        doubleSpinBoxBx2->setObjectName("doubleSpinBoxBx2");

        gridLayout->addWidget(doubleSpinBoxBx2, 2, 0, 1, 1);

        label_6 = new QLabel(widget1);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 2, 1, 1, 1);

        doubleSpinBoxBy2 = new QDoubleSpinBox(widget1);
        doubleSpinBoxBy2->setObjectName("doubleSpinBoxBy2");

        gridLayout->addWidget(doubleSpinBoxBy2, 2, 2, 1, 1);

        label_10 = new QLabel(widget1);
        label_10->setObjectName("label_10");

        gridLayout->addWidget(label_10, 2, 3, 1, 1);

        doubleSpinBoxEta2 = new QDoubleSpinBox(widget1);
        doubleSpinBoxEta2->setObjectName("doubleSpinBoxEta2");

        gridLayout->addWidget(doubleSpinBoxEta2, 2, 4, 1, 1);

        label_14 = new QLabel(widget1);
        label_14->setObjectName("label_14");

        gridLayout->addWidget(label_14, 2, 5, 1, 1);

        doubleSpinBoxAx2 = new QDoubleSpinBox(widget1);
        doubleSpinBoxAx2->setObjectName("doubleSpinBoxAx2");

        gridLayout->addWidget(doubleSpinBoxAx2, 3, 0, 1, 1);

        label_7 = new QLabel(widget1);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 3, 1, 1, 1);

        doubleSpinBoxAy2 = new QDoubleSpinBox(widget1);
        doubleSpinBoxAy2->setObjectName("doubleSpinBoxAy2");

        gridLayout->addWidget(doubleSpinBoxAy2, 3, 2, 1, 1);

        label_11 = new QLabel(widget1);
        label_11->setObjectName("label_11");

        gridLayout->addWidget(label_11, 3, 3, 1, 1);

        doubleSpinBoxEta2p = new QDoubleSpinBox(widget1);
        doubleSpinBoxEta2p->setObjectName("doubleSpinBoxEta2p");

        gridLayout->addWidget(doubleSpinBoxEta2p, 3, 4, 1, 1);

        label_15 = new QLabel(widget1);
        label_15->setObjectName("label_15");

        gridLayout->addWidget(label_15, 3, 5, 1, 1);


        retranslateUi(TrackerDistributionDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TrackerDistributionDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TrackerDistributionDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TrackerDistributionDialog);
    } // setupUi

    void retranslateUi(QDialog *TrackerDistributionDialog)
    {
        TrackerDistributionDialog->setWindowTitle(QCoreApplication::translate("TrackerDistributionDialog", "Distribution Function Parameters", nullptr));
        label->setText(QCoreApplication::translate("TrackerDistributionDialog", "Number of particles", nullptr));
        groupBox->setTitle(QCoreApplication::translate("TrackerDistributionDialog", "Distribution Function Limits", nullptr));
        label_2->setText(QCoreApplication::translate("TrackerDistributionDialog", "Rmin [0, Rmax-0.1]", nullptr));
        label_3->setText(QCoreApplication::translate("TrackerDistributionDialog", "Rmax [0,6]", nullptr));
        label_16->setText(QCoreApplication::translate("TrackerDistributionDialog", "U", nullptr));
        label_17->setText(QCoreApplication::translate("TrackerDistributionDialog", "dp/p", nullptr));
        label_4->setText(QCoreApplication::translate("TrackerDistributionDialog", "Beta_x1", nullptr));
        label_8->setText(QCoreApplication::translate("TrackerDistributionDialog", "Beta_y1", nullptr));
        label_12->setText(QCoreApplication::translate("TrackerDistributionDialog", "eta_1", nullptr));
        label_5->setText(QCoreApplication::translate("TrackerDistributionDialog", "Alpha_x1", nullptr));
        label_9->setText(QCoreApplication::translate("TrackerDistributionDialog", "Alpha_y1", nullptr));
        label_13->setText(QCoreApplication::translate("TrackerDistributionDialog", "eta_1p", nullptr));
        label_6->setText(QCoreApplication::translate("TrackerDistributionDialog", "Beta_x2", nullptr));
        label_10->setText(QCoreApplication::translate("TrackerDistributionDialog", "Beta_y2", nullptr));
        label_14->setText(QCoreApplication::translate("TrackerDistributionDialog", "eta_2", nullptr));
        label_7->setText(QCoreApplication::translate("TrackerDistributionDialog", "Alpha_x2", nullptr));
        label_11->setText(QCoreApplication::translate("TrackerDistributionDialog", "Alpha_y2", nullptr));
        label_15->setText(QCoreApplication::translate("TrackerDistributionDialog", "eta_2p", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackerDistributionDialog: public Ui_TrackerDistributionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISTRIBUTION_PARAMETERS_H
