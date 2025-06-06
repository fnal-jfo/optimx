/********************************************************************************
** Form generated from reading UI file 'edit_read_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDIT_READ_DIALOG_H
#define UI_EDIT_READ_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_EditReadDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditFilter;
    QLabel *label;
    QCheckBox *checkBoxMatchCase;
    QLineEdit *lineEditNamePrefix;
    QLabel *label_2;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxScale;
    QCheckBox *checkBoxCompareOnly;
    QLabel *label_4;
    QLineEdit *lineEditFileName;
    QPushButton *pushButton;

    void setupUi(QDialog *EditReadDialog)
    {
        if (EditReadDialog->objectName().isEmpty())
            EditReadDialog->setObjectName("EditReadDialog");
        EditReadDialog->resize(388, 282);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(EditReadDialog->sizePolicy().hasHeightForWidth());
        EditReadDialog->setSizePolicy(sizePolicy);
        EditReadDialog->setMinimumSize(QSize(290, 245));
        EditReadDialog->setMaximumSize(QSize(100000, 100000));
        buttonBox = new QDialogButtonBox(EditReadDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(110, 220, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEditFilter = new QLineEdit(EditReadDialog);
        lineEditFilter->setObjectName("lineEditFilter");
        lineEditFilter->setGeometry(QRect(60, 70, 113, 21));
        label = new QLabel(EditReadDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 80, 55, 13));
        checkBoxMatchCase = new QCheckBox(EditReadDialog);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(190, 70, 101, 21));
        lineEditNamePrefix = new QLineEdit(EditReadDialog);
        lineEditNamePrefix->setObjectName("lineEditNamePrefix");
        lineEditNamePrefix->setGeometry(QRect(190, 110, 41, 21));
        label_2 = new QLabel(EditReadDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 110, 171, 16));
        label_3 = new QLabel(EditReadDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 180, 161, 16));
        doubleSpinBoxScale = new QDoubleSpinBox(EditReadDialog);
        doubleSpinBoxScale->setObjectName("doubleSpinBoxScale");
        doubleSpinBoxScale->setGeometry(QRect(190, 180, 62, 22));
        checkBoxCompareOnly = new QCheckBox(EditReadDialog);
        checkBoxCompareOnly->setObjectName("checkBoxCompareOnly");
        checkBoxCompareOnly->setGeometry(QRect(20, 140, 121, 21));
        label_4 = new QLabel(EditReadDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 40, 55, 13));
        lineEditFileName = new QLineEdit(EditReadDialog);
        lineEditFileName->setObjectName("lineEditFileName");
        lineEditFileName->setGeometry(QRect(70, 30, 221, 21));
        pushButton = new QPushButton(EditReadDialog);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(300, 30, 41, 22));

        retranslateUi(EditReadDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, EditReadDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, EditReadDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButton, SIGNAL(clicked()), EditReadDialog, SLOT(selectFile()));

        QMetaObject::connectSlotsByName(EditReadDialog);
    } // setupUi

    void retranslateUi(QDialog *EditReadDialog)
    {
        EditReadDialog->setWindowTitle(QCoreApplication::translate("EditReadDialog", "Element Filter", nullptr));
        label->setText(QCoreApplication::translate("EditReadDialog", "Filter", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("EditReadDialog", "Match Case", nullptr));
        label_2->setText(QCoreApplication::translate("EditReadDialog", "Name Prefix [b,q,s, ...]", nullptr));
        label_3->setText(QCoreApplication::translate("EditReadDialog", "Scale [result = data*scale]", nullptr));
        checkBoxCompareOnly->setText(QCoreApplication::translate("EditReadDialog", "Compare Only", nullptr));
        label_4->setText(QCoreApplication::translate("EditReadDialog", "File Name", nullptr));
        pushButton->setText(QCoreApplication::translate("EditReadDialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EditReadDialog: public Ui_EditReadDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDIT_READ_DIALOG_H
