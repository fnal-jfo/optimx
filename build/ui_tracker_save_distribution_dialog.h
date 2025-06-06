/********************************************************************************
** Form generated from reading UI file 'tracker_save_distribution_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKER_SAVE_DISTRIBUTION_DIALOG_H
#define UI_TRACKER_SAVE_DISTRIBUTION_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_TrackerSaveDistributionDialog
{
public:
    QDialogButtonBox *buttonBox;
    QRadioButton *radioButtonSaveGood;
    QRadioButton *radioButtonSaveAll;

    void setupUi(QDialog *TrackerSaveDistributionDialog)
    {
        if (TrackerSaveDistributionDialog->objectName().isEmpty())
            TrackerSaveDistributionDialog->setObjectName("TrackerSaveDistributionDialog");
        TrackerSaveDistributionDialog->resize(305, 167);
        buttonBox = new QDialogButtonBox(TrackerSaveDistributionDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(58, 109, 189, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        radioButtonSaveGood = new QRadioButton(TrackerSaveDistributionDialog);
        radioButtonSaveGood->setObjectName("radioButtonSaveGood");
        radioButtonSaveGood->setGeometry(QRect(22, 20, 211, 20));
        radioButtonSaveGood->setChecked(true);
        radioButtonSaveAll = new QRadioButton(TrackerSaveDistributionDialog);
        radioButtonSaveAll->setObjectName("radioButtonSaveAll");
        radioButtonSaveAll->setGeometry(QRect(21, 62, 260, 20));

        retranslateUi(TrackerSaveDistributionDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TrackerSaveDistributionDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TrackerSaveDistributionDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TrackerSaveDistributionDialog);
    } // setupUi

    void retranslateUi(QDialog *TrackerSaveDistributionDialog)
    {
        TrackerSaveDistributionDialog->setWindowTitle(QCoreApplication::translate("TrackerSaveDistributionDialog", "Dialog", nullptr));
        radioButtonSaveGood->setText(QCoreApplication::translate("TrackerSaveDistributionDialog", "Save good partic&les only", nullptr));
        radioButtonSaveAll->setText(QCoreApplication::translate("TrackerSaveDistributionDialog", "Save all par&ticles and loss information", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackerSaveDistributionDialog: public Ui_TrackerSaveDistributionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACKER_SAVE_DISTRIBUTION_DIALOG_H
