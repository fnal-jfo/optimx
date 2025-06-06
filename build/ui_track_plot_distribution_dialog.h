/********************************************************************************
** Form generated from reading UI file 'track_plot_distribution_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACK_PLOT_DISTRIBUTION_DIALOG_H
#define UI_TRACK_PLOT_DISTRIBUTION_DIALOG_H

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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackPlotDistributionDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBoxX;
    QCheckBox *checkBoxY;
    QCheckBox *checkBoxS;
    QCheckBox *checkBoxPx;
    QCheckBox *checkBoxPy;
    QCheckBox *checkBoxPs;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QSpinBox *spinBoxNbins;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxInitial;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_3;
    QDoubleSpinBox *doubleSpinBoxFinal;

    void setupUi(QDialog *TrackPlotDistributionDialog)
    {
        if (TrackPlotDistributionDialog->objectName().isEmpty())
            TrackPlotDistributionDialog->setObjectName("TrackPlotDistributionDialog");
        TrackPlotDistributionDialog->resize(463, 219);
        buttonBox = new QDialogButtonBox(TrackPlotDistributionDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(130, 160, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(TrackPlotDistributionDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(330, 10, 91, 181));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(30, 20, 40, 148));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBoxX = new QCheckBox(layoutWidget);
        checkBoxX->setObjectName("checkBoxX");

        verticalLayout->addWidget(checkBoxX);

        checkBoxY = new QCheckBox(layoutWidget);
        checkBoxY->setObjectName("checkBoxY");

        verticalLayout->addWidget(checkBoxY);

        checkBoxS = new QCheckBox(layoutWidget);
        checkBoxS->setObjectName("checkBoxS");

        verticalLayout->addWidget(checkBoxS);

        checkBoxPx = new QCheckBox(layoutWidget);
        checkBoxPx->setObjectName("checkBoxPx");

        verticalLayout->addWidget(checkBoxPx);

        checkBoxPy = new QCheckBox(layoutWidget);
        checkBoxPy->setObjectName("checkBoxPy");

        verticalLayout->addWidget(checkBoxPy);

        checkBoxPs = new QCheckBox(layoutWidget);
        checkBoxPs->setObjectName("checkBoxPs");

        verticalLayout->addWidget(checkBoxPs);

        widget = new QWidget(TrackPlotDistributionDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(40, 20, 262, 84));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 1, 2, 2);

        spinBoxNbins = new QSpinBox(widget);
        spinBoxNbins->setObjectName("spinBoxNbins");

        gridLayout->addWidget(spinBoxNbins, 0, 3, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 2, 2, 2);

        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 2, 0, 1, 2);

        doubleSpinBoxInitial = new QDoubleSpinBox(widget);
        doubleSpinBoxInitial->setObjectName("doubleSpinBoxInitial");

        gridLayout->addWidget(doubleSpinBoxInitial, 2, 3, 1, 1);

        label_3 = new QLabel(widget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 3, 0, 2, 2);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 3, 2, 2, 2);

        doubleSpinBoxFinal = new QDoubleSpinBox(widget);
        doubleSpinBoxFinal->setObjectName("doubleSpinBoxFinal");

        gridLayout->addWidget(doubleSpinBoxFinal, 4, 3, 1, 1);


        retranslateUi(TrackPlotDistributionDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TrackPlotDistributionDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TrackPlotDistributionDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TrackPlotDistributionDialog);
    } // setupUi

    void retranslateUi(QDialog *TrackPlotDistributionDialog)
    {
        TrackPlotDistributionDialog->setWindowTitle(QCoreApplication::translate("TrackPlotDistributionDialog", "Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("TrackPlotDistributionDialog", "Projection", nullptr));
        checkBoxX->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "X", nullptr));
        checkBoxY->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "Y", nullptr));
        checkBoxS->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "S", nullptr));
        checkBoxPx->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "Px", nullptr));
        checkBoxPy->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "Py", nullptr));
        checkBoxPs->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "Ps", nullptr));
        label->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "No of bins", nullptr));
        label_2->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "Initial coordinate [ cm or mrad]", nullptr));
        label_3->setText(QCoreApplication::translate("TrackPlotDistributionDialog", "Final coordinate [cm or mrad]", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackPlotDistributionDialog: public Ui_TrackPlotDistributionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACK_PLOT_DISTRIBUTION_DIALOG_H
