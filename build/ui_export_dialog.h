/********************************************************************************
** Form generated from reading UI file 'export_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXPORT_DIALOG_H
#define UI_EXPORT_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ExportDialog
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBoxOmitFirstChar;
    QCheckBox *checkBoxUseLcav;
    QLineEdit *lineEdit;
    QLabel *label;
    QPushButton *pushButton;
    QLineEdit *lineEditDir;
    QLabel *label_2;

    void setupUi(QDialog *ExportDialog)
    {
        if (ExportDialog->objectName().isEmpty())
            ExportDialog->setObjectName("ExportDialog");
        ExportDialog->resize(388, 287);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ExportDialog->sizePolicy().hasHeightForWidth());
        ExportDialog->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(8);
        ExportDialog->setFont(font);
        buttonBox = new QDialogButtonBox(ExportDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(110, 200, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        checkBoxOmitFirstChar = new QCheckBox(ExportDialog);
        checkBoxOmitFirstChar->setObjectName("checkBoxOmitFirstChar");
        checkBoxOmitFirstChar->setGeometry(QRect(20, 140, 291, 21));
        checkBoxUseLcav = new QCheckBox(ExportDialog);
        checkBoxUseLcav->setObjectName("checkBoxUseLcav");
        checkBoxUseLcav->setGeometry(QRect(20, 170, 331, 21));
        lineEdit = new QLineEdit(ExportDialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(20, 100, 271, 21));
        label = new QLabel(ExportDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 70, 111, 16));
        pushButton = new QPushButton(ExportDialog);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(310, 100, 31, 22));
        lineEditDir = new QLineEdit(ExportDialog);
        lineEditDir->setObjectName("lineEditDir");
        lineEditDir->setGeometry(QRect(20, 40, 271, 21));
        label_2 = new QLabel(ExportDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 10, 111, 16));

        retranslateUi(ExportDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ExportDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ExportDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButton, SIGNAL(clicked()), ExportDialog, SLOT(setFileName()));

        QMetaObject::connectSlotsByName(ExportDialog);
    } // setupUi

    void retranslateUi(QDialog *ExportDialog)
    {
        ExportDialog->setWindowTitle(QCoreApplication::translate("ExportDialog", "Export Dialog", nullptr));
        checkBoxOmitFirstChar->setText(QCoreApplication::translate("ExportDialog", "Omit first character from element names", nullptr));
        checkBoxUseLcav->setText(QCoreApplication::translate("ExportDialog", "Convert RF cavities to linac style cavities [LCAV]", nullptr));
        label->setText(QCoreApplication::translate("ExportDialog", "Exported File Name", nullptr));
        pushButton->setText(QCoreApplication::translate("ExportDialog", "...", nullptr));
        label_2->setText(QCoreApplication::translate("ExportDialog", "Directory", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ExportDialog: public Ui_ExportDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXPORT_DIALOG_H
