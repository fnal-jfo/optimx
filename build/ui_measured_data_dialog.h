/********************************************************************************
** Form generated from reading UI file 'measured_data_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MEASURED_DATA_DIALOG_H
#define UI_MEASURED_DATA_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_MeasuredDataDialog
{
public:
    QLabel *label_10;
    QGroupBox *groupBox;
    QCheckBox *checkBoxRefToS0;
    QCheckBox *checkBoxSfromOptics;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QLabel *label_11;
    QSpinBox *spinBoxNameCol;
    QLabel *label_12;
    QLineEdit *lineEditNamePrefix;
    QLabel *label_13;
    QLineEdit *lineEditfilter;
    QCheckBox *checkBoxMatchCase;
    QWidget *layoutWidget_1;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *spinBoxScol;
    QLabel *label_2;
    QSpinBox *spinBoxDataCol;
    QLabel *label_3;
    QSpinBox *spinBoxErrmCol;
    QLabel *label_4;
    QSpinBox *spinBoxErrpCol;
    QLabel *label_5;
    QSpinBox *spinBoxColor;
    QLabel *label_6;
    QSpinBox *spinBoxCross;
    QLabel *label_7;
    ScientificDoubleSpinBox *doubleSpinBoxScaling;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBoxXoffs;
    QLineEdit *lineEditDataFile;
    QLabel *label_14;
    QPushButton *pushButtonDataFile;
    QDialogButtonBox *buttonBox;
    QButtonGroup *buttonGroup;

    void setupUi(QDialog *MeasuredDataDialog)
    {
        if (MeasuredDataDialog->objectName().isEmpty())
            MeasuredDataDialog->setObjectName("MeasuredDataDialog");
        MeasuredDataDialog->resize(410, 628);
        MeasuredDataDialog->setAutoFillBackground(false);
        MeasuredDataDialog->setSizeGripEnabled(false);
        label_10 = new QLabel(MeasuredDataDialog);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(30, 460, 55, 13));
        groupBox = new QGroupBox(MeasuredDataDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(17, 341, 381, 231));
        checkBoxRefToS0 = new QCheckBox(groupBox);
        buttonGroup = new QButtonGroup(MeasuredDataDialog);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(checkBoxRefToS0);
        checkBoxRefToS0->setObjectName("checkBoxRefToS0");
        checkBoxRefToS0->setGeometry(QRect(40, 30, 211, 21));
        checkBoxRefToS0->setAutoExclusive(true);
        checkBoxSfromOptics = new QCheckBox(groupBox);
        buttonGroup->addButton(checkBoxSfromOptics);
        checkBoxSfromOptics->setObjectName("checkBoxSfromOptics");
        checkBoxSfromOptics->setGeometry(QRect(40, 60, 301, 21));
        checkBoxSfromOptics->setChecked(true);
        checkBoxSfromOptics->setAutoExclusive(true);
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(50, 90, 231, 104));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 0, 0, 1, 2);

        spinBoxNameCol = new QSpinBox(layoutWidget);
        spinBoxNameCol->setObjectName("spinBoxNameCol");
        spinBoxNameCol->setValue(5);

        gridLayout_2->addWidget(spinBoxNameCol, 0, 2, 1, 1);

        label_12 = new QLabel(layoutWidget);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 1, 0, 1, 1);

        lineEditNamePrefix = new QLineEdit(layoutWidget);
        lineEditNamePrefix->setObjectName("lineEditNamePrefix");

        gridLayout_2->addWidget(lineEditNamePrefix, 1, 1, 1, 2);

        label_13 = new QLabel(layoutWidget);
        label_13->setObjectName("label_13");

        gridLayout_2->addWidget(label_13, 2, 0, 1, 1);

        lineEditfilter = new QLineEdit(layoutWidget);
        lineEditfilter->setObjectName("lineEditfilter");

        gridLayout_2->addWidget(lineEditfilter, 2, 1, 1, 2);

        checkBoxMatchCase = new QCheckBox(groupBox);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(30, 200, 111, 21));
        layoutWidget_1 = new QWidget(MeasuredDataDialog);
        layoutWidget_1->setObjectName("layoutWidget_1");
        layoutWidget_1->setGeometry(QRect(20, 50, 375, 284));
        gridLayout = new QGridLayout(layoutWidget_1);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget_1);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        spinBoxScol = new QSpinBox(layoutWidget_1);
        spinBoxScol->setObjectName("spinBoxScol");
        spinBoxScol->setMinimumSize(QSize(150, 0));
        spinBoxScol->setMaximumSize(QSize(150, 16777215));
        spinBoxScol->setMinimum(-1);
        spinBoxScol->setValue(1);

        gridLayout->addWidget(spinBoxScol, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget_1);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        spinBoxDataCol = new QSpinBox(layoutWidget_1);
        spinBoxDataCol->setObjectName("spinBoxDataCol");
        spinBoxDataCol->setMinimum(-1);
        spinBoxDataCol->setValue(2);

        gridLayout->addWidget(spinBoxDataCol, 1, 1, 1, 1);

        label_3 = new QLabel(layoutWidget_1);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        spinBoxErrmCol = new QSpinBox(layoutWidget_1);
        spinBoxErrmCol->setObjectName("spinBoxErrmCol");
        spinBoxErrmCol->setMinimum(-1);
        spinBoxErrmCol->setValue(3);

        gridLayout->addWidget(spinBoxErrmCol, 2, 1, 1, 1);

        label_4 = new QLabel(layoutWidget_1);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        spinBoxErrpCol = new QSpinBox(layoutWidget_1);
        spinBoxErrpCol->setObjectName("spinBoxErrpCol");
        spinBoxErrpCol->setMinimum(-1);
        spinBoxErrpCol->setValue(4);

        gridLayout->addWidget(spinBoxErrpCol, 3, 1, 1, 1);

        label_5 = new QLabel(layoutWidget_1);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        spinBoxColor = new QSpinBox(layoutWidget_1);
        spinBoxColor->setObjectName("spinBoxColor");
        spinBoxColor->setMinimum(0);
        spinBoxColor->setValue(4);

        gridLayout->addWidget(spinBoxColor, 4, 1, 1, 1);

        label_6 = new QLabel(layoutWidget_1);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        spinBoxCross = new QSpinBox(layoutWidget_1);
        spinBoxCross->setObjectName("spinBoxCross");
        spinBoxCross->setValue(4);

        gridLayout->addWidget(spinBoxCross, 5, 1, 1, 1);

        label_7 = new QLabel(layoutWidget_1);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 6, 0, 1, 1);

        doubleSpinBoxScaling = new ScientificDoubleSpinBox(layoutWidget_1);
        doubleSpinBoxScaling->setObjectName("doubleSpinBoxScaling");
        doubleSpinBoxScaling->setValue(1.000000000000000);

        gridLayout->addWidget(doubleSpinBoxScaling, 6, 1, 1, 1);

        label_8 = new QLabel(layoutWidget_1);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 7, 0, 1, 1);

        doubleSpinBoxXoffs = new QDoubleSpinBox(layoutWidget_1);
        doubleSpinBoxXoffs->setObjectName("doubleSpinBoxXoffs");

        gridLayout->addWidget(doubleSpinBoxXoffs, 7, 1, 1, 1);

        lineEditDataFile = new QLineEdit(MeasuredDataDialog);
        lineEditDataFile->setObjectName("lineEditDataFile");
        lineEditDataFile->setGeometry(QRect(70, 20, 251, 21));
        label_14 = new QLabel(MeasuredDataDialog);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(10, 30, 55, 13));
        pushButtonDataFile = new QPushButton(MeasuredDataDialog);
        pushButtonDataFile->setObjectName("pushButtonDataFile");
        pushButtonDataFile->setGeometry(QRect(330, 20, 41, 22));
        buttonBox = new QDialogButtonBox(MeasuredDataDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(70, 582, 211, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(MeasuredDataDialog);
        QObject::connect(pushButtonDataFile, SIGNAL(clicked()), MeasuredDataDialog, SLOT(getFileName()));
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, MeasuredDataDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, MeasuredDataDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(MeasuredDataDialog);
    } // setupUi

    void retranslateUi(QDialog *MeasuredDataDialog)
    {
        MeasuredDataDialog->setWindowTitle(QCoreApplication::translate("MeasuredDataDialog", "Measured Data", nullptr));
        label_10->setText(QString());
        groupBox->setTitle(QCoreApplication::translate("MeasuredDataDialog", "s-axis specification", nullptr));
        checkBoxRefToS0->setText(QCoreApplication::translate("MeasuredDataDialog", "Reference s-coordinate to s0", nullptr));
        checkBoxSfromOptics->setText(QCoreApplication::translate("MeasuredDataDialog", "Determine s-coordinate from element name", nullptr));
        label_11->setText(QCoreApplication::translate("MeasuredDataDialog", "Column no for element names", nullptr));
        label_12->setText(QCoreApplication::translate("MeasuredDataDialog", "Name prefix", nullptr));
        label_13->setText(QCoreApplication::translate("MeasuredDataDialog", "Filter for names", nullptr));
        lineEditfilter->setText(QCoreApplication::translate("MeasuredDataDialog", "*", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("MeasuredDataDialog", "Match case", nullptr));
        label->setText(QCoreApplication::translate("MeasuredDataDialog", "Column index for s-coordinate", nullptr));
        label_2->setText(QCoreApplication::translate("MeasuredDataDialog", "Column index for data", nullptr));
        label_3->setText(QCoreApplication::translate("MeasuredDataDialog", "Column index for data - error", nullptr));
        label_4->setText(QCoreApplication::translate("MeasuredDataDialog", "Column index for data + error", nullptr));
        label_5->setText(QCoreApplication::translate("MeasuredDataDialog", "Color [1-4]", nullptr));
        label_6->setText(QCoreApplication::translate("MeasuredDataDialog", "Symbol  [1-4]", nullptr));
        label_7->setText(QCoreApplication::translate("MeasuredDataDialog", "Scale factor", nullptr));
        label_8->setText(QCoreApplication::translate("MeasuredDataDialog", "Horizontal offset", nullptr));
        label_14->setText(QCoreApplication::translate("MeasuredDataDialog", "Data file", nullptr));
        pushButtonDataFile->setText(QCoreApplication::translate("MeasuredDataDialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MeasuredDataDialog: public Ui_MeasuredDataDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MEASURED_DATA_DIALOG_H
