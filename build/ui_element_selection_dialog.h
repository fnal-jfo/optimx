/********************************************************************************
** Form generated from reading UI file 'element_selection_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ELEMENT_SELECTION_DIALOG_H
#define UI_ELEMENT_SELECTION_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ElementSelectionDialog
{
public:
    QDialogButtonBox *buttonBox;
    QSpinBox *spinBoxElmNo;
    QLabel *label;

    void setupUi(QDialog *ElementSelectionDialog)
    {
        if (ElementSelectionDialog->objectName().isEmpty())
            ElementSelectionDialog->setObjectName("ElementSelectionDialog");
        ElementSelectionDialog->resize(278, 136);
        ElementSelectionDialog->setMinimumSize(QSize(278, 136));
        ElementSelectionDialog->setMaximumSize(QSize(278, 136));
        buttonBox = new QDialogButtonBox(ElementSelectionDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(60, 70, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        spinBoxElmNo = new QSpinBox(ElementSelectionDialog);
        spinBoxElmNo->setObjectName("spinBoxElmNo");
        spinBoxElmNo->setGeometry(QRect(210, 30, 52, 22));
        label = new QLabel(ElementSelectionDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 30, 171, 20));

        retranslateUi(ElementSelectionDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ElementSelectionDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ElementSelectionDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ElementSelectionDialog);
    } // setupUi

    void retranslateUi(QDialog *ElementSelectionDialog)
    {
        ElementSelectionDialog->setWindowTitle(QCoreApplication::translate("ElementSelectionDialog", "Element Selection", nullptr));
        label->setText(QCoreApplication::translate("ElementSelectionDialog", "Lattice Functions at Element", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ElementSelectionDialog: public Ui_ElementSelectionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ELEMENT_SELECTION_DIALOG_H
