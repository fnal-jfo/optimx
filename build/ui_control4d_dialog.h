/********************************************************************************
** Form generated from reading UI file 'control4d_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROL4D_DIALOG_H
#define UI_CONTROL4D_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_Control4DDialog
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBoxUpdate;

    void setupUi(QDialog *Control4DDialog)
    {
        if (Control4DDialog->objectName().isEmpty())
            Control4DDialog->setObjectName("Control4DDialog");
        Control4DDialog->resize(296, 99);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Control4DDialog->sizePolicy().hasHeightForWidth());
        Control4DDialog->setSizePolicy(sizePolicy);
        Control4DDialog->setMinimumSize(QSize(296, 99));
        Control4DDialog->setMaximumSize(QSize(296, 99));
        QFont font;
        font.setPointSize(8);
        Control4DDialog->setFont(font);
        buttonBox = new QDialogButtonBox(Control4DDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(60, 40, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        checkBoxUpdate = new QCheckBox(Control4DDialog);
        checkBoxUpdate->setObjectName("checkBoxUpdate");
        checkBoxUpdate->setGeometry(QRect(10, 10, 311, 21));

        retranslateUi(Control4DDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, Control4DDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, Control4DDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(Control4DDialog);
    } // setupUi

    void retranslateUi(QDialog *Control4DDialog)
    {
        Control4DDialog->setWindowTitle(QCoreApplication::translate("Control4DDialog", "4D Control Dialog", nullptr));
        checkBoxUpdate->setText(QCoreApplication::translate("Control4DDialog", "Update 4D parameters before calculations.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Control4DDialog: public Ui_Control4DDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROL4D_DIALOG_H
