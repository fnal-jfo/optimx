/********************************************************************************
** Form generated from reading UI file 'track_positions_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACK_POSITIONS_DIALOG_H
#define UI_TRACK_POSITIONS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackPositionsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButtonHx;
    QRadioButton *radioButtonHPx;
    QRadioButton *radioButtonHy;
    QRadioButton *radioButtonHPy;
    QRadioButton *radioButtonHs;
    QRadioButton *radioButtonHPs;
    QRadioButton *radioButtonHTurn;
    QGroupBox *groupBox_2;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *radioButtonVx;
    QRadioButton *radioButtonVPx;
    QRadioButton *radioButtonVy;
    QRadioButton *radioButtonVPy;
    QRadioButton *radioButtonVs;
    QRadioButton *radioButtonVPs;
    QRadioButton *radioButtonVTurn;

    void setupUi(QDialog *TrackPositionsDialog)
    {
        if (TrackPositionsDialog->objectName().isEmpty())
            TrackPositionsDialog->setObjectName("TrackPositionsDialog");
        TrackPositionsDialog->resize(280, 323);
        buttonBox = new QDialogButtonBox(TrackPositionsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(50, 260, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(TrackPositionsDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 10, 120, 231));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 30, 93, 173));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        radioButtonHx = new QRadioButton(layoutWidget);
        radioButtonHx->setObjectName("radioButtonHx");

        verticalLayout->addWidget(radioButtonHx);

        radioButtonHPx = new QRadioButton(layoutWidget);
        radioButtonHPx->setObjectName("radioButtonHPx");

        verticalLayout->addWidget(radioButtonHPx);

        radioButtonHy = new QRadioButton(layoutWidget);
        radioButtonHy->setObjectName("radioButtonHy");

        verticalLayout->addWidget(radioButtonHy);

        radioButtonHPy = new QRadioButton(layoutWidget);
        radioButtonHPy->setObjectName("radioButtonHPy");

        verticalLayout->addWidget(radioButtonHPy);

        radioButtonHs = new QRadioButton(layoutWidget);
        radioButtonHs->setObjectName("radioButtonHs");

        verticalLayout->addWidget(radioButtonHs);

        radioButtonHPs = new QRadioButton(layoutWidget);
        radioButtonHPs->setObjectName("radioButtonHPs");

        verticalLayout->addWidget(radioButtonHPs);

        radioButtonHTurn = new QRadioButton(layoutWidget);
        radioButtonHTurn->setObjectName("radioButtonHTurn");

        verticalLayout->addWidget(radioButtonHTurn);

        groupBox_2 = new QGroupBox(TrackPositionsDialog);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(140, 10, 131, 221));
        layoutWidget_2 = new QWidget(groupBox_2);
        layoutWidget_2->setObjectName("layoutWidget_2");
        layoutWidget_2->setGeometry(QRect(20, 30, 93, 173));
        verticalLayout_2 = new QVBoxLayout(layoutWidget_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        radioButtonVx = new QRadioButton(layoutWidget_2);
        radioButtonVx->setObjectName("radioButtonVx");

        verticalLayout_2->addWidget(radioButtonVx);

        radioButtonVPx = new QRadioButton(layoutWidget_2);
        radioButtonVPx->setObjectName("radioButtonVPx");

        verticalLayout_2->addWidget(radioButtonVPx);

        radioButtonVy = new QRadioButton(layoutWidget_2);
        radioButtonVy->setObjectName("radioButtonVy");

        verticalLayout_2->addWidget(radioButtonVy);

        radioButtonVPy = new QRadioButton(layoutWidget_2);
        radioButtonVPy->setObjectName("radioButtonVPy");

        verticalLayout_2->addWidget(radioButtonVPy);

        radioButtonVs = new QRadioButton(layoutWidget_2);
        radioButtonVs->setObjectName("radioButtonVs");

        verticalLayout_2->addWidget(radioButtonVs);

        radioButtonVPs = new QRadioButton(layoutWidget_2);
        radioButtonVPs->setObjectName("radioButtonVPs");

        verticalLayout_2->addWidget(radioButtonVPs);

        radioButtonVTurn = new QRadioButton(layoutWidget_2);
        radioButtonVTurn->setObjectName("radioButtonVTurn");

        verticalLayout_2->addWidget(radioButtonVTurn);


        retranslateUi(TrackPositionsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TrackPositionsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TrackPositionsDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TrackPositionsDialog);
    } // setupUi

    void retranslateUi(QDialog *TrackPositionsDialog)
    {
        TrackPositionsDialog->setWindowTitle(QCoreApplication::translate("TrackPositionsDialog", "Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("TrackPositionsDialog", "X axis", nullptr));
        radioButtonHx->setText(QCoreApplication::translate("TrackPositionsDialog", "X", nullptr));
        radioButtonHPx->setText(QCoreApplication::translate("TrackPositionsDialog", "Px", nullptr));
        radioButtonHy->setText(QCoreApplication::translate("TrackPositionsDialog", "Y", nullptr));
        radioButtonHPy->setText(QCoreApplication::translate("TrackPositionsDialog", "Py", nullptr));
        radioButtonHs->setText(QCoreApplication::translate("TrackPositionsDialog", "S", nullptr));
        radioButtonHPs->setText(QCoreApplication::translate("TrackPositionsDialog", "Ps", nullptr));
        radioButtonHTurn->setText(QCoreApplication::translate("TrackPositionsDialog", "turn/elm no", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("TrackPositionsDialog", "Y axis", nullptr));
        radioButtonVx->setText(QCoreApplication::translate("TrackPositionsDialog", "X", nullptr));
        radioButtonVPx->setText(QCoreApplication::translate("TrackPositionsDialog", "Px", nullptr));
        radioButtonVy->setText(QCoreApplication::translate("TrackPositionsDialog", "Y", nullptr));
        radioButtonVPy->setText(QCoreApplication::translate("TrackPositionsDialog", "Py", nullptr));
        radioButtonVs->setText(QCoreApplication::translate("TrackPositionsDialog", "S", nullptr));
        radioButtonVPs->setText(QCoreApplication::translate("TrackPositionsDialog", "Ps", nullptr));
        radioButtonVTurn->setText(QCoreApplication::translate("TrackPositionsDialog", "turn/elm no", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackPositionsDialog: public Ui_TrackPositionsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACK_POSITIONS_DIALOG_H
