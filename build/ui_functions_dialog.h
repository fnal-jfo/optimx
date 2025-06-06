/********************************************************************************
** Form generated from reading UI file 'functions_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUNCTIONS_DIALOG_H
#define UI_FUNCTIONS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FunctionsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QCheckBox *checkBox_5;
    QWidget *widget;
    QGridLayout *gridLayout;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_4;
    QCheckBox *checkBox;
    QWidget *widget1;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLineEdit *lineEdit_3;
    QLabel *label_4;
    QLineEdit *lineEdit_4;

    void setupUi(QDialog *FunctionsDialog)
    {
        if (FunctionsDialog->objectName().isEmpty())
            FunctionsDialog->setObjectName("FunctionsDialog");
        FunctionsDialog->resize(267, 331);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FunctionsDialog->sizePolicy().hasHeightForWidth());
        FunctionsDialog->setSizePolicy(sizePolicy);
        FunctionsDialog->setMinimumSize(QSize(267, 331));
        FunctionsDialog->setMaximumSize(QSize(267, 331));
        QFont font;
        font.setPointSize(8);
        FunctionsDialog->setFont(font);
        buttonBox = new QDialogButtonBox(FunctionsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(40, 290, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit = new QLineEdit(FunctionsDialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(91, 20, 164, 22));
        label = new QLabel(FunctionsDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 91, 16));
        checkBox_5 = new QCheckBox(FunctionsDialog);
        checkBox_5->setObjectName("checkBox_5");
        checkBox_5->setGeometry(QRect(23, 265, 221, 21));
        widget = new QWidget(FunctionsDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(55, 48, 160, 100));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        checkBox_3 = new QCheckBox(widget);
        checkBox_3->setObjectName("checkBox_3");

        gridLayout->addWidget(checkBox_3, 3, 0, 1, 2);

        checkBox_2 = new QCheckBox(widget);
        checkBox_2->setObjectName("checkBox_2");

        gridLayout->addWidget(checkBox_2, 2, 0, 1, 2);

        checkBox_4 = new QCheckBox(widget);
        checkBox_4->setObjectName("checkBox_4");

        gridLayout->addWidget(checkBox_4, 4, 0, 1, 1);

        checkBox = new QCheckBox(widget);
        checkBox->setObjectName("checkBox");

        gridLayout->addWidget(checkBox, 1, 0, 1, 1);

        widget1 = new QWidget(FunctionsDialog);
        widget1->setObjectName("widget1");
        widget1->setGeometry(QRect(22, 153, 225, 101));
        gridLayout_2 = new QGridLayout(widget1);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(widget1);
        label_2->setObjectName("label_2");

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        lineEdit_2 = new QLineEdit(widget1);
        lineEdit_2->setObjectName("lineEdit_2");

        gridLayout_2->addWidget(lineEdit_2, 0, 1, 1, 1);

        label_3 = new QLabel(widget1);
        label_3->setObjectName("label_3");

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        lineEdit_3 = new QLineEdit(widget1);
        lineEdit_3->setObjectName("lineEdit_3");

        gridLayout_2->addWidget(lineEdit_3, 1, 1, 1, 1);

        label_4 = new QLabel(widget1);
        label_4->setObjectName("label_4");

        gridLayout_2->addWidget(label_4, 2, 0, 1, 1);

        lineEdit_4 = new QLineEdit(widget1);
        lineEdit_4->setObjectName("lineEdit_4");

        gridLayout_2->addWidget(lineEdit_4, 2, 1, 1, 1);


        retranslateUi(FunctionsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, FunctionsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, FunctionsDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(FunctionsDialog);
    } // setupUi

    void retranslateUi(QDialog *FunctionsDialog)
    {
        FunctionsDialog->setWindowTitle(QCoreApplication::translate("FunctionsDialog", "Functions Dialog", nullptr));
        label->setText(QCoreApplication::translate("FunctionsDialog", "Filter [*,%,...]", nullptr));
        checkBox_5->setText(QCoreApplication::translate("FunctionsDialog", "Start s-coordinates from zero", nullptr));
        checkBox_3->setText(QCoreApplication::translate("FunctionsDialog", "Omit first character", nullptr));
        checkBox_2->setText(QCoreApplication::translate("FunctionsDialog", "Print element name", nullptr));
        checkBox_4->setText(QCoreApplication::translate("FunctionsDialog", "Print Energy", nullptr));
        checkBox->setText(QCoreApplication::translate("FunctionsDialog", "match case", nullptr));
        label_2->setText(QCoreApplication::translate("FunctionsDialog", "Step [cm]", nullptr));
        label_3->setText(QCoreApplication::translate("FunctionsDialog", "Start from element", nullptr));
        label_4->setText(QCoreApplication::translate("FunctionsDialog", "Stop at element", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FunctionsDialog: public Ui_FunctionsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUNCTIONS_DIALOG_H
