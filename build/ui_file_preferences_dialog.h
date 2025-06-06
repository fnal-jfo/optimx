/********************************************************************************
** Form generated from reading UI file 'file_preferences_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILE_PREFERENCES_DIALOG_H
#define UI_FILE_PREFERENCES_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_GeneralPreferencesDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditWorkDir;
    QLabel *label;
    QPushButton *pushButtonWorkDir;

    void setupUi(QDialog *GeneralPreferencesDialog)
    {
        if (GeneralPreferencesDialog->objectName().isEmpty())
            GeneralPreferencesDialog->setObjectName("GeneralPreferencesDialog");
        GeneralPreferencesDialog->resize(408, 133);
        buttonBox = new QDialogButtonBox(GeneralPreferencesDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(130, 90, 131, 21));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEditWorkDir = new QLineEdit(GeneralPreferencesDialog);
        lineEditWorkDir->setObjectName("lineEditWorkDir");
        lineEditWorkDir->setGeometry(QRect(10, 50, 391, 21));
        label = new QLabel(GeneralPreferencesDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 111, 16));
        pushButtonWorkDir = new QPushButton(GeneralPreferencesDialog);
        pushButtonWorkDir->setObjectName("pushButtonWorkDir");
        pushButtonWorkDir->setGeometry(QRect(120, 20, 51, 22));

        retranslateUi(GeneralPreferencesDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, GeneralPreferencesDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, GeneralPreferencesDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(GeneralPreferencesDialog);
    } // setupUi

    void retranslateUi(QDialog *GeneralPreferencesDialog)
    {
        GeneralPreferencesDialog->setWindowTitle(QCoreApplication::translate("GeneralPreferencesDialog", "General Preferences", nullptr));
        label->setText(QCoreApplication::translate("GeneralPreferencesDialog", "Working Directory", nullptr));
        pushButtonWorkDir->setText(QCoreApplication::translate("GeneralPreferencesDialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GeneralPreferencesDialog: public Ui_GeneralPreferencesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILE_PREFERENCES_DIALOG_H
