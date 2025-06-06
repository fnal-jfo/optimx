/********************************************************************************
** Form generated from reading UI file 'lattice_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LATTICE_DIALOG_H
#define UI_LATTICE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_LatticeDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QCheckBox *checkBoxMatchCase;
    QLineEdit *lineEditFilter;
    QLabel *label_2;
    QSpinBox *spinBoxMaxPoleOrder;
    QCheckBox *checkBoxPrintBdl;
    QCheckBox *checkBoxOmitFirstChar;
    QCheckBox *checkBoxUseSFromFile;

    void setupUi(QDialog *LatticeDialog)
    {
        if (LatticeDialog->objectName().isEmpty())
            LatticeDialog->setObjectName("LatticeDialog");
        LatticeDialog->resize(323, 268);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LatticeDialog->sizePolicy().hasHeightForWidth());
        LatticeDialog->setSizePolicy(sizePolicy);
        LatticeDialog->setMinimumSize(QSize(323, 268));
        LatticeDialog->setMaximumSize(QSize(323, 268));
        QFont font;
        font.setPointSize(8);
        LatticeDialog->setFont(font);
        buttonBox = new QDialogButtonBox(LatticeDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(80, 220, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(LatticeDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 20, 91, 16));
        checkBoxMatchCase = new QCheckBox(LatticeDialog);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(120, 50, 101, 21));
        lineEditFilter = new QLineEdit(LatticeDialog);
        lineEditFilter->setObjectName("lineEditFilter");
        lineEditFilter->setGeometry(QRect(120, 20, 113, 22));
        label_2 = new QLabel(LatticeDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 80, 161, 16));
        spinBoxMaxPoleOrder = new QSpinBox(LatticeDialog);
        spinBoxMaxPoleOrder->setObjectName("spinBoxMaxPoleOrder");
        spinBoxMaxPoleOrder->setGeometry(QRect(190, 80, 56, 22));
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(spinBoxMaxPoleOrder->sizePolicy().hasHeightForWidth());
        spinBoxMaxPoleOrder->setSizePolicy(sizePolicy1);
        checkBoxPrintBdl = new QCheckBox(LatticeDialog);
        checkBoxPrintBdl->setObjectName("checkBoxPrintBdl");
        checkBoxPrintBdl->setGeometry(QRect(20, 120, 221, 21));
        checkBoxOmitFirstChar = new QCheckBox(LatticeDialog);
        checkBoxOmitFirstChar->setObjectName("checkBoxOmitFirstChar");
        checkBoxOmitFirstChar->setGeometry(QRect(20, 150, 291, 21));
        checkBoxUseSFromFile = new QCheckBox(LatticeDialog);
        checkBoxUseSFromFile->setObjectName("checkBoxUseSFromFile");
        checkBoxUseSFromFile->setGeometry(QRect(20, 180, 201, 21));

        retranslateUi(LatticeDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, LatticeDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, LatticeDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(LatticeDialog);
    } // setupUi

    void retranslateUi(QDialog *LatticeDialog)
    {
        LatticeDialog->setWindowTitle(QCoreApplication::translate("LatticeDialog", "Lattice Dialog", nullptr));
        label->setText(QCoreApplication::translate("LatticeDialog", "Filter [*, %, ...] ", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("LatticeDialog", "match case", nullptr));
        label_2->setText(QCoreApplication::translate("LatticeDialog", "Keep multipoles up to order ", nullptr));
        checkBoxPrintBdl->setText(QCoreApplication::translate("LatticeDialog", "Print Integral B dl ", nullptr));
        checkBoxOmitFirstChar->setText(QCoreApplication::translate("LatticeDialog", "Do not print first character of element names", nullptr));
        checkBoxUseSFromFile->setText(QCoreApplication::translate("LatticeDialog", "begin s-coordinates from file", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LatticeDialog: public Ui_LatticeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LATTICE_DIALOG_H
