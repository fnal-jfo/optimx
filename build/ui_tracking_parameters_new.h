/********************************************************************************
** Form generated from reading UI file 'tracking_parameters_new.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKING_PARAMETERS_NEW_H
#define UI_TRACKING_PARAMETERS_NEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackingParametersNewDialog
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBoxPrintResults;
    QGroupBox *groupBox;
    QLabel *label;
    QSpinBox *spinBoxNTurn;
    QLabel *label_2;
    QSpinBox *spinBoxNElm;
    QLabel *label_5;
    QLineEdit *lineEditFilter;
    QCheckBox *checkBoxMatchCase;
    QCheckBox *checkBoxIncrementTurn;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QRadioButton *radioButtonAll;
    QRadioButton *radioButtonSpecific;
    QRadioButton *radioButtonSelected;
    QButtonGroup *buttonGroup;

    void setupUi(QDialog *TrackingParametersNewDialog)
    {
        if (TrackingParametersNewDialog->objectName().isEmpty())
            TrackingParametersNewDialog->setObjectName("TrackingParametersNewDialog");
        TrackingParametersNewDialog->resize(541, 534);
        QSizePolicy sizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TrackingParametersNewDialog->sizePolicy().hasHeightForWidth());
        TrackingParametersNewDialog->setSizePolicy(sizePolicy);
        TrackingParametersNewDialog->setMinimumSize(QSize(446, 344));
        TrackingParametersNewDialog->setMaximumSize(QSize(10000, 10000));
        buttonBox = new QDialogButtonBox(TrackingParametersNewDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(140, 490, 171, 31));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        checkBoxPrintResults = new QCheckBox(TrackingParametersNewDialog);
        checkBoxPrintResults->setObjectName("checkBoxPrintResults");
        checkBoxPrintResults->setGeometry(QRect(40, 420, 271, 21));
        groupBox = new QGroupBox(TrackingParametersNewDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 10, 451, 391));
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 50, 181, 16));
        spinBoxNTurn = new QSpinBox(groupBox);
        spinBoxNTurn->setObjectName("spinBoxNTurn");
        spinBoxNTurn->setGeometry(QRect(220, 50, 71, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 210, 131, 16));
        spinBoxNElm = new QSpinBox(groupBox);
        spinBoxNElm->setObjectName("spinBoxNElm");
        spinBoxNElm->setGeometry(QRect(230, 210, 101, 22));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(30, 240, 161, 20));
        lineEditFilter = new QLineEdit(groupBox);
        lineEditFilter->setObjectName("lineEditFilter");
        lineEditFilter->setGeometry(QRect(230, 240, 171, 21));
        checkBoxMatchCase = new QCheckBox(groupBox);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(170, 270, 111, 21));
        checkBoxIncrementTurn = new QCheckBox(groupBox);
        checkBoxIncrementTurn->setObjectName("checkBoxIncrementTurn");
        checkBoxIncrementTurn->setGeometry(QRect(30, 340, 391, 21));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(30, 90, 341, 77));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        radioButtonAll = new QRadioButton(layoutWidget);
        buttonGroup = new QButtonGroup(TrackingParametersNewDialog);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(radioButtonAll);
        radioButtonAll->setObjectName("radioButtonAll");

        gridLayout->addWidget(radioButtonAll, 0, 0, 1, 1);

        radioButtonSpecific = new QRadioButton(layoutWidget);
        buttonGroup->addButton(radioButtonSpecific);
        radioButtonSpecific->setObjectName("radioButtonSpecific");

        gridLayout->addWidget(radioButtonSpecific, 1, 0, 1, 1);

        radioButtonSelected = new QRadioButton(layoutWidget);
        buttonGroup->addButton(radioButtonSelected);
        radioButtonSelected->setObjectName("radioButtonSelected");

        gridLayout->addWidget(radioButtonSelected, 2, 0, 1, 1);


        retranslateUi(TrackingParametersNewDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TrackingParametersNewDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TrackingParametersNewDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TrackingParametersNewDialog);
    } // setupUi

    void retranslateUi(QDialog *TrackingParametersNewDialog)
    {
        TrackingParametersNewDialog->setWindowTitle(QCoreApplication::translate("TrackingParametersNewDialog", "Tracking Parameters", nullptr));
        checkBoxPrintResults->setText(QCoreApplication::translate("TrackingParametersNewDialog", "  Display tracking results in a text window", nullptr));
        groupBox->setTitle(QCoreApplication::translate("TrackingParametersNewDialog", "Multi-turn Tracking", nullptr));
        label->setText(QCoreApplication::translate("TrackingParametersNewDialog", "Number of turns to track", nullptr));
        label_2->setText(QCoreApplication::translate("TrackingParametersNewDialog", "Element index", nullptr));
        label_5->setText(QCoreApplication::translate("TrackingParametersNewDialog", "Element selection filter", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("TrackingParametersNewDialog", "Match Case", nullptr));
        checkBoxIncrementTurn->setText(QCoreApplication::translate("TrackingParametersNewDialog", "Increment  variable $_Nturn and re-parse at each turn", nullptr));
        radioButtonAll->setText(QCoreApplication::translate("TrackingParametersNewDialog", "All elements (averaged over all turns)", nullptr));
        radioButtonSpecific->setText(QCoreApplication::translate("TrackingParametersNewDialog", "Turn-by-turn output at a specific element ", nullptr));
        radioButtonSelected->setText(QCoreApplication::translate("TrackingParametersNewDialog", "Turn-by-turn output at selected elements", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackingParametersNewDialog: public Ui_TrackingParametersNewDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACKING_PARAMETERS_NEW_H
