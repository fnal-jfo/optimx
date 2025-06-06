/********************************************************************************
** Form generated from reading UI file 'find_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FIND_DIALOG_H
#define UI_FIND_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FindDialog
{
public:
    QLineEdit *lineEditWhat;
    QLabel *label;
    QCheckBox *checkBoxMatchWord;
    QCheckBox *checkBoxMatchCase;
    QGroupBox *groupBox;
    QRadioButton *radioButtonDown;
    QRadioButton *radioButtonUp;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QDialogButtonBox *buttonBox;
    QPushButton *pushButtonNext;

    void setupUi(QDialog *FindDialog)
    {
        if (FindDialog->objectName().isEmpty())
            FindDialog->setObjectName("FindDialog");
        FindDialog->resize(512, 162);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FindDialog->sizePolicy().hasHeightForWidth());
        FindDialog->setSizePolicy(sizePolicy);
        FindDialog->setMaximumSize(QSize(512, 162));
        FindDialog->setModal(false);
        lineEditWhat = new QLineEdit(FindDialog);
        lineEditWhat->setObjectName("lineEditWhat");
        lineEditWhat->setGeometry(QRect(90, 20, 261, 21));
        label = new QLabel(FindDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 30, 55, 13));
        checkBoxMatchWord = new QCheckBox(FindDialog);
        checkBoxMatchWord->setObjectName("checkBoxMatchWord");
        checkBoxMatchWord->setGeometry(QRect(20, 70, 181, 21));
        checkBoxMatchCase = new QCheckBox(FindDialog);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(20, 100, 181, 21));
        groupBox = new QGroupBox(FindDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(200, 50, 101, 81));
        groupBox->setCheckable(false);
        radioButtonDown = new QRadioButton(groupBox);
        radioButtonDown->setObjectName("radioButtonDown");
        radioButtonDown->setGeometry(QRect(20, 50, 93, 21));
        radioButtonDown->setChecked(true);
        radioButtonUp = new QRadioButton(groupBox);
        radioButtonUp->setObjectName("radioButtonUp");
        radioButtonUp->setGeometry(QRect(20, 20, 93, 21));
        layoutWidget = new QWidget(FindDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(310, 60, 176, 72));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        buttonBox = new QDialogButtonBox(layoutWidget);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

        pushButtonNext = new QPushButton(layoutWidget);
        pushButtonNext->setObjectName("pushButtonNext");

        verticalLayout->addWidget(pushButtonNext);


        retranslateUi(FindDialog);
        QObject::connect(pushButtonNext, SIGNAL(clicked()), FindDialog, SLOT(next()));
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, FindDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, FindDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(FindDialog);
    } // setupUi

    void retranslateUi(QDialog *FindDialog)
    {
        FindDialog->setWindowTitle(QCoreApplication::translate("FindDialog", "Find", nullptr));
        label->setText(QCoreApplication::translate("FindDialog", "Find What", nullptr));
        checkBoxMatchWord->setText(QCoreApplication::translate("FindDialog", "Match Whole Word Only", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("FindDialog", "Match Case", nullptr));
        groupBox->setTitle(QCoreApplication::translate("FindDialog", "Direction", nullptr));
        radioButtonDown->setText(QCoreApplication::translate("FindDialog", "&Down", nullptr));
        radioButtonUp->setText(QCoreApplication::translate("FindDialog", "&Up", nullptr));
        pushButtonNext->setText(QCoreApplication::translate("FindDialog", "Next", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FindDialog: public Ui_FindDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FIND_DIALOG_H
