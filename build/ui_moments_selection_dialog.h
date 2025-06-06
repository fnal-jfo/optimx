/********************************************************************************
** Form generated from reading UI file 'moments_selection_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOMENTS_SELECTION_DIALOG_H
#define UI_MOMENTS_SELECTION_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MomentsSelectionDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QCheckBox *checkBox10;
    QCheckBox *checkBox31;
    QCheckBox *checkBox20;
    QLabel *label_13;
    QLabel *label_4;
    QLabel *label_3;
    QLabel *label_12;
    QCheckBox *checkBox53;
    QCheckBox *checkBox33;
    QCheckBox *checkBox54;
    QCheckBox *checkBox41;
    QLabel *label_8;
    QLabel *label_10;
    QCheckBox *checkBox42;
    QCheckBox *checkBox30;
    QCheckBox *checkBox21;
    QLabel *label_14;
    QCheckBox *checkBox00;
    QLabel *label_5;
    QCheckBox *checkBox22;
    QCheckBox *checkBox44;
    QCheckBox *checkBox52;
    QLabel *label_7;
    QCheckBox *checkBox32;
    QLabel *label_11;
    QLabel *label_9;
    QCheckBox *checkBox11;
    QLabel *label_6;
    QCheckBox *checkBox40;
    QCheckBox *checkBox55;
    QCheckBox *checkBox50;
    QCheckBox *checkBox51;
    QCheckBox *checkBox43;
    QButtonGroup *buttonGroup;

    void setupUi(QDialog *MomentsSelectionDialog)
    {
        if (MomentsSelectionDialog->objectName().isEmpty())
            MomentsSelectionDialog->setObjectName("MomentsSelectionDialog");
        MomentsSelectionDialog->setEnabled(true);
        MomentsSelectionDialog->resize(623, 327);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MomentsSelectionDialog->sizePolicy().hasHeightForWidth());
        MomentsSelectionDialog->setSizePolicy(sizePolicy);
        MomentsSelectionDialog->setMinimumSize(QSize(350, 225));
        MomentsSelectionDialog->setMaximumSize(QSize(100000, 100000));
        MomentsSelectionDialog->setBaseSize(QSize(246, 165));
        QFont font;
        font.setPointSize(8);
        MomentsSelectionDialog->setFont(font);
        buttonBox = new QDialogButtonBox(MomentsSelectionDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(230, 280, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(MomentsSelectionDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 10, 591, 261));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 40, 559, 197));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        checkBox10 = new QCheckBox(layoutWidget);
        buttonGroup = new QButtonGroup(MomentsSelectionDialog);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(checkBox10);
        checkBox10->setObjectName("checkBox10");

        gridLayout_2->addWidget(checkBox10, 2, 1, 1, 1);

        checkBox31 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox31);
        checkBox31->setObjectName("checkBox31");

        gridLayout_2->addWidget(checkBox31, 4, 2, 1, 1);

        checkBox20 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox20);
        checkBox20->setObjectName("checkBox20");

        gridLayout_2->addWidget(checkBox20, 3, 1, 1, 1);

        label_13 = new QLabel(layoutWidget);
        label_13->setObjectName("label_13");

        gridLayout_2->addWidget(label_13, 0, 5, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        gridLayout_2->addWidget(label_4, 2, 0, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        label_12 = new QLabel(layoutWidget);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 0, 4, 1, 1);

        checkBox53 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox53);
        checkBox53->setObjectName("checkBox53");

        gridLayout_2->addWidget(checkBox53, 6, 4, 1, 1);

        checkBox33 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox33);
        checkBox33->setObjectName("checkBox33");

        gridLayout_2->addWidget(checkBox33, 4, 4, 1, 1);

        checkBox54 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox54);
        checkBox54->setObjectName("checkBox54");

        gridLayout_2->addWidget(checkBox54, 6, 5, 1, 1);

        checkBox41 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox41);
        checkBox41->setObjectName("checkBox41");

        gridLayout_2->addWidget(checkBox41, 5, 2, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName("label_8");

        gridLayout_2->addWidget(label_8, 6, 0, 1, 1);

        label_10 = new QLabel(layoutWidget);
        label_10->setObjectName("label_10");

        gridLayout_2->addWidget(label_10, 0, 2, 1, 1);

        checkBox42 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox42);
        checkBox42->setObjectName("checkBox42");

        gridLayout_2->addWidget(checkBox42, 5, 3, 1, 1);

        checkBox30 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox30);
        checkBox30->setObjectName("checkBox30");

        gridLayout_2->addWidget(checkBox30, 4, 1, 1, 1);

        checkBox21 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox21);
        checkBox21->setObjectName("checkBox21");

        gridLayout_2->addWidget(checkBox21, 3, 2, 1, 1);

        label_14 = new QLabel(layoutWidget);
        label_14->setObjectName("label_14");

        gridLayout_2->addWidget(label_14, 0, 6, 1, 1);

        checkBox00 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox00);
        checkBox00->setObjectName("checkBox00");

        gridLayout_2->addWidget(checkBox00, 1, 1, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        gridLayout_2->addWidget(label_5, 3, 0, 1, 1);

        checkBox22 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox22);
        checkBox22->setObjectName("checkBox22");

        gridLayout_2->addWidget(checkBox22, 3, 3, 1, 1);

        checkBox44 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox44);
        checkBox44->setObjectName("checkBox44");

        gridLayout_2->addWidget(checkBox44, 5, 5, 1, 1);

        checkBox52 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox52);
        checkBox52->setObjectName("checkBox52");

        gridLayout_2->addWidget(checkBox52, 6, 3, 1, 1);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName("label_7");

        gridLayout_2->addWidget(label_7, 5, 0, 1, 1);

        checkBox32 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox32);
        checkBox32->setObjectName("checkBox32");

        gridLayout_2->addWidget(checkBox32, 4, 3, 1, 1);

        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 0, 3, 1, 1);

        label_9 = new QLabel(layoutWidget);
        label_9->setObjectName("label_9");

        gridLayout_2->addWidget(label_9, 0, 1, 1, 1);

        checkBox11 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox11);
        checkBox11->setObjectName("checkBox11");

        gridLayout_2->addWidget(checkBox11, 2, 2, 1, 1);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        gridLayout_2->addWidget(label_6, 4, 0, 1, 1);

        checkBox40 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox40);
        checkBox40->setObjectName("checkBox40");

        gridLayout_2->addWidget(checkBox40, 5, 1, 1, 1);

        checkBox55 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox55);
        checkBox55->setObjectName("checkBox55");

        gridLayout_2->addWidget(checkBox55, 6, 6, 1, 1);

        checkBox50 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox50);
        checkBox50->setObjectName("checkBox50");

        gridLayout_2->addWidget(checkBox50, 6, 1, 1, 1);

        checkBox51 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox51);
        checkBox51->setObjectName("checkBox51");

        gridLayout_2->addWidget(checkBox51, 6, 2, 1, 1);

        checkBox43 = new QCheckBox(layoutWidget);
        buttonGroup->addButton(checkBox43);
        checkBox43->setObjectName("checkBox43");

        gridLayout_2->addWidget(checkBox43, 5, 4, 1, 1);


        retranslateUi(MomentsSelectionDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, MomentsSelectionDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, MomentsSelectionDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(MomentsSelectionDialog);
    } // setupUi

    void retranslateUi(QDialog *MomentsSelectionDialog)
    {
        MomentsSelectionDialog->setWindowTitle(QCoreApplication::translate("MomentsSelectionDialog", "Moments Selection Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MomentsSelectionDialog", "Moments Selection", nullptr));
        checkBox10->setText(QString());
        checkBox31->setText(QString());
        checkBox20->setText(QString());
        label_13->setText(QCoreApplication::translate("MomentsSelectionDialog", "Z", nullptr));
        label_4->setText(QCoreApplication::translate("MomentsSelectionDialog", "X'", nullptr));
        label_3->setText(QCoreApplication::translate("MomentsSelectionDialog", "X", nullptr));
        label_12->setText(QCoreApplication::translate("MomentsSelectionDialog", "Y'", nullptr));
        checkBox53->setText(QString());
        checkBox33->setText(QString());
        checkBox54->setText(QString());
        checkBox41->setText(QString());
        label_8->setText(QCoreApplication::translate("MomentsSelectionDialog", "dp/p", nullptr));
        label_10->setText(QCoreApplication::translate("MomentsSelectionDialog", "X'", nullptr));
        checkBox42->setText(QString());
        checkBox30->setText(QString());
        checkBox21->setText(QString());
        label_14->setText(QCoreApplication::translate("MomentsSelectionDialog", "dp/p", nullptr));
        checkBox00->setText(QString());
        label_5->setText(QCoreApplication::translate("MomentsSelectionDialog", "Y", nullptr));
        checkBox22->setText(QString());
        checkBox44->setText(QString());
        checkBox52->setText(QString());
        label_7->setText(QCoreApplication::translate("MomentsSelectionDialog", "Z", nullptr));
        checkBox32->setText(QString());
        label_11->setText(QCoreApplication::translate("MomentsSelectionDialog", "Y", nullptr));
        label_9->setText(QCoreApplication::translate("MomentsSelectionDialog", "X", nullptr));
        checkBox11->setText(QString());
        label_6->setText(QCoreApplication::translate("MomentsSelectionDialog", "Y'", nullptr));
        checkBox40->setText(QString());
        checkBox55->setText(QString());
        checkBox50->setText(QString());
        checkBox51->setText(QString());
        checkBox43->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MomentsSelectionDialog: public Ui_MomentsSelectionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOMENTS_SELECTION_DIALOG_H
