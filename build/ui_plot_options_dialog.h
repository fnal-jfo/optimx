/********************************************************************************
** Form generated from reading UI file 'plot_options_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOT_OPTIONS_DIALOG_H
#define UI_PLOT_OPTIONS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_PlotOptionsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditTitle;
    QLabel *label;
    QGroupBox *groupBoxLego;
    QRadioButton *radioButtonLegoHide;
    QRadioButton *radioButtonLegoBottom;
    QRadioButton *radioButtonLegoTop;
    QTabWidget *tabWidget;
    QWidget *tabBottom;
    QLabel *label_2;
    QLineEdit *lineEditBottomAxisLabel;
    QGroupBox *groupBox_4;
    QWidget *layoutWidget_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QRadioButton *radioButtonAutoBottom;
    QSpacerItem *horizontalSpacer_2;
    QRadioButton *radioButtonFixedBottom;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    ScientificDoubleSpinBox *doubleSpinBoxMinBottom;
    QSpacerItem *horizontalSpacer;
    QLabel *label_4;
    ScientificDoubleSpinBox *doubleSpinBoxMaxBottom;
    QCheckBox *checkBoxZoomXBottom;
    QWidget *tabRight;
    QGroupBox *groupBox_3;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_6;
    QRadioButton *radioButtonAutoRight;
    QSpacerItem *horizontalSpacer_6;
    QRadioButton *radioButtonFixedRight;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_7;
    ScientificDoubleSpinBox *doubleSpinBoxMinRight;
    QLabel *label_8;
    ScientificDoubleSpinBox *doubleSpinBoxMaxRight;
    QLabel *label_18;
    QLineEdit *lineEditRightAxisLabel;
    QCheckBox *checkBoxRightActive;
    QCheckBox *checkBoxZoomYRight;
    QWidget *tabLeft;
    QGroupBox *groupBox_2;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *radioButtonAutoLeft;
    QSpacerItem *horizontalSpacer_4;
    QRadioButton *radioButtonFixedLeft;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    ScientificDoubleSpinBox *doubleSpinBoxMinLeft;
    QLabel *label_6;
    ScientificDoubleSpinBox *doubleSpinBoxMaxLeft;
    QLineEdit *lineEditLeftAxisLabel;
    QLabel *label_9;
    QCheckBox *checkBoxZoomYLeft;
    QCheckBox *checkBoxLeftActive;

    void setupUi(QDialog *PlotOptionsDialog)
    {
        if (PlotOptionsDialog->objectName().isEmpty())
            PlotOptionsDialog->setObjectName("PlotOptionsDialog");
        PlotOptionsDialog->resize(471, 455);
        buttonBox = new QDialogButtonBox(PlotOptionsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(90, 410, 191, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEditTitle = new QLineEdit(PlotOptionsDialog);
        lineEditTitle->setObjectName("lineEditTitle");
        lineEditTitle->setGeometry(QRect(20, 370, 421, 21));
        label = new QLabel(PlotOptionsDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 350, 55, 13));
        groupBoxLego = new QGroupBox(PlotOptionsDialog);
        groupBoxLego->setObjectName("groupBoxLego");
        groupBoxLego->setGeometry(QRect(70, 290, 331, 51));
        radioButtonLegoHide = new QRadioButton(groupBoxLego);
        radioButtonLegoHide->setObjectName("radioButtonLegoHide");
        radioButtonLegoHide->setGeometry(QRect(210, 20, 93, 21));
        radioButtonLegoBottom = new QRadioButton(groupBoxLego);
        radioButtonLegoBottom->setObjectName("radioButtonLegoBottom");
        radioButtonLegoBottom->setGeometry(QRect(100, 20, 93, 21));
        radioButtonLegoBottom->setChecked(false);
        radioButtonLegoBottom->setAutoExclusive(true);
        radioButtonLegoTop = new QRadioButton(groupBoxLego);
        radioButtonLegoTop->setObjectName("radioButtonLegoTop");
        radioButtonLegoTop->setGeometry(QRect(10, 20, 93, 21));
        radioButtonLegoTop->setChecked(false);
        tabWidget = new QTabWidget(PlotOptionsDialog);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(20, 20, 421, 251));
        tabBottom = new QWidget();
        tabBottom->setObjectName("tabBottom");
        label_2 = new QLabel(tabBottom);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 180, 71, 16));
        lineEditBottomAxisLabel = new QLineEdit(tabBottom);
        lineEditBottomAxisLabel->setObjectName("lineEditBottomAxisLabel");
        lineEditBottomAxisLabel->setGeometry(QRect(100, 170, 201, 31));
        groupBox_4 = new QGroupBox(tabBottom);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setGeometry(QRect(10, 30, 400, 120));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy);
        groupBox_4->setMinimumSize(QSize(400, 120));
        groupBox_4->setMaximumSize(QSize(400, 120));
        groupBox_4->setCheckable(false);
        layoutWidget_3 = new QWidget(groupBox_4);
        layoutWidget_3->setObjectName("layoutWidget_3");
        layoutWidget_3->setGeometry(QRect(10, 30, 369, 78));
        verticalLayout = new QVBoxLayout(layoutWidget_3);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        radioButtonAutoBottom = new QRadioButton(layoutWidget_3);
        radioButtonAutoBottom->setObjectName("radioButtonAutoBottom");
        radioButtonAutoBottom->setCheckable(true);
        radioButtonAutoBottom->setChecked(true);

        horizontalLayout->addWidget(radioButtonAutoBottom);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        radioButtonFixedBottom = new QRadioButton(layoutWidget_3);
        radioButtonFixedBottom->setObjectName("radioButtonFixedBottom");

        horizontalLayout->addWidget(radioButtonFixedBottom);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_3 = new QLabel(layoutWidget_3);
        label_3->setObjectName("label_3");

        horizontalLayout_2->addWidget(label_3);

        doubleSpinBoxMinBottom = new ScientificDoubleSpinBox(layoutWidget_3);
        doubleSpinBoxMinBottom->setObjectName("doubleSpinBoxMinBottom");
        doubleSpinBoxMinBottom->setEnabled(false);
        doubleSpinBoxMinBottom->setDecimals(6);
        doubleSpinBoxMinBottom->setMinimum(-999999.000000000000000);
        doubleSpinBoxMinBottom->setMaximum(999999.000000000000000);

        horizontalLayout_2->addWidget(doubleSpinBoxMinBottom);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label_4 = new QLabel(layoutWidget_3);
        label_4->setObjectName("label_4");

        horizontalLayout_2->addWidget(label_4);

        doubleSpinBoxMaxBottom = new ScientificDoubleSpinBox(layoutWidget_3);
        doubleSpinBoxMaxBottom->setObjectName("doubleSpinBoxMaxBottom");
        doubleSpinBoxMaxBottom->setEnabled(false);
        doubleSpinBoxMaxBottom->setDecimals(6);
        doubleSpinBoxMaxBottom->setMinimum(-1000000.000000000000000);
        doubleSpinBoxMaxBottom->setMaximum(100000.000000000000000);

        horizontalLayout_2->addWidget(doubleSpinBoxMaxBottom);


        verticalLayout->addLayout(horizontalLayout_2);

        checkBoxZoomXBottom = new QCheckBox(tabBottom);
        checkBoxZoomXBottom->setObjectName("checkBoxZoomXBottom");
        checkBoxZoomXBottom->setGeometry(QRect(200, 10, 111, 21));
        tabWidget->addTab(tabBottom, QString());
        tabRight = new QWidget();
        tabRight->setObjectName("tabRight");
        groupBox_3 = new QGroupBox(tabRight);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(10, 40, 381, 120));
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy1);
        groupBox_3->setMinimumSize(QSize(300, 120));
        groupBox_3->setMaximumSize(QSize(400, 120));
        layoutWidget_2 = new QWidget(groupBox_3);
        layoutWidget_2->setObjectName("layoutWidget_2");
        layoutWidget_2->setGeometry(QRect(10, 30, 363, 64));
        verticalLayout_3 = new QVBoxLayout(layoutWidget_2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        radioButtonAutoRight = new QRadioButton(layoutWidget_2);
        radioButtonAutoRight->setObjectName("radioButtonAutoRight");
        radioButtonAutoRight->setChecked(true);

        horizontalLayout_6->addWidget(radioButtonAutoRight);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        radioButtonFixedRight = new QRadioButton(layoutWidget_2);
        radioButtonFixedRight->setObjectName("radioButtonFixedRight");

        horizontalLayout_6->addWidget(radioButtonFixedRight);


        verticalLayout_3->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_7 = new QLabel(layoutWidget_2);
        label_7->setObjectName("label_7");

        horizontalLayout_5->addWidget(label_7);

        doubleSpinBoxMinRight = new ScientificDoubleSpinBox(layoutWidget_2);
        doubleSpinBoxMinRight->setObjectName("doubleSpinBoxMinRight");
        doubleSpinBoxMinRight->setEnabled(false);
        doubleSpinBoxMinRight->setDecimals(6);
        doubleSpinBoxMinRight->setMinimum(-100000.000000000000000);
        doubleSpinBoxMinRight->setMaximum(100000.000000000000000);

        horizontalLayout_5->addWidget(doubleSpinBoxMinRight);

        label_8 = new QLabel(layoutWidget_2);
        label_8->setObjectName("label_8");

        horizontalLayout_5->addWidget(label_8);

        doubleSpinBoxMaxRight = new ScientificDoubleSpinBox(layoutWidget_2);
        doubleSpinBoxMaxRight->setObjectName("doubleSpinBoxMaxRight");
        doubleSpinBoxMaxRight->setEnabled(false);
        doubleSpinBoxMaxRight->setDecimals(6);
        doubleSpinBoxMaxRight->setMinimum(-1000000.000000000000000);
        doubleSpinBoxMaxRight->setMaximum(100000.000000000000000);

        horizontalLayout_5->addWidget(doubleSpinBoxMaxRight);


        verticalLayout_3->addLayout(horizontalLayout_5);

        label_18 = new QLabel(tabRight);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(10, 180, 81, 16));
        lineEditRightAxisLabel = new QLineEdit(tabRight);
        lineEditRightAxisLabel->setObjectName("lineEditRightAxisLabel");
        lineEditRightAxisLabel->setGeometry(QRect(100, 180, 221, 21));
        checkBoxRightActive = new QCheckBox(tabRight);
        checkBoxRightActive->setObjectName("checkBoxRightActive");
        checkBoxRightActive->setGeometry(QRect(10, 10, 81, 21));
        checkBoxRightActive->setAutoExclusive(true);
        checkBoxZoomYRight = new QCheckBox(tabRight);
        checkBoxZoomYRight->setObjectName("checkBoxZoomYRight");
        checkBoxZoomYRight->setEnabled(false);
        checkBoxZoomYRight->setGeometry(QRect(200, 10, 91, 21));
        tabWidget->addTab(tabRight, QString());
        tabLeft = new QWidget();
        tabLeft->setObjectName("tabLeft");
        groupBox_2 = new QGroupBox(tabLeft);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(6, 37, 400, 120));
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        groupBox_2->setMinimumSize(QSize(400, 120));
        groupBox_2->setMaximumSize(QSize(300, 120));
        groupBox_2->setCheckable(false);
        layoutWidget = new QWidget(groupBox_2);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 30, 355, 64));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        radioButtonAutoLeft = new QRadioButton(layoutWidget);
        radioButtonAutoLeft->setObjectName("radioButtonAutoLeft");
        radioButtonAutoLeft->setChecked(true);

        horizontalLayout_4->addWidget(radioButtonAutoLeft);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        radioButtonFixedLeft = new QRadioButton(layoutWidget);
        radioButtonFixedLeft->setObjectName("radioButtonFixedLeft");

        horizontalLayout_4->addWidget(radioButtonFixedLeft);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        horizontalLayout_3->addWidget(label_5);

        doubleSpinBoxMinLeft = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxMinLeft->setObjectName("doubleSpinBoxMinLeft");
        doubleSpinBoxMinLeft->setEnabled(false);
        doubleSpinBoxMinLeft->setDecimals(6);
        doubleSpinBoxMinLeft->setMinimum(-100000.000000000000000);
        doubleSpinBoxMinLeft->setMaximum(100000.000000000000000);

        horizontalLayout_3->addWidget(doubleSpinBoxMinLeft);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        horizontalLayout_3->addWidget(label_6);

        doubleSpinBoxMaxLeft = new ScientificDoubleSpinBox(layoutWidget);
        doubleSpinBoxMaxLeft->setObjectName("doubleSpinBoxMaxLeft");
        doubleSpinBoxMaxLeft->setEnabled(false);
        doubleSpinBoxMaxLeft->setDecimals(6);
        doubleSpinBoxMaxLeft->setMinimum(-100000.000000000000000);
        doubleSpinBoxMaxLeft->setMaximum(100000.000000000000000);

        horizontalLayout_3->addWidget(doubleSpinBoxMaxLeft);


        verticalLayout_2->addLayout(horizontalLayout_3);

        lineEditLeftAxisLabel = new QLineEdit(tabLeft);
        lineEditLeftAxisLabel->setObjectName("lineEditLeftAxisLabel");
        lineEditLeftAxisLabel->setGeometry(QRect(90, 170, 261, 28));
        label_9 = new QLabel(tabLeft);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 180, 71, 16));
        checkBoxZoomYLeft = new QCheckBox(tabLeft);
        checkBoxZoomYLeft->setObjectName("checkBoxZoomYLeft");
        checkBoxZoomYLeft->setEnabled(true);
        checkBoxZoomYLeft->setGeometry(QRect(190, 7, 85, 23));
        checkBoxLeftActive = new QCheckBox(tabLeft);
        checkBoxLeftActive->setObjectName("checkBoxLeftActive");
        checkBoxLeftActive->setGeometry(QRect(6, 7, 70, 23));
        checkBoxLeftActive->setChecked(false);
        checkBoxLeftActive->setAutoExclusive(true);
        tabWidget->addTab(tabLeft, QString());

        retranslateUi(PlotOptionsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, PlotOptionsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, PlotOptionsDialog, qOverload<>(&QDialog::reject));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PlotOptionsDialog);
    } // setupUi

    void retranslateUi(QDialog *PlotOptionsDialog)
    {
        PlotOptionsDialog->setWindowTitle(QCoreApplication::translate("PlotOptionsDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("PlotOptionsDialog", "Plot Title ", nullptr));
        groupBoxLego->setTitle(QCoreApplication::translate("PlotOptionsDialog", "Lego Plot", nullptr));
        radioButtonLegoHide->setText(QCoreApplication::translate("PlotOptionsDialog", "Hide", nullptr));
        radioButtonLegoBottom->setText(QCoreApplication::translate("PlotOptionsDialog", "Bottom", nullptr));
        radioButtonLegoTop->setText(QCoreApplication::translate("PlotOptionsDialog", "&Top", nullptr));
        label_2->setText(QCoreApplication::translate("PlotOptionsDialog", "Axis  label", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("PlotOptionsDialog", "Scale", nullptr));
        radioButtonAutoBottom->setText(QCoreApplication::translate("PlotOptionsDialog", "A&uto", nullptr));
        radioButtonFixedBottom->setText(QCoreApplication::translate("PlotOptionsDialog", "Fi&xed", nullptr));
        label_3->setText(QCoreApplication::translate("PlotOptionsDialog", "Min", nullptr));
        label_4->setText(QCoreApplication::translate("PlotOptionsDialog", "Max", nullptr));
        checkBoxZoomXBottom->setText(QCoreApplication::translate("PlotOptionsDialog", "Zoom On", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabBottom), QCoreApplication::translate("PlotOptionsDialog", "Bottom Axis ", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("PlotOptionsDialog", "Scale", nullptr));
        radioButtonAutoRight->setText(QCoreApplication::translate("PlotOptionsDialog", "A&uto", nullptr));
        radioButtonFixedRight->setText(QCoreApplication::translate("PlotOptionsDialog", "Fi&xed", nullptr));
        label_7->setText(QCoreApplication::translate("PlotOptionsDialog", "Min", nullptr));
        label_8->setText(QCoreApplication::translate("PlotOptionsDialog", "Max", nullptr));
        label_18->setText(QCoreApplication::translate("PlotOptionsDialog", "Axis  label", nullptr));
        checkBoxRightActive->setText(QCoreApplication::translate("PlotOptionsDialog", "Active", nullptr));
        checkBoxZoomYRight->setText(QCoreApplication::translate("PlotOptionsDialog", "Zoom On", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabRight), QCoreApplication::translate("PlotOptionsDialog", "Right Axis", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("PlotOptionsDialog", "Scale", nullptr));
        radioButtonAutoLeft->setText(QCoreApplication::translate("PlotOptionsDialog", "A&uto", nullptr));
        radioButtonFixedLeft->setText(QCoreApplication::translate("PlotOptionsDialog", "Fi&xed", nullptr));
        label_5->setText(QCoreApplication::translate("PlotOptionsDialog", "Min", nullptr));
        label_6->setText(QCoreApplication::translate("PlotOptionsDialog", "Max", nullptr));
        label_9->setText(QCoreApplication::translate("PlotOptionsDialog", "Axis  label", nullptr));
        checkBoxZoomYLeft->setText(QCoreApplication::translate("PlotOptionsDialog", "Zoom On", nullptr));
        checkBoxLeftActive->setText(QCoreApplication::translate("PlotOptionsDialog", " Active", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLeft), QCoreApplication::translate("PlotOptionsDialog", "Left Axis", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlotOptionsDialog: public Ui_PlotOptionsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOT_OPTIONS_DIALOG_H
