/********************************************************************************
** Form generated from reading UI file 'plot_preferences_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOT_PREFERENCES_DIALOG_H
#define UI_PLOT_PREFERENCES_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlotPreferencesDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *spinBoxResolution;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxHSize;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBoxVSize;
    QGroupBox *groupBox_2;
    QSpinBox *spinBoxAxesLWidth;
    QLabel *label_3;
    QLabel *label_5;
    QSpinBox *spinBoxFontSize;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxLWidth;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBoxCompactScales;
    QCheckBox *checkBoxCompactLegend;
    QCheckBox *checkBoxTitle;
    QCheckBox *checkBoxColoredFrame;
    QCheckBox *checkBoxTimeStamp;
    QPushButton *pushButtonFont;

    void setupUi(QDialog *PlotPreferencesDialog)
    {
        if (PlotPreferencesDialog->objectName().isEmpty())
            PlotPreferencesDialog->setObjectName("PlotPreferencesDialog");
        PlotPreferencesDialog->resize(333, 666);
        buttonBox = new QDialogButtonBox(PlotPreferencesDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(60, 590, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(PlotPreferencesDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 400, 291, 171));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 31, 244, 113));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        spinBoxResolution = new QSpinBox(layoutWidget);
        spinBoxResolution->setObjectName("spinBoxResolution");

        gridLayout->addWidget(spinBoxResolution, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        doubleSpinBoxHSize = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxHSize->setObjectName("doubleSpinBoxHSize");

        gridLayout->addWidget(doubleSpinBoxHSize, 1, 1, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 2, 0, 1, 1);

        doubleSpinBoxVSize = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxVSize->setObjectName("doubleSpinBoxVSize");

        gridLayout->addWidget(doubleSpinBoxVSize, 2, 1, 1, 1);

        groupBox_2 = new QGroupBox(PlotPreferencesDialog);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(20, 20, 291, 341));
        spinBoxAxesLWidth = new QSpinBox(groupBox_2);
        spinBoxAxesLWidth->setObjectName("spinBoxAxesLWidth");
        spinBoxAxesLWidth->setGeometry(QRect(190, 60, 61, 22));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 30, 131, 16));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(20, 90, 161, 16));
        spinBoxFontSize = new QSpinBox(groupBox_2);
        spinBoxFontSize->setObjectName("spinBoxFontSize");
        spinBoxFontSize->setGeometry(QRect(190, 90, 61, 22));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(20, 60, 111, 16));
        doubleSpinBoxLWidth = new QDoubleSpinBox(groupBox_2);
        doubleSpinBoxLWidth->setObjectName("doubleSpinBoxLWidth");
        doubleSpinBoxLWidth->setGeometry(QRect(190, 30, 62, 22));
        layoutWidget1 = new QWidget(groupBox_2);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(20, 170, 216, 151));
        verticalLayout = new QVBoxLayout(layoutWidget1);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBoxCompactScales = new QCheckBox(layoutWidget1);
        checkBoxCompactScales->setObjectName("checkBoxCompactScales");

        verticalLayout->addWidget(checkBoxCompactScales);

        checkBoxCompactLegend = new QCheckBox(layoutWidget1);
        checkBoxCompactLegend->setObjectName("checkBoxCompactLegend");

        verticalLayout->addWidget(checkBoxCompactLegend);

        checkBoxTitle = new QCheckBox(layoutWidget1);
        checkBoxTitle->setObjectName("checkBoxTitle");
        checkBoxTitle->setEnabled(true);

        verticalLayout->addWidget(checkBoxTitle);

        checkBoxColoredFrame = new QCheckBox(layoutWidget1);
        checkBoxColoredFrame->setObjectName("checkBoxColoredFrame");
        checkBoxColoredFrame->setEnabled(true);

        verticalLayout->addWidget(checkBoxColoredFrame);

        checkBoxTimeStamp = new QCheckBox(layoutWidget1);
        checkBoxTimeStamp->setObjectName("checkBoxTimeStamp");
        checkBoxTimeStamp->setEnabled(true);

        verticalLayout->addWidget(checkBoxTimeStamp);

        pushButtonFont = new QPushButton(groupBox_2);
        pushButtonFont->setObjectName("pushButtonFont");
        pushButtonFont->setGeometry(QRect(150, 120, 84, 31));

        retranslateUi(PlotPreferencesDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, PlotPreferencesDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, PlotPreferencesDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButtonFont, SIGNAL(clicked()), PlotPreferencesDialog, SLOT(selectAxisFont()));

        QMetaObject::connectSlotsByName(PlotPreferencesDialog);
    } // setupUi

    void retranslateUi(QDialog *PlotPreferencesDialog)
    {
        PlotPreferencesDialog->setWindowTitle(QCoreApplication::translate("PlotPreferencesDialog", "Plot Preferences", nullptr));
        groupBox->setTitle(QCoreApplication::translate("PlotPreferencesDialog", "Exported Plots", nullptr));
        label->setText(QCoreApplication::translate("PlotPreferencesDialog", "Resolution (DPI)", nullptr));
        label_2->setText(QCoreApplication::translate("PlotPreferencesDialog", "Horizontal image size [in]", nullptr));
        label_4->setText(QCoreApplication::translate("PlotPreferencesDialog", "Vertical image size [in]", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("PlotPreferencesDialog", "General", nullptr));
        label_3->setText(QCoreApplication::translate("PlotPreferencesDialog", "Plot line width", nullptr));
        label_5->setText(QCoreApplication::translate("PlotPreferencesDialog", "Axes font size  [points]", nullptr));
        label_6->setText(QCoreApplication::translate("PlotPreferencesDialog", "Axes line width ", nullptr));
        checkBoxCompactScales->setText(QCoreApplication::translate("PlotPreferencesDialog", "Use compact scales", nullptr));
        checkBoxCompactLegend->setText(QCoreApplication::translate("PlotPreferencesDialog", "Use compact legend", nullptr));
        checkBoxTitle->setText(QCoreApplication::translate("PlotPreferencesDialog", "Show Plot Title", nullptr));
        checkBoxColoredFrame->setText(QCoreApplication::translate("PlotPreferencesDialog", "Colored Frame ", nullptr));
        checkBoxTimeStamp->setText(QCoreApplication::translate("PlotPreferencesDialog", "Display Timestamp", nullptr));
        pushButtonFont->setText(QCoreApplication::translate("PlotPreferencesDialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlotPreferencesDialog: public Ui_PlotPreferencesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOT_PREFERENCES_DIALOG_H
