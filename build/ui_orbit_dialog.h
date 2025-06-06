/********************************************************************************
** Form generated from reading UI file 'orbit_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ORBIT_DIALOG_H
#define UI_ORBIT_DIALOG_H

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
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_OrbitDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    ScientificDoubleSpinBox *doubleSpinBoxX0;
    QLabel *label_2;
    ScientificDoubleSpinBox *doubleSpinBoxY0;
    QLabel *label_3;
    ScientificDoubleSpinBox *doubleSpinBoxZ0;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBoxXp0;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBoxYp0;
    QLabel *label_5;
    ScientificDoubleSpinBox *doubleSpinBoxS0;
    QCheckBox *checkBoxNoPrintFirst;
    QCheckBox *checkBoxInitMatchCase;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QLabel *label_7;
    QLineEdit *lineEditFilter;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxStepSize;
    QCheckBox *checkBoxUseMADFrame;
    QCheckBox *checkBoxInitFromFile;

    void setupUi(QDialog *OrbitDialog)
    {
        if (OrbitDialog->objectName().isEmpty())
            OrbitDialog->setObjectName("OrbitDialog");
        OrbitDialog->resize(350, 600);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(OrbitDialog->sizePolicy().hasHeightForWidth());
        OrbitDialog->setSizePolicy(sizePolicy);
        OrbitDialog->setMinimumSize(QSize(350, 600));
        OrbitDialog->setMaximumSize(QSize(400, 600));
        QFont font;
        font.setPointSize(8);
        OrbitDialog->setFont(font);
        buttonBox = new QDialogButtonBox(OrbitDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(75, 535, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(OrbitDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 20, 301, 251));
        groupBox->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(30, 20, 251, 212));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        doubleSpinBoxX0 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxX0->setObjectName("doubleSpinBoxX0");
        doubleSpinBoxX0->setDecimals(4);
        doubleSpinBoxX0->setMinimum(-10000.000000000000000);
        doubleSpinBoxX0->setMaximum(10000.000000000000000);
        doubleSpinBoxX0->setSingleStep(0.100000000000000);
        doubleSpinBoxX0->setValue(0.000000000000000);

        gridLayout->addWidget(doubleSpinBoxX0, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        doubleSpinBoxY0 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxY0->setObjectName("doubleSpinBoxY0");
        doubleSpinBoxY0->setDecimals(4);
        doubleSpinBoxY0->setMinimum(-10000.000000000000000);
        doubleSpinBoxY0->setMaximum(10000.000000000000000);
        doubleSpinBoxY0->setSingleStep(0.010000000000000);

        gridLayout->addWidget(doubleSpinBoxY0, 1, 1, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        doubleSpinBoxZ0 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxZ0->setObjectName("doubleSpinBoxZ0");
        doubleSpinBoxZ0->setDecimals(4);
        doubleSpinBoxZ0->setMinimum(-10000.000000000000000);
        doubleSpinBoxZ0->setMaximum(10000.000000000000000);
        doubleSpinBoxZ0->setSingleStep(0.010000000000000);

        gridLayout->addWidget(doubleSpinBoxZ0, 2, 1, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        doubleSpinBoxXp0 = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxXp0->setObjectName("doubleSpinBoxXp0");
        doubleSpinBoxXp0->setDecimals(4);
        doubleSpinBoxXp0->setMinimum(-180.000000000000000);
        doubleSpinBoxXp0->setMaximum(180.000000000000000);
        doubleSpinBoxXp0->setSingleStep(0.010000000000000);

        gridLayout->addWidget(doubleSpinBoxXp0, 3, 1, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 4, 0, 1, 1);

        doubleSpinBoxYp0 = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxYp0->setObjectName("doubleSpinBoxYp0");
        doubleSpinBoxYp0->setDecimals(4);
        doubleSpinBoxYp0->setMinimum(-180.000000000000000);
        doubleSpinBoxYp0->setMaximum(180.000000000000000);
        doubleSpinBoxYp0->setSingleStep(0.010000000000000);

        gridLayout->addWidget(doubleSpinBoxYp0, 4, 1, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        doubleSpinBoxS0 = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxS0->setObjectName("doubleSpinBoxS0");
        doubleSpinBoxS0->setDecimals(4);
        doubleSpinBoxS0->setMinimum(-10000.000000000000000);
        doubleSpinBoxS0->setMaximum(10000.000000000000000);
        doubleSpinBoxS0->setSingleStep(0.010000000000000);

        gridLayout->addWidget(doubleSpinBoxS0, 5, 1, 1, 1);

        checkBoxNoPrintFirst = new QCheckBox(OrbitDialog);
        checkBoxNoPrintFirst->setObjectName("checkBoxNoPrintFirst");
        checkBoxNoPrintFirst->setGeometry(QRect(30, 417, 251, 31));
        checkBoxInitMatchCase = new QCheckBox(OrbitDialog);
        checkBoxInitMatchCase->setObjectName("checkBoxInitMatchCase");
        checkBoxInitMatchCase->setGeometry(QRect(32, 499, 101, 21));
        layoutWidget1 = new QWidget(OrbitDialog);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(29, 455, 251, 32));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(layoutWidget1);
        label_7->setObjectName("label_7");

        horizontalLayout->addWidget(label_7);

        lineEditFilter = new QLineEdit(layoutWidget1);
        lineEditFilter->setObjectName("lineEditFilter");

        horizontalLayout->addWidget(lineEditFilter);

        layoutWidget2 = new QWidget(OrbitDialog);
        layoutWidget2->setObjectName("layoutWidget2");
        layoutWidget2->setGeometry(QRect(32, 379, 245, 32));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_6 = new QLabel(layoutWidget2);
        label_6->setObjectName("label_6");

        horizontalLayout_2->addWidget(label_6);

        doubleSpinBoxStepSize = new QDoubleSpinBox(layoutWidget2);
        doubleSpinBoxStepSize->setObjectName("doubleSpinBoxStepSize");
        doubleSpinBoxStepSize->setDecimals(4);
        doubleSpinBoxStepSize->setMaximum(100000.000000000000000);

        horizontalLayout_2->addWidget(doubleSpinBoxStepSize);

        checkBoxUseMADFrame = new QCheckBox(OrbitDialog);
        checkBoxUseMADFrame->setObjectName("checkBoxUseMADFrame");
        checkBoxUseMADFrame->setGeometry(QRect(31, 328, 241, 31));
        checkBoxInitFromFile = new QCheckBox(OrbitDialog);
        checkBoxInitFromFile->setObjectName("checkBoxInitFromFile");
        checkBoxInitFromFile->setGeometry(QRect(34, 283, 241, 31));

        retranslateUi(OrbitDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, OrbitDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, OrbitDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(OrbitDialog);
    } // setupUi

    void retranslateUi(QDialog *OrbitDialog)
    {
        OrbitDialog->setWindowTitle(QCoreApplication::translate("OrbitDialog", "Orbit Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("OrbitDialog", "Initial", nullptr));
        label->setText(QCoreApplication::translate("OrbitDialog", "X [cm]   ", nullptr));
        label_2->setText(QCoreApplication::translate("OrbitDialog", "Y [cm]   ", nullptr));
        label_3->setText(QCoreApplication::translate("OrbitDialog", "Z [cm]   ", nullptr));
        label_4->setText(QCoreApplication::translate("OrbitDialog", "X angle [deg]   ", nullptr));
        label_8->setText(QCoreApplication::translate("OrbitDialog", "Y angle [deg]   ", nullptr));
        label_5->setText(QCoreApplication::translate("OrbitDialog", "Path Length [cm]   ", nullptr));
        checkBoxNoPrintFirst->setText(QCoreApplication::translate("OrbitDialog", "Omit first character in element names", nullptr));
        checkBoxInitMatchCase->setText(QCoreApplication::translate("OrbitDialog", "Match case", nullptr));
        label_7->setText(QCoreApplication::translate("OrbitDialog", "Filter [*, %, ... ]", nullptr));
        label_6->setText(QCoreApplication::translate("OrbitDialog", "Step size [cm]", nullptr));
        checkBoxUseMADFrame->setText(QCoreApplication::translate("OrbitDialog", "Use MAD  local frame convention ", nullptr));
        checkBoxInitFromFile->setText(QCoreApplication::translate("OrbitDialog", "Use  the initial coordinates specified\n"
"in the input file ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OrbitDialog: public Ui_OrbitDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ORBIT_DIALOG_H
