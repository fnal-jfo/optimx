/********************************************************************************
** Form generated from reading UI file 'import_from_mad_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMPORT_FROM_MAD_DIALOG_H
#define UI_IMPORT_FROM_MAD_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImportFromMADDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEditEnergy;
    QLabel *label_2;
    QLineEdit *lineEditMass;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *ImportFromMADDialog)
    {
        if (ImportFromMADDialog->objectName().isEmpty())
            ImportFromMADDialog->setObjectName("ImportFromMADDialog");
        ImportFromMADDialog->resize(516, 257);
        buttonBox = new QDialogButtonBox(ImportFromMADDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(170, 200, 157, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(ImportFromMADDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(100, 130, 274, 50));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEditEnergy = new QLineEdit(layoutWidget);
        lineEditEnergy->setObjectName("lineEditEnergy");

        gridLayout->addWidget(lineEditEnergy, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEditMass = new QLineEdit(layoutWidget);
        lineEditMass->setObjectName("lineEditMass");

        gridLayout->addWidget(lineEditMass, 1, 1, 1, 1);

        lineEdit = new QLineEdit(ImportFromMADDialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(200, 50, 301, 21));
        lineEdit_2 = new QLineEdit(ImportFromMADDialog);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setGeometry(QRect(200, 80, 301, 21));
        label_3 = new QLabel(ImportFromMADDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 50, 91, 20));
        label_4 = new QLabel(ImportFromMADDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 80, 81, 16));
        pushButton = new QPushButton(ImportFromMADDialog);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(100, 50, 89, 22));
        pushButton_2 = new QPushButton(ImportFromMADDialog);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(100, 80, 89, 22));

        retranslateUi(ImportFromMADDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ImportFromMADDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ImportFromMADDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ImportFromMADDialog);
    } // setupUi

    void retranslateUi(QDialog *ImportFromMADDialog)
    {
        ImportFromMADDialog->setWindowTitle(QCoreApplication::translate("ImportFromMADDialog", "Import from MAD", nullptr));
        label->setText(QCoreApplication::translate("ImportFromMADDialog", "Particle Energy [MeV]", nullptr));
        label_2->setText(QCoreApplication::translate("ImportFromMADDialog", "Particle Mass [MeV/c**2]", nullptr));
        label_3->setText(QCoreApplication::translate("ImportFromMADDialog", "From: MAD ", nullptr));
        label_4->setText(QCoreApplication::translate("ImportFromMADDialog", "To: OptiM ", nullptr));
        pushButton->setText(QCoreApplication::translate("ImportFromMADDialog", "Select File", nullptr));
        pushButton_2->setText(QCoreApplication::translate("ImportFromMADDialog", "Select File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImportFromMADDialog: public Ui_ImportFromMADDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMPORT_FROM_MAD_DIALOG_H
