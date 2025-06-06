/********************************************************************************
** Form generated from reading UI file 'extra_scatter_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXTRA_SCATTER_DIALOG_H
#define UI_EXTRA_SCATTER_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExtraScatterDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QWidget *widget;
    QFormLayout *formLayout;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxdX;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxdY;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxdS;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBoxdPX;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxdPY;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxdPP;
    QGroupBox *groupBox_2;
    QWidget *widget1;
    QFormLayout *formLayout_2;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBoxSigmaX;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBoxSigmaY;
    QLabel *label_9;
    QDoubleSpinBox *doubleSpinBoxSigmaS;
    QLabel *label_10;
    QDoubleSpinBox *doubleSpinBoxSigmaPX;
    QLabel *label_11;
    QDoubleSpinBox *doubleSpinBoxSigmaPY;
    QLabel *label_12;
    QDoubleSpinBox *doubleSpinBoxSigmadPP;

    void setupUi(QDialog *ExtraScatterDialog)
    {
        if (ExtraScatterDialog->objectName().isEmpty())
            ExtraScatterDialog->setObjectName("ExtraScatterDialog");
        ExtraScatterDialog->resize(500, 300);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ExtraScatterDialog->sizePolicy().hasHeightForWidth());
        ExtraScatterDialog->setSizePolicy(sizePolicy);
        ExtraScatterDialog->setMinimumSize(QSize(500, 300));
        ExtraScatterDialog->setMaximumSize(QSize(500, 300));
        buttonBox = new QDialogButtonBox(ExtraScatterDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(170, 260, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(ExtraScatterDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 20, 221, 231));
        widget = new QWidget(groupBox);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(30, 50, 161, 154));
        formLayout = new QFormLayout(widget);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        doubleSpinBoxdX = new QDoubleSpinBox(widget);
        doubleSpinBoxdX->setObjectName("doubleSpinBoxdX");

        formLayout->setWidget(0, QFormLayout::FieldRole, doubleSpinBoxdX);

        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        doubleSpinBoxdY = new QDoubleSpinBox(widget);
        doubleSpinBoxdY->setObjectName("doubleSpinBoxdY");

        formLayout->setWidget(1, QFormLayout::FieldRole, doubleSpinBoxdY);

        label_3 = new QLabel(widget);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        doubleSpinBoxdS = new QDoubleSpinBox(widget);
        doubleSpinBoxdS->setObjectName("doubleSpinBoxdS");

        formLayout->setWidget(2, QFormLayout::FieldRole, doubleSpinBoxdS);

        label_4 = new QLabel(widget);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        doubleSpinBoxdPX = new QDoubleSpinBox(widget);
        doubleSpinBoxdPX->setObjectName("doubleSpinBoxdPX");

        formLayout->setWidget(3, QFormLayout::FieldRole, doubleSpinBoxdPX);

        label_5 = new QLabel(widget);
        label_5->setObjectName("label_5");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_5);

        doubleSpinBoxdPY = new QDoubleSpinBox(widget);
        doubleSpinBoxdPY->setObjectName("doubleSpinBoxdPY");

        formLayout->setWidget(4, QFormLayout::FieldRole, doubleSpinBoxdPY);

        label_6 = new QLabel(widget);
        label_6->setObjectName("label_6");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_6);

        doubleSpinBoxdPP = new QDoubleSpinBox(widget);
        doubleSpinBoxdPP->setObjectName("doubleSpinBoxdPP");

        formLayout->setWidget(5, QFormLayout::FieldRole, doubleSpinBoxdPP);

        groupBox_2 = new QGroupBox(ExtraScatterDialog);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(250, 20, 221, 231));
        widget1 = new QWidget(groupBox_2);
        widget1->setObjectName("widget1");
        widget1->setGeometry(QRect(20, 50, 169, 154));
        formLayout_2 = new QFormLayout(widget1);
        formLayout_2->setObjectName("formLayout_2");
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(widget1);
        label_7->setObjectName("label_7");

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_7);

        doubleSpinBoxSigmaX = new QDoubleSpinBox(widget1);
        doubleSpinBoxSigmaX->setObjectName("doubleSpinBoxSigmaX");

        formLayout_2->setWidget(0, QFormLayout::FieldRole, doubleSpinBoxSigmaX);

        label_8 = new QLabel(widget1);
        label_8->setObjectName("label_8");

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_8);

        doubleSpinBoxSigmaY = new QDoubleSpinBox(widget1);
        doubleSpinBoxSigmaY->setObjectName("doubleSpinBoxSigmaY");

        formLayout_2->setWidget(1, QFormLayout::FieldRole, doubleSpinBoxSigmaY);

        label_9 = new QLabel(widget1);
        label_9->setObjectName("label_9");

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_9);

        doubleSpinBoxSigmaS = new QDoubleSpinBox(widget1);
        doubleSpinBoxSigmaS->setObjectName("doubleSpinBoxSigmaS");

        formLayout_2->setWidget(2, QFormLayout::FieldRole, doubleSpinBoxSigmaS);

        label_10 = new QLabel(widget1);
        label_10->setObjectName("label_10");

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_10);

        doubleSpinBoxSigmaPX = new QDoubleSpinBox(widget1);
        doubleSpinBoxSigmaPX->setObjectName("doubleSpinBoxSigmaPX");

        formLayout_2->setWidget(3, QFormLayout::FieldRole, doubleSpinBoxSigmaPX);

        label_11 = new QLabel(widget1);
        label_11->setObjectName("label_11");

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_11);

        doubleSpinBoxSigmaPY = new QDoubleSpinBox(widget1);
        doubleSpinBoxSigmaPY->setObjectName("doubleSpinBoxSigmaPY");

        formLayout_2->setWidget(4, QFormLayout::FieldRole, doubleSpinBoxSigmaPY);

        label_12 = new QLabel(widget1);
        label_12->setObjectName("label_12");

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_12);

        doubleSpinBoxSigmadPP = new QDoubleSpinBox(widget1);
        doubleSpinBoxSigmadPP->setObjectName("doubleSpinBoxSigmadPP");

        formLayout_2->setWidget(5, QFormLayout::FieldRole, doubleSpinBoxSigmadPP);


        retranslateUi(ExtraScatterDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ExtraScatterDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ExtraScatterDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ExtraScatterDialog);
    } // setupUi

    void retranslateUi(QDialog *ExtraScatterDialog)
    {
        ExtraScatterDialog->setWindowTitle(QCoreApplication::translate("ExtraScatterDialog", "Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ExtraScatterDialog", "Distribution Center Offsets", nullptr));
        label->setText(QCoreApplication::translate("ExtraScatterDialog", "Delta X [cm]", nullptr));
        label_2->setText(QCoreApplication::translate("ExtraScatterDialog", "Delta Y [cm]", nullptr));
        label_3->setText(QCoreApplication::translate("ExtraScatterDialog", "Delta S [cm]", nullptr));
        label_4->setText(QCoreApplication::translate("ExtraScatterDialog", "Delta PX [mrad]", nullptr));
        label_5->setText(QCoreApplication::translate("ExtraScatterDialog", "Delta PY [mrad]", nullptr));
        label_6->setText(QCoreApplication::translate("ExtraScatterDialog", "Delta P/ P [mrad]", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("ExtraScatterDialog", "Additional RMS Scattering", nullptr));
        label_7->setText(QCoreApplication::translate("ExtraScatterDialog", "Sigma X [cm]", nullptr));
        label_8->setText(QCoreApplication::translate("ExtraScatterDialog", "Sigma Y [cm]", nullptr));
        label_9->setText(QCoreApplication::translate("ExtraScatterDialog", "Sigma S [cm]", nullptr));
        label_10->setText(QCoreApplication::translate("ExtraScatterDialog", "Sigma PX [mrad]", nullptr));
        label_11->setText(QCoreApplication::translate("ExtraScatterDialog", "Sigma PY [mrad]", nullptr));
        label_12->setText(QCoreApplication::translate("ExtraScatterDialog", "Sigma dP/P [mrad]", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ExtraScatterDialog: public Ui_ExtraScatterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTRA_SCATTER_DIALOG_H
