/********************************************************************************
** Form generated from reading UI file 'integration_step_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTEGRATION_STEP_DIALOG_H
#define UI_INTEGRATION_STEP_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_IntegrationStepDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxStepSize;

    void setupUi(QDialog *IntegrationStepDialog)
    {
        if (IntegrationStepDialog->objectName().isEmpty())
            IntegrationStepDialog->setObjectName("IntegrationStepDialog");
        IntegrationStepDialog->resize(285, 100);
        IntegrationStepDialog->setMinimumSize(QSize(285, 100));
        IntegrationStepDialog->setMaximumSize(QSize(285, 100));
        buttonBox = new QDialogButtonBox(IntegrationStepDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(70, 60, 156, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(IntegrationStepDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(70, 20, 157, 24));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        doubleSpinBoxStepSize = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxStepSize->setObjectName("doubleSpinBoxStepSize");

        gridLayout->addWidget(doubleSpinBoxStepSize, 0, 1, 1, 1);


        retranslateUi(IntegrationStepDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, IntegrationStepDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, IntegrationStepDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(IntegrationStepDialog);
    } // setupUi

    void retranslateUi(QDialog *IntegrationStepDialog)
    {
        IntegrationStepDialog->setWindowTitle(QCoreApplication::translate("IntegrationStepDialog", "Integration  Step Size ", nullptr));
        label->setText(QCoreApplication::translate("IntegrationStepDialog", "Integration Step", nullptr));
    } // retranslateUi

};

namespace Ui {
    class IntegrationStepDialog: public Ui_IntegrationStepDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTEGRATION_STEP_DIALOG_H
