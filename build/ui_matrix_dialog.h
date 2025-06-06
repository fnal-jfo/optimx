/********************************************************************************
** Form generated from reading UI file 'matrix_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MATRIX_DIALOG_H
#define UI_MATRIX_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MatrixDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *spinBoxFirst;
    QLabel *label_2;
    QSpinBox *spinBoxLast;
    QCheckBox *checkBoxSymplecticity;

    void setupUi(QDialog *MatrixDialog)
    {
        if (MatrixDialog->objectName().isEmpty())
            MatrixDialog->setObjectName("MatrixDialog");
        MatrixDialog->setEnabled(true);
        MatrixDialog->resize(350, 225);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MatrixDialog->sizePolicy().hasHeightForWidth());
        MatrixDialog->setSizePolicy(sizePolicy);
        MatrixDialog->setMinimumSize(QSize(350, 225));
        MatrixDialog->setMaximumSize(QSize(350, 225));
        MatrixDialog->setBaseSize(QSize(246, 165));
        QFont font;
        font.setPointSize(8);
        MatrixDialog->setFont(font);
        buttonBox = new QDialogButtonBox(MatrixDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(90, 173, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(MatrixDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(40, 30, 291, 68));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        spinBoxFirst = new QSpinBox(layoutWidget);
        spinBoxFirst->setObjectName("spinBoxFirst");
        spinBoxFirst->setMaximum(100000);

        gridLayout->addWidget(spinBoxFirst, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        spinBoxLast = new QSpinBox(layoutWidget);
        spinBoxLast->setObjectName("spinBoxLast");
        spinBoxLast->setMaximum(100000);

        gridLayout->addWidget(spinBoxLast, 1, 1, 1, 1);

        checkBoxSymplecticity = new QCheckBox(MatrixDialog);
        checkBoxSymplecticity->setObjectName("checkBoxSymplecticity");
        checkBoxSymplecticity->setGeometry(QRect(41, 120, 176, 20));

        retranslateUi(MatrixDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, MatrixDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, MatrixDialog, qOverload<>(&QDialog::reject));
        QObject::connect(buttonBox, SIGNAL(accepted()), MatrixDialog, SLOT(updateDialogData()));

        QMetaObject::connectSlotsByName(MatrixDialog);
    } // setupUi

    void retranslateUi(QDialog *MatrixDialog)
    {
        MatrixDialog->setWindowTitle(QCoreApplication::translate("MatrixDialog", "Matrix Dialog", nullptr));
        label->setText(QCoreApplication::translate("MatrixDialog", "Initial Element Index ", nullptr));
        label_2->setText(QCoreApplication::translate("MatrixDialog", "Final Element Index", nullptr));
        checkBoxSymplecticity->setText(QCoreApplication::translate("MatrixDialog", "Include Symplecticity Test ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MatrixDialog: public Ui_MatrixDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MATRIX_DIALOG_H
