/********************************************************************************
** Form generated from reading UI file 'edit_measured_data_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDIT_MEASURED_DATA_DIALOG_H
#define UI_EDIT_MEASURED_DATA_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_EditMeasuredDataDialog
{
public:
    QLabel *label_10;
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBoxShowLegend;
    QCheckBox *checkBoxShowFullPath;
    QGroupBox *groupBox;
    QComboBox *comboBoxDataSets;
    QLabel *label;
    QRadioButton *radioButtonSetColor;
    QRadioButton *radioButtonRemoveSet;
    QLabel *labelColor;
    QCheckBox *checkBoxDataPoints;
    QButtonGroup *buttonGroup;

    void setupUi(QDialog *EditMeasuredDataDialog)
    {
        if (EditMeasuredDataDialog->objectName().isEmpty())
            EditMeasuredDataDialog->setObjectName("EditMeasuredDataDialog");
        EditMeasuredDataDialog->resize(300, 400);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(EditMeasuredDataDialog->sizePolicy().hasHeightForWidth());
        EditMeasuredDataDialog->setSizePolicy(sizePolicy);
        EditMeasuredDataDialog->setMinimumSize(QSize(300, 400));
        EditMeasuredDataDialog->setMaximumSize(QSize(300, 400));
        EditMeasuredDataDialog->setAutoFillBackground(false);
        EditMeasuredDataDialog->setSizeGripEnabled(false);
        label_10 = new QLabel(EditMeasuredDataDialog);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(30, 460, 55, 13));
        buttonBox = new QDialogButtonBox(EditMeasuredDataDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(57, 304, 161, 31));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        checkBoxShowLegend = new QCheckBox(EditMeasuredDataDialog);
        checkBoxShowLegend->setObjectName("checkBoxShowLegend");
        checkBoxShowLegend->setGeometry(QRect(40, 30, 111, 21));
        checkBoxShowFullPath = new QCheckBox(EditMeasuredDataDialog);
        checkBoxShowFullPath->setObjectName("checkBoxShowFullPath");
        checkBoxShowFullPath->setGeometry(QRect(39, 64, 220, 21));
        groupBox = new QGroupBox(EditMeasuredDataDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(21, 139, 241, 146));
        comboBoxDataSets = new QComboBox(groupBox);
        comboBoxDataSets->setObjectName("comboBoxDataSets");
        comboBoxDataSets->setGeometry(QRect(110, 40, 85, 22));
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setGeometry(QRect(40, 40, 71, 16));
        radioButtonSetColor = new QRadioButton(groupBox);
        buttonGroup = new QButtonGroup(EditMeasuredDataDialog);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(radioButtonSetColor);
        radioButtonSetColor->setObjectName("radioButtonSetColor");
        radioButtonSetColor->setGeometry(QRect(40, 70, 85, 23));
        radioButtonRemoveSet = new QRadioButton(groupBox);
        buttonGroup->addButton(radioButtonRemoveSet);
        radioButtonRemoveSet->setObjectName("radioButtonRemoveSet");
        radioButtonRemoveSet->setGeometry(QRect(40, 99, 76, 23));
        labelColor = new QLabel(groupBox);
        labelColor->setObjectName("labelColor");
        labelColor->setGeometry(QRect(154, 74, 50, 16));
        checkBoxDataPoints = new QCheckBox(EditMeasuredDataDialog);
        checkBoxDataPoints->setObjectName("checkBoxDataPoints");
        checkBoxDataPoints->setGeometry(QRect(38, 103, 166, 20));

        retranslateUi(EditMeasuredDataDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, EditMeasuredDataDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, EditMeasuredDataDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(EditMeasuredDataDialog);
    } // setupUi

    void retranslateUi(QDialog *EditMeasuredDataDialog)
    {
        EditMeasuredDataDialog->setWindowTitle(QCoreApplication::translate("EditMeasuredDataDialog", "Edit Measured Data", nullptr));
        label_10->setText(QString());
        checkBoxShowLegend->setText(QCoreApplication::translate("EditMeasuredDataDialog", "Show Legend ", nullptr));
        checkBoxShowFullPath->setText(QCoreApplication::translate("EditMeasuredDataDialog", "Show full paths in legend", nullptr));
        groupBox->setTitle(QCoreApplication::translate("EditMeasuredDataDialog", "Modify", nullptr));
        label->setText(QCoreApplication::translate("EditMeasuredDataDialog", "Data Set", nullptr));
        radioButtonSetColor->setText(QCoreApplication::translate("EditMeasuredDataDialog", "Set  Co&lor", nullptr));
        radioButtonRemoveSet->setText(QCoreApplication::translate("EditMeasuredDataDialog", "Remove", nullptr));
        labelColor->setText(QCoreApplication::translate("EditMeasuredDataDialog", "TextLabel", nullptr));
        checkBoxDataPoints->setText(QCoreApplication::translate("EditMeasuredDataDialog", "Show data points only", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EditMeasuredDataDialog: public Ui_EditMeasuredDataDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDIT_MEASURED_DATA_DIALOG_H
