/********************************************************************************
** Form generated from reading UI file 'ptracking_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PTRACKING_DIALOG_H
#define UI_PTRACKING_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PTrackingDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditFilter;
    QLabel *label;
    QCheckBox *checkBoxMatchCase;
    QCheckBox *checkBoxPrintName;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxSteps;
    QLabel *label_3;
    QSpinBox *spinBoxStart;
    QLabel *label_4;
    QSpinBox *spinBoxStop;

    void setupUi(QDialog *PTrackingDialog)
    {
        if (PTrackingDialog->objectName().isEmpty())
            PTrackingDialog->setObjectName("PTrackingDialog");
        PTrackingDialog->resize(226, 248);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PTrackingDialog->sizePolicy().hasHeightForWidth());
        PTrackingDialog->setSizePolicy(sizePolicy);
        PTrackingDialog->setMinimumSize(QSize(226, 248));
        PTrackingDialog->setMaximumSize(QSize(226, 248));
        buttonBox = new QDialogButtonBox(PTrackingDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(30, 210, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEditFilter = new QLineEdit(PTrackingDialog);
        lineEditFilter->setObjectName("lineEditFilter");
        lineEditFilter->setGeometry(QRect(80, 30, 113, 21));
        label = new QLabel(PTrackingDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 30, 91, 16));
        checkBoxMatchCase = new QCheckBox(PTrackingDialog);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(80, 50, 101, 21));
        checkBoxPrintName = new QCheckBox(PTrackingDialog);
        checkBoxPrintName->setObjectName("checkBoxPrintName");
        checkBoxPrintName->setGeometry(QRect(20, 90, 151, 21));
        layoutWidget = new QWidget(PTrackingDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 120, 175, 76));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        doubleSpinBoxSteps = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxSteps->setObjectName("doubleSpinBoxSteps");

        formLayout->setWidget(0, QFormLayout::FieldRole, doubleSpinBoxSteps);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        spinBoxStart = new QSpinBox(layoutWidget);
        spinBoxStart->setObjectName("spinBoxStart");

        formLayout->setWidget(1, QFormLayout::FieldRole, spinBoxStart);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_4);

        spinBoxStop = new QSpinBox(layoutWidget);
        spinBoxStop->setObjectName("spinBoxStop");

        formLayout->setWidget(2, QFormLayout::FieldRole, spinBoxStop);


        retranslateUi(PTrackingDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, PTrackingDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, PTrackingDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(PTrackingDialog);
    } // setupUi

    void retranslateUi(QDialog *PTrackingDialog)
    {
        PTrackingDialog->setWindowTitle(QCoreApplication::translate("PTrackingDialog", "Trajectory ", nullptr));
        label->setText(QCoreApplication::translate("PTrackingDialog", "Filter (*,% ...)", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("PTrackingDialog", "Match Case", nullptr));
        checkBoxPrintName->setText(QCoreApplication::translate("PTrackingDialog", " Print element name", nullptr));
        label_2->setText(QCoreApplication::translate("PTrackingDialog", "Step [cm]", nullptr));
        label_3->setText(QCoreApplication::translate("PTrackingDialog", "Start from element", nullptr));
        label_4->setText(QCoreApplication::translate("PTrackingDialog", "Stop at element", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PTrackingDialog: public Ui_PTrackingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PTRACKING_DIALOG_H
