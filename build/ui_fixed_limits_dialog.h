/********************************************************************************
** Form generated from reading UI file 'fixed_limits_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FIXED_LIMITS_DIALOG_H
#define UI_FIXED_LIMITS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include "ScientificDoubleSpinBox.h"

QT_BEGIN_NAMESPACE

class Ui_FixedLimitsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label;
    ScientificDoubleSpinBox *techSpinBoxMax;
    ScientificDoubleSpinBox *techSpinBoxMin;
    QCheckBox *checkBoxActive;
    QCheckBox *checkBoxZoom;
    QCheckBox *checkBoxAutoScale;

    void setupUi(QDialog *FixedLimitsDialog)
    {
        if (FixedLimitsDialog->objectName().isEmpty())
            FixedLimitsDialog->setObjectName("FixedLimitsDialog");
        FixedLimitsDialog->resize(250, 150);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FixedLimitsDialog->sizePolicy().hasHeightForWidth());
        FixedLimitsDialog->setSizePolicy(sizePolicy);
        FixedLimitsDialog->setMinimumSize(QSize(250, 150));
        FixedLimitsDialog->setMaximumSize(QSize(250, 150));
        buttonBox = new QDialogButtonBox(FixedLimitsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(40, 110, 171, 31));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(FixedLimitsDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 10, 231, 61));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 1, 0, 1, 1);

        techSpinBoxMax = new ScientificDoubleSpinBox(layoutWidget);
        techSpinBoxMax->setObjectName("techSpinBoxMax");

        gridLayout->addWidget(techSpinBoxMax, 0, 1, 1, 1);

        techSpinBoxMin = new ScientificDoubleSpinBox(layoutWidget);
        techSpinBoxMin->setObjectName("techSpinBoxMin");

        gridLayout->addWidget(techSpinBoxMin, 1, 1, 1, 1);

        checkBoxActive = new QCheckBox(FixedLimitsDialog);
        checkBoxActive->setObjectName("checkBoxActive");
        checkBoxActive->setGeometry(QRect(10, 80, 82, 21));
        checkBoxZoom = new QCheckBox(FixedLimitsDialog);
        checkBoxZoom->setObjectName("checkBoxZoom");
        checkBoxZoom->setGeometry(QRect(80, 80, 82, 21));
        checkBoxAutoScale = new QCheckBox(FixedLimitsDialog);
        checkBoxAutoScale->setObjectName("checkBoxAutoScale");
        checkBoxAutoScale->setGeometry(QRect(150, 80, 82, 21));

        retranslateUi(FixedLimitsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, FixedLimitsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, FixedLimitsDialog, qOverload<>(&QDialog::reject));
        QObject::connect(checkBoxAutoScale, SIGNAL(clicked(bool)), techSpinBoxMin, SLOT(setDisabled(bool)));
        QObject::connect(checkBoxAutoScale, SIGNAL(clicked(bool)), techSpinBoxMax, SLOT(setDisabled(bool)));

        QMetaObject::connectSlotsByName(FixedLimitsDialog);
    } // setupUi

    void retranslateUi(QDialog *FixedLimitsDialog)
    {
        FixedLimitsDialog->setWindowTitle(QCoreApplication::translate("FixedLimitsDialog", "Dialog", nullptr));
        label_2->setText(QCoreApplication::translate("FixedLimitsDialog", "Maximum", nullptr));
        label->setText(QCoreApplication::translate("FixedLimitsDialog", "Minimum", nullptr));
        checkBoxActive->setText(QCoreApplication::translate("FixedLimitsDialog", "Active", nullptr));
        checkBoxZoom->setText(QCoreApplication::translate("FixedLimitsDialog", "Zoom", nullptr));
        checkBoxAutoScale->setText(QCoreApplication::translate("FixedLimitsDialog", "Autoscale", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FixedLimitsDialog: public Ui_FixedLimitsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FIXED_LIMITS_DIALOG_H
