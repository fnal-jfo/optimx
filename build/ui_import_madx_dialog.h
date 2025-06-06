/********************************************************************************
** Form generated from reading UI file 'import_madx_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMPORT_MADX_DIALOG_H
#define UI_IMPORT_MADX_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImportMadXDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxEtot;
    QDoubleSpinBox *doubleSpinBoxM0c2;
    QLineEdit *lineEditFileName;
    QLabel *label_4;
    QPushButton *pushButton_2;

    void setupUi(QDialog *ImportMadXDialog)
    {
        if (ImportMadXDialog->objectName().isEmpty())
            ImportMadXDialog->setObjectName("ImportMadXDialog");
        ImportMadXDialog->resize(486, 175);
        buttonBox = new QDialogButtonBox(ImportMadXDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(151, 139, 157, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(ImportMadXDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(101, 65, 300, 64));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        doubleSpinBoxEtot = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxEtot->setObjectName("doubleSpinBoxEtot");
        doubleSpinBoxEtot->setReadOnly(true);
        doubleSpinBoxEtot->setButtonSymbols(QAbstractSpinBox::NoButtons);
        doubleSpinBoxEtot->setDecimals(6);
        doubleSpinBoxEtot->setMaximum(1000000000000000.000000000000000);

        gridLayout->addWidget(doubleSpinBoxEtot, 0, 1, 1, 1);

        doubleSpinBoxM0c2 = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxM0c2->setObjectName("doubleSpinBoxM0c2");
        doubleSpinBoxM0c2->setReadOnly(true);
        doubleSpinBoxM0c2->setButtonSymbols(QAbstractSpinBox::NoButtons);
        doubleSpinBoxM0c2->setDecimals(6);
        doubleSpinBoxM0c2->setMaximum(10000000000000.000000000000000);

        gridLayout->addWidget(doubleSpinBoxM0c2, 1, 1, 1, 1);

        lineEditFileName = new QLineEdit(ImportMadXDialog);
        lineEditFileName->setObjectName("lineEditFileName");
        lineEditFileName->setGeometry(QRect(37, 26, 301, 21));
        label_4 = new QLabel(ImportMadXDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(131, 5, 81, 16));
        pushButton_2 = new QPushButton(ImportMadXDialog);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(363, 25, 89, 22));

        retranslateUi(ImportMadXDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ImportMadXDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ImportMadXDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButton_2, SIGNAL(clicked()), ImportMadXDialog, SLOT(setOutputFilename()));

        QMetaObject::connectSlotsByName(ImportMadXDialog);
    } // setupUi

    void retranslateUi(QDialog *ImportMadXDialog)
    {
        ImportMadXDialog->setWindowTitle(QCoreApplication::translate("ImportMadXDialog", "Import from MADX", nullptr));
        label_2->setText(QCoreApplication::translate("ImportMadXDialog", "Particle Mass [GeV/c**2]", nullptr));
        label->setText(QCoreApplication::translate("ImportMadXDialog", "Total Energy [GeV]", nullptr));
        label_4->setText(QCoreApplication::translate("ImportMadXDialog", "To: OptiM File", nullptr));
        pushButton_2->setText(QCoreApplication::translate("ImportMadXDialog", "Select File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImportMadXDialog: public Ui_ImportMadXDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMPORT_MADX_DIALOG_H
