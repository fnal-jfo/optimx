/********************************************************************************
** Form generated from reading UI file 'external_plot_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXTERNAL_PLOT_DIALOG_H
#define UI_EXTERNAL_PLOT_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExternalPlotDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditFileName;
    QLabel *label;
    QPushButton *pushButtonFileSelect;
    QLabel *label_2;
    QLineEdit *lineEditPlotCapture;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_3;
    QSpinBox *spinBoxXMin;
    QSpinBox *spinBoxXMax;
    QSpinBox *spinBoxXGridSize;
    QLabel *label_4;
    QSpinBox *spinBoxYLeftMin;
    QSpinBox *spinBoxYLeftMax;
    QSpinBox *spinBoxYLeftGridSize;
    QLabel *label_5;
    QSpinBox *spinBoxYRightMin;
    QSpinBox *spinBoxYRightMax;
    QSpinBox *spinBoxYRightGridSize;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_2;
    QCheckBox *checkBoxCurve1Cross;
    QCheckBox *checkBoxCurve3Line;
    QComboBox *comboBoxCurve2YSide;
    QLabel *label_14;
    QLabel *label_10;
    QLabel *label_9;
    QLabel *label_15;
    QLabel *label_16;
    QComboBox *comboBoxCurve4YSide;
    QLabel *label_17;
    QLineEdit *lineEditCurve1Legend;
    QComboBox *comboBoxCurve1YSide;
    QSpinBox *spinBoxXCol;
    QCheckBox *checkBoxCurve1Line;
    QLabel *label_12;
    QLabel *label_11;
    QLineEdit *lineEditCurve2Legend;
    QCheckBox *checkBoxCurve3Cross;
    QSpinBox *spinBoxCurve1Col;
    QSpinBox *spinBoxCurve2Col;
    QLineEdit *lineEditCurve4Legend;
    QCheckBox *checkBoxCurve2Line;
    QCheckBox *checkBoxCurve2Cross;
    QSpinBox *spinBoxCurve3Col;
    QLabel *label_13;
    QComboBox *comboBoxCurve3YSide;
    QLineEdit *lineEditCurve3Legend;
    QCheckBox *checkBoxCurve4Line;
    QCheckBox *checkBoxCurve4Cross;
    QSpinBox *spinBoxCurve4Col;
    QLabel *label_18;

    void setupUi(QDialog *ExternalPlotDialog)
    {
        if (ExternalPlotDialog->objectName().isEmpty())
            ExternalPlotDialog->setObjectName("ExternalPlotDialog");
        ExternalPlotDialog->resize(500, 500);
        ExternalPlotDialog->setMinimumSize(QSize(500, 500));
        ExternalPlotDialog->setMaximumSize(QSize(500, 500));
        buttonBox = new QDialogButtonBox(ExternalPlotDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(150, 440, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEditFileName = new QLineEdit(ExternalPlotDialog);
        lineEditFileName->setObjectName("lineEditFileName");
        lineEditFileName->setGeometry(QRect(120, 50, 271, 21));
        label = new QLabel(ExternalPlotDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 50, 71, 16));
        pushButtonFileSelect = new QPushButton(ExternalPlotDialog);
        pushButtonFileSelect->setObjectName("pushButtonFileSelect");
        pushButtonFileSelect->setGeometry(QRect(410, 50, 51, 22));
        label_2 = new QLabel(ExternalPlotDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 90, 81, 16));
        lineEditPlotCapture = new QLineEdit(ExternalPlotDialog);
        lineEditPlotCapture->setObjectName("lineEditPlotCapture");
        lineEditPlotCapture->setGeometry(QRect(120, 90, 271, 21));
        layoutWidget = new QWidget(ExternalPlotDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(80, 130, 331, 111));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 0, 1, 1, 1);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 0, 2, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 0, 3, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        spinBoxXMin = new QSpinBox(layoutWidget);
        spinBoxXMin->setObjectName("spinBoxXMin");

        gridLayout->addWidget(spinBoxXMin, 1, 1, 1, 1);

        spinBoxXMax = new QSpinBox(layoutWidget);
        spinBoxXMax->setObjectName("spinBoxXMax");

        gridLayout->addWidget(spinBoxXMax, 1, 2, 1, 1);

        spinBoxXGridSize = new QSpinBox(layoutWidget);
        spinBoxXGridSize->setObjectName("spinBoxXGridSize");

        gridLayout->addWidget(spinBoxXGridSize, 1, 3, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 2, 0, 1, 1);

        spinBoxYLeftMin = new QSpinBox(layoutWidget);
        spinBoxYLeftMin->setObjectName("spinBoxYLeftMin");

        gridLayout->addWidget(spinBoxYLeftMin, 2, 1, 1, 1);

        spinBoxYLeftMax = new QSpinBox(layoutWidget);
        spinBoxYLeftMax->setObjectName("spinBoxYLeftMax");

        gridLayout->addWidget(spinBoxYLeftMax, 2, 2, 1, 1);

        spinBoxYLeftGridSize = new QSpinBox(layoutWidget);
        spinBoxYLeftGridSize->setObjectName("spinBoxYLeftGridSize");

        gridLayout->addWidget(spinBoxYLeftGridSize, 2, 3, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 3, 0, 1, 1);

        spinBoxYRightMin = new QSpinBox(layoutWidget);
        spinBoxYRightMin->setObjectName("spinBoxYRightMin");

        gridLayout->addWidget(spinBoxYRightMin, 3, 1, 1, 1);

        spinBoxYRightMax = new QSpinBox(layoutWidget);
        spinBoxYRightMax->setObjectName("spinBoxYRightMax");

        gridLayout->addWidget(spinBoxYRightMax, 3, 2, 1, 1);

        spinBoxYRightGridSize = new QSpinBox(layoutWidget);
        spinBoxYRightGridSize->setObjectName("spinBoxYRightGridSize");

        gridLayout->addWidget(spinBoxYRightGridSize, 3, 3, 1, 1);

        layoutWidget1 = new QWidget(ExternalPlotDialog);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(60, 260, 369, 174));
        gridLayout_2 = new QGridLayout(layoutWidget1);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        checkBoxCurve1Cross = new QCheckBox(layoutWidget1);
        checkBoxCurve1Cross->setObjectName("checkBoxCurve1Cross");

        gridLayout_2->addWidget(checkBoxCurve1Cross, 3, 5, 1, 1);

        checkBoxCurve3Line = new QCheckBox(layoutWidget1);
        checkBoxCurve3Line->setObjectName("checkBoxCurve3Line");

        gridLayout_2->addWidget(checkBoxCurve3Line, 7, 4, 1, 1);

        comboBoxCurve2YSide = new QComboBox(layoutWidget1);
        comboBoxCurve2YSide->addItem(QString());
        comboBoxCurve2YSide->addItem(QString());
        comboBoxCurve2YSide->setObjectName("comboBoxCurve2YSide");

        gridLayout_2->addWidget(comboBoxCurve2YSide, 5, 2, 2, 1);

        label_14 = new QLabel(layoutWidget1);
        label_14->setObjectName("label_14");

        gridLayout_2->addWidget(label_14, 0, 2, 1, 1);

        label_10 = new QLabel(layoutWidget1);
        label_10->setObjectName("label_10");

        gridLayout_2->addWidget(label_10, 9, 0, 1, 1);

        label_9 = new QLabel(layoutWidget1);
        label_9->setObjectName("label_9");

        gridLayout_2->addWidget(label_9, 2, 0, 1, 1);

        label_15 = new QLabel(layoutWidget1);
        label_15->setObjectName("label_15");

        gridLayout_2->addWidget(label_15, 0, 5, 1, 1);

        label_16 = new QLabel(layoutWidget1);
        label_16->setObjectName("label_16");

        gridLayout_2->addWidget(label_16, 0, 3, 1, 1);

        comboBoxCurve4YSide = new QComboBox(layoutWidget1);
        comboBoxCurve4YSide->addItem(QString());
        comboBoxCurve4YSide->addItem(QString());
        comboBoxCurve4YSide->setObjectName("comboBoxCurve4YSide");

        gridLayout_2->addWidget(comboBoxCurve4YSide, 9, 2, 1, 1);

        label_17 = new QLabel(layoutWidget1);
        label_17->setObjectName("label_17");

        gridLayout_2->addWidget(label_17, 0, 4, 1, 1);

        lineEditCurve1Legend = new QLineEdit(layoutWidget1);
        lineEditCurve1Legend->setObjectName("lineEditCurve1Legend");

        gridLayout_2->addWidget(lineEditCurve1Legend, 3, 3, 1, 1);

        comboBoxCurve1YSide = new QComboBox(layoutWidget1);
        comboBoxCurve1YSide->addItem(QString());
        comboBoxCurve1YSide->addItem(QString());
        comboBoxCurve1YSide->setObjectName("comboBoxCurve1YSide");

        gridLayout_2->addWidget(comboBoxCurve1YSide, 3, 2, 2, 1);

        spinBoxXCol = new QSpinBox(layoutWidget1);
        spinBoxXCol->setObjectName("spinBoxXCol");

        gridLayout_2->addWidget(spinBoxXCol, 2, 1, 1, 1);

        checkBoxCurve1Line = new QCheckBox(layoutWidget1);
        checkBoxCurve1Line->setObjectName("checkBoxCurve1Line");

        gridLayout_2->addWidget(checkBoxCurve1Line, 3, 4, 1, 1);

        label_12 = new QLabel(layoutWidget1);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 5, 0, 1, 1);

        label_11 = new QLabel(layoutWidget1);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 3, 0, 1, 1);

        lineEditCurve2Legend = new QLineEdit(layoutWidget1);
        lineEditCurve2Legend->setObjectName("lineEditCurve2Legend");

        gridLayout_2->addWidget(lineEditCurve2Legend, 5, 3, 1, 1);

        checkBoxCurve3Cross = new QCheckBox(layoutWidget1);
        checkBoxCurve3Cross->setObjectName("checkBoxCurve3Cross");

        gridLayout_2->addWidget(checkBoxCurve3Cross, 7, 5, 1, 1);

        spinBoxCurve1Col = new QSpinBox(layoutWidget1);
        spinBoxCurve1Col->setObjectName("spinBoxCurve1Col");

        gridLayout_2->addWidget(spinBoxCurve1Col, 3, 1, 1, 1);

        spinBoxCurve2Col = new QSpinBox(layoutWidget1);
        spinBoxCurve2Col->setObjectName("spinBoxCurve2Col");

        gridLayout_2->addWidget(spinBoxCurve2Col, 4, 1, 2, 1);

        lineEditCurve4Legend = new QLineEdit(layoutWidget1);
        lineEditCurve4Legend->setObjectName("lineEditCurve4Legend");

        gridLayout_2->addWidget(lineEditCurve4Legend, 9, 3, 1, 1);

        checkBoxCurve2Line = new QCheckBox(layoutWidget1);
        checkBoxCurve2Line->setObjectName("checkBoxCurve2Line");

        gridLayout_2->addWidget(checkBoxCurve2Line, 5, 4, 1, 1);

        checkBoxCurve2Cross = new QCheckBox(layoutWidget1);
        checkBoxCurve2Cross->setObjectName("checkBoxCurve2Cross");

        gridLayout_2->addWidget(checkBoxCurve2Cross, 5, 5, 1, 1);

        spinBoxCurve3Col = new QSpinBox(layoutWidget1);
        spinBoxCurve3Col->setObjectName("spinBoxCurve3Col");

        gridLayout_2->addWidget(spinBoxCurve3Col, 6, 1, 2, 1);

        label_13 = new QLabel(layoutWidget1);
        label_13->setObjectName("label_13");

        gridLayout_2->addWidget(label_13, 7, 0, 1, 1);

        comboBoxCurve3YSide = new QComboBox(layoutWidget1);
        comboBoxCurve3YSide->addItem(QString());
        comboBoxCurve3YSide->addItem(QString());
        comboBoxCurve3YSide->setObjectName("comboBoxCurve3YSide");

        gridLayout_2->addWidget(comboBoxCurve3YSide, 7, 2, 2, 1);

        lineEditCurve3Legend = new QLineEdit(layoutWidget1);
        lineEditCurve3Legend->setObjectName("lineEditCurve3Legend");

        gridLayout_2->addWidget(lineEditCurve3Legend, 7, 3, 1, 1);

        checkBoxCurve4Line = new QCheckBox(layoutWidget1);
        checkBoxCurve4Line->setObjectName("checkBoxCurve4Line");

        gridLayout_2->addWidget(checkBoxCurve4Line, 9, 4, 1, 1);

        checkBoxCurve4Cross = new QCheckBox(layoutWidget1);
        checkBoxCurve4Cross->setObjectName("checkBoxCurve4Cross");

        gridLayout_2->addWidget(checkBoxCurve4Cross, 9, 5, 1, 1);

        spinBoxCurve4Col = new QSpinBox(layoutWidget1);
        spinBoxCurve4Col->setObjectName("spinBoxCurve4Col");

        gridLayout_2->addWidget(spinBoxCurve4Col, 8, 1, 2, 1);

        label_18 = new QLabel(layoutWidget1);
        label_18->setObjectName("label_18");

        gridLayout_2->addWidget(label_18, 0, 1, 1, 1);


        retranslateUi(ExternalPlotDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ExternalPlotDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ExternalPlotDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButtonFileSelect, SIGNAL(clicked()), ExternalPlotDialog, SLOT(fileSelect()));

        QMetaObject::connectSlotsByName(ExternalPlotDialog);
    } // setupUi

    void retranslateUi(QDialog *ExternalPlotDialog)
    {
        ExternalPlotDialog->setWindowTitle(QCoreApplication::translate("ExternalPlotDialog", "External Plot Dialog", nullptr));
        label->setText(QCoreApplication::translate("ExternalPlotDialog", "File Name", nullptr));
        pushButtonFileSelect->setText(QCoreApplication::translate("ExternalPlotDialog", "...", nullptr));
        label_2->setText(QCoreApplication::translate("ExternalPlotDialog", "Plot Capture", nullptr));
        label_6->setText(QCoreApplication::translate("ExternalPlotDialog", "Min", nullptr));
        label_7->setText(QCoreApplication::translate("ExternalPlotDialog", "Max", nullptr));
        label_8->setText(QCoreApplication::translate("ExternalPlotDialog", "Grid size", nullptr));
        label_3->setText(QCoreApplication::translate("ExternalPlotDialog", "x-axis", nullptr));
        label_4->setText(QCoreApplication::translate("ExternalPlotDialog", "y-axis (left)", nullptr));
        label_5->setText(QCoreApplication::translate("ExternalPlotDialog", "y-axis (right)", nullptr));
        checkBoxCurve1Cross->setText(QString());
        checkBoxCurve3Line->setText(QString());
        comboBoxCurve2YSide->setItemText(0, QCoreApplication::translate("ExternalPlotDialog", "L", nullptr));
        comboBoxCurve2YSide->setItemText(1, QCoreApplication::translate("ExternalPlotDialog", "R", nullptr));

        label_14->setText(QCoreApplication::translate("ExternalPlotDialog", "y-axis (R,L)", nullptr));
        label_10->setText(QCoreApplication::translate("ExternalPlotDialog", "Curve 4:", nullptr));
        label_9->setText(QCoreApplication::translate("ExternalPlotDialog", "x", nullptr));
        label_15->setText(QCoreApplication::translate("ExternalPlotDialog", "Cross", nullptr));
        label_16->setText(QCoreApplication::translate("ExternalPlotDialog", "Legend", nullptr));
        comboBoxCurve4YSide->setItemText(0, QCoreApplication::translate("ExternalPlotDialog", "L", nullptr));
        comboBoxCurve4YSide->setItemText(1, QCoreApplication::translate("ExternalPlotDialog", "R", nullptr));

        label_17->setText(QCoreApplication::translate("ExternalPlotDialog", "Line", nullptr));
        comboBoxCurve1YSide->setItemText(0, QCoreApplication::translate("ExternalPlotDialog", "L", nullptr));
        comboBoxCurve1YSide->setItemText(1, QCoreApplication::translate("ExternalPlotDialog", "R", nullptr));

        checkBoxCurve1Line->setText(QString());
        label_12->setText(QCoreApplication::translate("ExternalPlotDialog", "Curve 2:", nullptr));
        label_11->setText(QCoreApplication::translate("ExternalPlotDialog", "Curve 1:", nullptr));
        checkBoxCurve3Cross->setText(QString());
        checkBoxCurve2Line->setText(QString());
        checkBoxCurve2Cross->setText(QString());
        label_13->setText(QCoreApplication::translate("ExternalPlotDialog", "Curve 3:", nullptr));
        comboBoxCurve3YSide->setItemText(0, QCoreApplication::translate("ExternalPlotDialog", "L", nullptr));
        comboBoxCurve3YSide->setItemText(1, QCoreApplication::translate("ExternalPlotDialog", "R", nullptr));

        checkBoxCurve4Line->setText(QString());
        checkBoxCurve4Cross->setText(QString());
        label_18->setText(QCoreApplication::translate("ExternalPlotDialog", "Column (1-20)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ExternalPlotDialog: public Ui_ExternalPlotDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTERNAL_PLOT_DIALOG_H
