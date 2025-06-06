/********************************************************************************
** Form generated from reading UI file 'tune_diagram_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TUNE_DIAGRAM_DIALOG_H
#define UI_TUNE_DIAGRAM_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TuneDiagramDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBoxSumResonances;
    QCheckBox *checkBoxCouplingResonances;
    QCheckBox *checkBoxCaptions;
    QWidget *layoutWidget_2;
    QGridLayout *gridLayout;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxQxMin;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBoxQxMax;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxQyMin;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBoxQyMax;
    QLabel *label_9;
    QLabel *label_10;
    QSpinBox *spinBoxMinOrder;
    QSpinBox *spinBoxMaxOrder;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QSpinBox *spinBoxQxIntervals;
    QLabel *label_4;
    QSpinBox *spinBoxQyIntervals;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxQx;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxQy;

    void setupUi(QDialog *TuneDiagramDialog)
    {
        if (TuneDiagramDialog->objectName().isEmpty())
            TuneDiagramDialog->setObjectName("TuneDiagramDialog");
        TuneDiagramDialog->resize(400, 350);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TuneDiagramDialog->sizePolicy().hasHeightForWidth());
        TuneDiagramDialog->setSizePolicy(sizePolicy);
        TuneDiagramDialog->setMinimumSize(QSize(400, 350));
        TuneDiagramDialog->setMaximumSize(QSize(400, 350));
        QFont font;
        font.setPointSize(8);
        TuneDiagramDialog->setFont(font);
        buttonBox = new QDialogButtonBox(TuneDiagramDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(117, 304, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(TuneDiagramDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(12, 128, 149, 74));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBoxSumResonances = new QCheckBox(layoutWidget);
        checkBoxSumResonances->setObjectName("checkBoxSumResonances");

        verticalLayout->addWidget(checkBoxSumResonances);

        checkBoxCouplingResonances = new QCheckBox(layoutWidget);
        checkBoxCouplingResonances->setObjectName("checkBoxCouplingResonances");

        verticalLayout->addWidget(checkBoxCouplingResonances);

        checkBoxCaptions = new QCheckBox(layoutWidget);
        checkBoxCaptions->setObjectName("checkBoxCaptions");

        verticalLayout->addWidget(checkBoxCaptions);

        layoutWidget_2 = new QWidget(TuneDiagramDialog);
        layoutWidget_2->setObjectName("layoutWidget_2");
        layoutWidget_2->setGeometry(QRect(10, 10, 331, 104));
        gridLayout = new QGridLayout(layoutWidget_2);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(layoutWidget_2);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 0, 0, 1, 1);

        doubleSpinBoxQxMin = new QDoubleSpinBox(layoutWidget_2);
        doubleSpinBoxQxMin->setObjectName("doubleSpinBoxQxMin");

        gridLayout->addWidget(doubleSpinBoxQxMin, 0, 1, 1, 1);

        label_7 = new QLabel(layoutWidget_2);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 0, 2, 1, 1);

        doubleSpinBoxQxMax = new QDoubleSpinBox(layoutWidget_2);
        doubleSpinBoxQxMax->setObjectName("doubleSpinBoxQxMax");

        gridLayout->addWidget(doubleSpinBoxQxMax, 0, 3, 1, 1);

        label_6 = new QLabel(layoutWidget_2);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 1, 0, 1, 1);

        doubleSpinBoxQyMin = new QDoubleSpinBox(layoutWidget_2);
        doubleSpinBoxQyMin->setObjectName("doubleSpinBoxQyMin");

        gridLayout->addWidget(doubleSpinBoxQyMin, 1, 1, 1, 1);

        label_8 = new QLabel(layoutWidget_2);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 1, 2, 1, 1);

        doubleSpinBoxQyMax = new QDoubleSpinBox(layoutWidget_2);
        doubleSpinBoxQyMax->setObjectName("doubleSpinBoxQyMax");

        gridLayout->addWidget(doubleSpinBoxQyMax, 1, 3, 1, 1);

        label_9 = new QLabel(layoutWidget_2);
        label_9->setObjectName("label_9");

        gridLayout->addWidget(label_9, 2, 0, 1, 1);

        label_10 = new QLabel(layoutWidget_2);
        label_10->setObjectName("label_10");

        gridLayout->addWidget(label_10, 2, 2, 1, 1);

        spinBoxMinOrder = new QSpinBox(layoutWidget_2);
        spinBoxMinOrder->setObjectName("spinBoxMinOrder");

        gridLayout->addWidget(spinBoxMinOrder, 2, 1, 1, 1);

        spinBoxMaxOrder = new QSpinBox(layoutWidget_2);
        spinBoxMaxOrder->setObjectName("spinBoxMaxOrder");

        gridLayout->addWidget(spinBoxMaxOrder, 2, 3, 1, 1);

        widget = new QWidget(TuneDiagramDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(22, 224, 332, 68));
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(widget);
        label_3->setObjectName("label_3");

        gridLayout_2->addWidget(label_3, 0, 0, 1, 1);

        spinBoxQxIntervals = new QSpinBox(widget);
        spinBoxQxIntervals->setObjectName("spinBoxQxIntervals");

        gridLayout_2->addWidget(spinBoxQxIntervals, 0, 1, 1, 1);

        label_4 = new QLabel(widget);
        label_4->setObjectName("label_4");

        gridLayout_2->addWidget(label_4, 0, 2, 1, 1);

        spinBoxQyIntervals = new QSpinBox(widget);
        spinBoxQyIntervals->setObjectName("spinBoxQyIntervals");

        gridLayout_2->addWidget(spinBoxQyIntervals, 0, 3, 1, 1);

        label = new QLabel(widget);
        label->setObjectName("label");

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        doubleSpinBoxQx = new QDoubleSpinBox(widget);
        doubleSpinBoxQx->setObjectName("doubleSpinBoxQx");

        gridLayout_2->addWidget(doubleSpinBoxQx, 1, 1, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");

        gridLayout_2->addWidget(label_2, 1, 2, 1, 1);

        doubleSpinBoxQy = new QDoubleSpinBox(widget);
        doubleSpinBoxQy->setObjectName("doubleSpinBoxQy");

        gridLayout_2->addWidget(doubleSpinBoxQy, 1, 3, 1, 1);


        retranslateUi(TuneDiagramDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TuneDiagramDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TuneDiagramDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TuneDiagramDialog);
    } // setupUi

    void retranslateUi(QDialog *TuneDiagramDialog)
    {
        TuneDiagramDialog->setWindowTitle(QCoreApplication::translate("TuneDiagramDialog", "Tune Diagram Dialog", nullptr));
        checkBoxSumResonances->setText(QCoreApplication::translate("TuneDiagramDialog", "Sum resonances", nullptr));
        checkBoxCouplingResonances->setText(QCoreApplication::translate("TuneDiagramDialog", "Coupling resonances", nullptr));
        checkBoxCaptions->setText(QCoreApplication::translate("TuneDiagramDialog", "Show captions", nullptr));
        label_5->setText(QCoreApplication::translate("TuneDiagramDialog", "Qx min", nullptr));
        label_7->setText(QCoreApplication::translate("TuneDiagramDialog", "Qx max", nullptr));
        label_6->setText(QCoreApplication::translate("TuneDiagramDialog", "Qy min", nullptr));
        label_8->setText(QCoreApplication::translate("TuneDiagramDialog", "Qy max", nullptr));
        label_9->setText(QCoreApplication::translate("TuneDiagramDialog", "Min order ", nullptr));
        label_10->setText(QCoreApplication::translate("TuneDiagramDialog", "Max order ", nullptr));
        label_3->setText(QCoreApplication::translate("TuneDiagramDialog", "Grid intervals:   Along Qx ", nullptr));
        label_4->setText(QCoreApplication::translate("TuneDiagramDialog", "Along Qy", nullptr));
        label->setText(QCoreApplication::translate("TuneDiagramDialog", "Machine tunes     Qx:", nullptr));
        label_2->setText(QCoreApplication::translate("TuneDiagramDialog", "Qy", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TuneDiagramDialog: public Ui_TuneDiagramDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TUNE_DIAGRAM_DIALOG_H
