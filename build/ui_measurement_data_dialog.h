/********************************************************************************
** Form generated from reading UI file 'measurement_data_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MEASUREMENT_DATA_DIALOG_H
#define UI_MEASUREMENT_DATA_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MeasuredDataDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_10;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *spinBox;
    QLabel *label_2;
    QSpinBox *spinBox_3;
    QLabel *label_3;
    QSpinBox *spinBox_4;
    QLabel *label_4;
    QSpinBox *spinBox_5;
    QLabel *label_5;
    QSpinBox *spinBox_6;
    QLabel *label_6;
    QSpinBox *spinBox_7;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBox_2;
    QGroupBox *groupBox;
    QCheckBox *checkBox;
    QLabel *label_9;
    QGroupBox *groupBox_3;
    QWidget *layoutWidget_2;
    QGridLayout *gridLayout_2;
    QLabel *label_11;
    QSpinBox *spinBox_2;
    QLabel *label_12;
    QLineEdit *lineEdit_2;
    QLabel *label_13;
    QLineEdit *lineEdit;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;

    void setupUi(QDialog *MeasuredDataDialog)
    {
        if (MeasuredDataDialog->objectName().isEmpty())
            MeasuredDataDialog->setObjectName("MeasuredDataDialog");
        MeasuredDataDialog->resize(410, 609);
        buttonBox = new QDialogButtonBox(MeasuredDataDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(120, 560, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_10 = new QLabel(MeasuredDataDialog);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(30, 460, 55, 13));
        layoutWidget = new QWidget(MeasuredDataDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 10, 371, 268));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        spinBox = new QSpinBox(layoutWidget);
        spinBox->setObjectName("spinBox");

        gridLayout->addWidget(spinBox, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        spinBox_3 = new QSpinBox(layoutWidget);
        spinBox_3->setObjectName("spinBox_3");

        gridLayout->addWidget(spinBox_3, 1, 1, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        spinBox_4 = new QSpinBox(layoutWidget);
        spinBox_4->setObjectName("spinBox_4");

        gridLayout->addWidget(spinBox_4, 2, 1, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        spinBox_5 = new QSpinBox(layoutWidget);
        spinBox_5->setObjectName("spinBox_5");

        gridLayout->addWidget(spinBox_5, 3, 1, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        spinBox_6 = new QSpinBox(layoutWidget);
        spinBox_6->setObjectName("spinBox_6");

        gridLayout->addWidget(spinBox_6, 4, 1, 1, 1);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        spinBox_7 = new QSpinBox(layoutWidget);
        spinBox_7->setObjectName("spinBox_7");

        gridLayout->addWidget(spinBox_7, 5, 1, 1, 1);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 6, 0, 1, 1);

        doubleSpinBox = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox->setObjectName("doubleSpinBox");

        gridLayout->addWidget(doubleSpinBox, 6, 1, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 7, 0, 1, 1);

        doubleSpinBox_2 = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_2->setObjectName("doubleSpinBox_2");

        gridLayout->addWidget(doubleSpinBox_2, 7, 1, 1, 1);

        groupBox = new QGroupBox(MeasuredDataDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 290, 381, 271));
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName("checkBox");
        checkBox->setGeometry(QRect(80, 30, 211, 21));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(170, 60, 55, 21));
        groupBox_3 = new QGroupBox(groupBox);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(20, 80, 341, 181));
        layoutWidget_2 = new QWidget(groupBox_3);
        layoutWidget_2->setObjectName("layoutWidget_2");
        layoutWidget_2->setGeometry(QRect(50, 40, 234, 98));
        gridLayout_2 = new QGridLayout(layoutWidget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(layoutWidget_2);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 0, 0, 1, 2);

        spinBox_2 = new QSpinBox(layoutWidget_2);
        spinBox_2->setObjectName("spinBox_2");

        gridLayout_2->addWidget(spinBox_2, 0, 2, 1, 1);

        label_12 = new QLabel(layoutWidget_2);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 1, 0, 1, 1);

        lineEdit_2 = new QLineEdit(layoutWidget_2);
        lineEdit_2->setObjectName("lineEdit_2");

        gridLayout_2->addWidget(lineEdit_2, 1, 1, 1, 2);

        label_13 = new QLabel(layoutWidget_2);
        label_13->setObjectName("label_13");

        gridLayout_2->addWidget(label_13, 2, 0, 1, 1);

        lineEdit = new QLineEdit(layoutWidget_2);
        lineEdit->setObjectName("lineEdit");

        gridLayout_2->addWidget(lineEdit, 2, 1, 1, 2);

        checkBox_2 = new QCheckBox(groupBox_3);
        checkBox_2->setObjectName("checkBox_2");
        checkBox_2->setGeometry(QRect(20, 10, 301, 21));
        checkBox_3 = new QCheckBox(groupBox_3);
        checkBox_3->setObjectName("checkBox_3");
        checkBox_3->setGeometry(QRect(20, 150, 111, 21));

        retranslateUi(MeasuredDataDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, MeasuredDataDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, MeasuredDataDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(MeasuredDataDialog);
    } // setupUi

    void retranslateUi(QDialog *MeasuredDataDialog)
    {
        MeasuredDataDialog->setWindowTitle(QCoreApplication::translate("MeasuredDataDialog", "Measured Data", nullptr));
        label_10->setText(QString());
        label->setText(QCoreApplication::translate("MeasuredDataDialog", "Column index for s-coordinate", nullptr));
        label_2->setText(QCoreApplication::translate("MeasuredDataDialog", "Column index for data", nullptr));
        label_3->setText(QCoreApplication::translate("MeasuredDataDialog", "Column index for data - error", nullptr));
        label_4->setText(QCoreApplication::translate("MeasuredDataDialog", "Column index for data + error", nullptr));
        label_5->setText(QCoreApplication::translate("MeasuredDataDialog", "Color [1-4]", nullptr));
        label_6->setText(QCoreApplication::translate("MeasuredDataDialog", "Cross type [1-4]", nullptr));
        label_7->setText(QCoreApplication::translate("MeasuredDataDialog", "Scale factor", nullptr));
        label_8->setText(QCoreApplication::translate("MeasuredDataDialog", "Horizontal offset", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MeasuredDataDialog", "s-axis specification", nullptr));
        checkBox->setText(QCoreApplication::translate("MeasuredDataDialog", "Reference s-coordinate to s0", nullptr));
        label_9->setText(QCoreApplication::translate("MeasuredDataDialog", "OR", nullptr));
        groupBox_3->setTitle(QString());
        label_11->setText(QCoreApplication::translate("MeasuredDataDialog", "Column no for element names", nullptr));
        label_12->setText(QCoreApplication::translate("MeasuredDataDialog", "Name prefix", nullptr));
        label_13->setText(QCoreApplication::translate("MeasuredDataDialog", "Filter for names", nullptr));
        checkBox_2->setText(QCoreApplication::translate("MeasuredDataDialog", "Determine s-coordinate from element name", nullptr));
        checkBox_3->setText(QCoreApplication::translate("MeasuredDataDialog", "Match case", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MeasuredDataDialog: public Ui_MeasuredDataDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MEASUREMENT_DATA_DIALOG_H
