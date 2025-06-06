/********************************************************************************
** Form generated from reading UI file 'track_save_positions_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACK_SAVE_POSITIONS_DIALOG_H
#define UI_TRACK_SAVE_POSITIONS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackSavePositionsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditFilter;
    QCheckBox *checkBoxMatchCase;
    QLabel *labelFilter;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QCheckBox *checkBoxX;
    QCheckBox *checkBoxThetaX;
    QCheckBox *checkBoxY;
    QCheckBox *checkBoxThetaY;
    QCheckBox *checkBoxZ;
    QCheckBox *checkBoxdPP;
    QLineEdit *lineEditFileName;
    QLabel *label;
    QPushButton *pushButton;

    void setupUi(QDialog *TrackSavePositionsDialog)
    {
        if (TrackSavePositionsDialog->objectName().isEmpty())
            TrackSavePositionsDialog->setObjectName("TrackSavePositionsDialog");
        TrackSavePositionsDialog->resize(340, 300);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TrackSavePositionsDialog->sizePolicy().hasHeightForWidth());
        TrackSavePositionsDialog->setSizePolicy(sizePolicy);
        TrackSavePositionsDialog->setMinimumSize(QSize(340, 300));
        TrackSavePositionsDialog->setMaximumSize(QSize(340, 300));
        buttonBox = new QDialogButtonBox(TrackSavePositionsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(90, 220, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEditFilter = new QLineEdit(TrackSavePositionsDialog);
        lineEditFilter->setObjectName("lineEditFilter");
        lineEditFilter->setGeometry(QRect(80, 60, 113, 21));
        checkBoxMatchCase = new QCheckBox(TrackSavePositionsDialog);
        checkBoxMatchCase->setObjectName("checkBoxMatchCase");
        checkBoxMatchCase->setGeometry(QRect(10, 90, 101, 21));
        labelFilter = new QLabel(TrackSavePositionsDialog);
        labelFilter->setObjectName("labelFilter");
        labelFilter->setGeometry(QRect(10, 60, 31, 16));
        layoutWidget = new QWidget(TrackSavePositionsDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(100, 120, 121, 81));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        checkBoxX = new QCheckBox(layoutWidget);
        checkBoxX->setObjectName("checkBoxX");

        formLayout->setWidget(0, QFormLayout::LabelRole, checkBoxX);

        checkBoxThetaX = new QCheckBox(layoutWidget);
        checkBoxThetaX->setObjectName("checkBoxThetaX");

        formLayout->setWidget(0, QFormLayout::FieldRole, checkBoxThetaX);

        checkBoxY = new QCheckBox(layoutWidget);
        checkBoxY->setObjectName("checkBoxY");

        formLayout->setWidget(1, QFormLayout::LabelRole, checkBoxY);

        checkBoxThetaY = new QCheckBox(layoutWidget);
        checkBoxThetaY->setObjectName("checkBoxThetaY");

        formLayout->setWidget(1, QFormLayout::FieldRole, checkBoxThetaY);

        checkBoxZ = new QCheckBox(layoutWidget);
        checkBoxZ->setObjectName("checkBoxZ");

        formLayout->setWidget(2, QFormLayout::LabelRole, checkBoxZ);

        checkBoxdPP = new QCheckBox(layoutWidget);
        checkBoxdPP->setObjectName("checkBoxdPP");

        formLayout->setWidget(2, QFormLayout::FieldRole, checkBoxdPP);

        lineEditFileName = new QLineEdit(TrackSavePositionsDialog);
        lineEditFileName->setObjectName("lineEditFileName");
        lineEditFileName->setGeometry(QRect(80, 30, 201, 21));
        label = new QLabel(TrackSavePositionsDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 30, 55, 13));
        pushButton = new QPushButton(TrackSavePositionsDialog);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(290, 30, 31, 22));

        retranslateUi(TrackSavePositionsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TrackSavePositionsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TrackSavePositionsDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButton, SIGNAL(clicked()), TrackSavePositionsDialog, SLOT(selectFile()));

        QMetaObject::connectSlotsByName(TrackSavePositionsDialog);
    } // setupUi

    void retranslateUi(QDialog *TrackSavePositionsDialog)
    {
        TrackSavePositionsDialog->setWindowTitle(QCoreApplication::translate("TrackSavePositionsDialog", "Dialog", nullptr));
        lineEditFilter->setText(QCoreApplication::translate("TrackSavePositionsDialog", "*", nullptr));
        checkBoxMatchCase->setText(QCoreApplication::translate("TrackSavePositionsDialog", "Match Case", nullptr));
        labelFilter->setText(QCoreApplication::translate("TrackSavePositionsDialog", "Filter", nullptr));
        checkBoxX->setText(QCoreApplication::translate("TrackSavePositionsDialog", "x", nullptr));
        checkBoxThetaX->setText(QCoreApplication::translate("TrackSavePositionsDialog", "theta x", nullptr));
        checkBoxY->setText(QCoreApplication::translate("TrackSavePositionsDialog", "y", nullptr));
        checkBoxThetaY->setText(QCoreApplication::translate("TrackSavePositionsDialog", "theta y", nullptr));
        checkBoxZ->setText(QCoreApplication::translate("TrackSavePositionsDialog", "z", nullptr));
        checkBoxdPP->setText(QCoreApplication::translate("TrackSavePositionsDialog", "delta P/P", nullptr));
        label->setText(QCoreApplication::translate("TrackSavePositionsDialog", "File name", nullptr));
        pushButton->setText(QCoreApplication::translate("TrackSavePositionsDialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackSavePositionsDialog: public Ui_TrackSavePositionsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACK_SAVE_POSITIONS_DIALOG_H
