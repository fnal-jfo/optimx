/********************************************************************************
** Form generated from reading UI file 'replace_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPLACE_DIALOG_H
#define UI_REPLACE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReplaceDialog
{
public:
    QLineEdit *lineEditWhat;
    QLabel *label;
    QCheckBox *checkBoxMatchWord;
    QCheckBox *checkBoxMatchCase;
    QLineEdit *lineEditWith;
    QLabel *label_2;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButtonNext;
    QPushButton *pushButtonReplace;
    QPushButton *pushButtonReplaceAll;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ReplaceDialog)
    {
        if (ReplaceDialog->objectName().isEmpty())
            ReplaceDialog->setObjectName("ReplaceDialog");
        ReplaceDialog->resize(415, 198);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ReplaceDialog->sizePolicy().hasHeightForWidth());
        ReplaceDialog->setSizePolicy(sizePolicy);
        ReplaceDialog->setMaximumSize(QSize(415, 200));
        lineEditWhat = new QLineEdit(ReplaceDialog);
        lineEditWhat->setObjectName("lineEditWhat");
        lineEditWhat->setGeometry(QRect(100, 20, 191, 21));
        label = new QLabel(ReplaceDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 30, 55, 13));
        checkBoxMatchWord = new QCheckBox(ReplaceDialog);
        checkBoxMatchWord->setObjectName("checkBoxMatchWord");
        checkBoxMatchWord->setGeometry(QRect(20, 100, 181, 21));
        checkBoxMatchCase = new QCheckBox(ReplaceDialog);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(20, 130, 181, 21));
        lineEditWith = new QLineEdit(ReplaceDialog);
        lineEditWith->setObjectName("lineEditWith");
        lineEditWith->setGeometry(QRect(100, 50, 191, 21));
        label_2 = new QLabel(ReplaceDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 60, 81, 16));
        layoutWidget = new QWidget(ReplaceDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(300, 20, 106, 105));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pushButtonNext = new QPushButton(layoutWidget);
        pushButtonNext->setObjectName("pushButtonNext");

        verticalLayout->addWidget(pushButtonNext);

        pushButtonReplace = new QPushButton(layoutWidget);
        pushButtonReplace->setObjectName("pushButtonReplace");

        verticalLayout->addWidget(pushButtonReplace);

        pushButtonReplaceAll = new QPushButton(layoutWidget);
        pushButtonReplaceAll->setObjectName("pushButtonReplaceAll");

        verticalLayout->addWidget(pushButtonReplaceAll);

        buttonBox = new QDialogButtonBox(layoutWidget);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(ReplaceDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ReplaceDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ReplaceDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButtonNext, SIGNAL(clicked()), ReplaceDialog, SLOT(findNext()));
        QObject::connect(pushButtonReplace, SIGNAL(clicked()), ReplaceDialog, SLOT(replaceOnce()));
        QObject::connect(pushButtonReplaceAll, SIGNAL(clicked()), ReplaceDialog, SLOT(replaceAll()));

        QMetaObject::connectSlotsByName(ReplaceDialog);
    } // setupUi

    void retranslateUi(QDialog *ReplaceDialog)
    {
        ReplaceDialog->setWindowTitle(QCoreApplication::translate("ReplaceDialog", "Replace", nullptr));
        label->setText(QCoreApplication::translate("ReplaceDialog", "Find What", nullptr));
        checkBoxMatchWord->setText(QCoreApplication::translate("ReplaceDialog", "Match Whole Word Only", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("ReplaceDialog", "Match Case", nullptr));
        label_2->setText(QCoreApplication::translate("ReplaceDialog", "Replace With", nullptr));
        pushButtonNext->setText(QCoreApplication::translate("ReplaceDialog", "Find Next", nullptr));
        pushButtonReplace->setText(QCoreApplication::translate("ReplaceDialog", "Replace", nullptr));
        pushButtonReplaceAll->setText(QCoreApplication::translate("ReplaceDialog", "Replace All", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ReplaceDialog: public Ui_ReplaceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPLACE_DIALOG_H
