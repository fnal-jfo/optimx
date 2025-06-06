/********************************************************************************
** Form generated from reading UI file 'editor_preferences_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITOR_PREFERENCES_DIALOG_H
#define UI_EDITOR_PREFERENCES_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EditorPreferencesDialog
{
public:
    QGroupBox *groupBox;
    QPushButton *pushButtonSelectFont;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QSpinBox *spinBoxSize;
    QLineEdit *lineEditFont;
    QLabel *label_7;
    QLabel *label_5;
    QLineEdit *lineEditStyle;
    QLineEdit *lineEditWeight;
    QLabel *label_6;
    QLabel *label_8;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout;
    QSpinBox *spinBoxBackB;
    QPushButton *pushButtonForeRGB;
    QSpinBox *spinBoxTextEditorTabs;
    QLabel *label_2;
    QLabel *label_4;
    QSpinBox *spinBoxBackR;
    QPushButton *pushButtonBackRGB;
    QSpinBox *spinBoxBackG;
    QSpinBox *spinBoxForeB;
    QSpinBox *spinBoxForeG;
    QLabel *label_3;
    QLabel *label;
    QSpinBox *spinBoxLatticeEditorTabs;
    QSpinBox *spinBoxForeR;
    QSpinBox *spinBoxCursor;
    QLabel *label_9;
    QCheckBox *checkBoxShowLines;
    QCheckBox *checkBoxCurLine;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *EditorPreferencesDialog)
    {
        if (EditorPreferencesDialog->objectName().isEmpty())
            EditorPreferencesDialog->setObjectName("EditorPreferencesDialog");
        EditorPreferencesDialog->resize(530, 479);
        groupBox = new QGroupBox(EditorPreferencesDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 10, 451, 131));
        pushButtonSelectFont = new QPushButton(groupBox);
        pushButtonSelectFont->setObjectName("pushButtonSelectFont");
        pushButtonSelectFont->setGeometry(QRect(200, 90, 51, 22));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 30, 431, 54));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        spinBoxSize = new QSpinBox(layoutWidget);
        spinBoxSize->setObjectName("spinBoxSize");
        spinBoxSize->setEnabled(true);
        spinBoxSize->setReadOnly(true);
        spinBoxSize->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBoxSize->setMaximum(256);

        gridLayout_2->addWidget(spinBoxSize, 6, 4, 1, 1);

        lineEditFont = new QLineEdit(layoutWidget);
        lineEditFont->setObjectName("lineEditFont");
        lineEditFont->setEnabled(true);
        lineEditFont->setReadOnly(true);

        gridLayout_2->addWidget(lineEditFont, 6, 0, 1, 1);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName("label_7");

        gridLayout_2->addWidget(label_7, 3, 4, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        gridLayout_2->addWidget(label_5, 3, 0, 1, 1);

        lineEditStyle = new QLineEdit(layoutWidget);
        lineEditStyle->setObjectName("lineEditStyle");
        lineEditStyle->setEnabled(true);
        lineEditStyle->setReadOnly(true);

        gridLayout_2->addWidget(lineEditStyle, 6, 3, 1, 1);

        lineEditWeight = new QLineEdit(layoutWidget);
        lineEditWeight->setObjectName("lineEditWeight");

        gridLayout_2->addWidget(lineEditWeight, 6, 2, 1, 1);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        gridLayout_2->addWidget(label_6, 3, 3, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName("label_8");

        gridLayout_2->addWidget(label_8, 3, 2, 1, 1);

        layoutWidget1 = new QWidget(EditorPreferencesDialog);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(10, 157, 492, 232));
        gridLayout = new QGridLayout(layoutWidget1);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        spinBoxBackB = new QSpinBox(layoutWidget1);
        spinBoxBackB->setObjectName("spinBoxBackB");
        spinBoxBackB->setMaximum(255);

        gridLayout->addWidget(spinBoxBackB, 3, 3, 1, 1);

        pushButtonForeRGB = new QPushButton(layoutWidget1);
        pushButtonForeRGB->setObjectName("pushButtonForeRGB");

        gridLayout->addWidget(pushButtonForeRGB, 2, 4, 1, 1);

        spinBoxTextEditorTabs = new QSpinBox(layoutWidget1);
        spinBoxTextEditorTabs->setObjectName("spinBoxTextEditorTabs");

        gridLayout->addWidget(spinBoxTextEditorTabs, 1, 1, 1, 1);

        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_4 = new QLabel(layoutWidget1);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        spinBoxBackR = new QSpinBox(layoutWidget1);
        spinBoxBackR->setObjectName("spinBoxBackR");
        spinBoxBackR->setMaximum(255);

        gridLayout->addWidget(spinBoxBackR, 3, 1, 1, 1);

        pushButtonBackRGB = new QPushButton(layoutWidget1);
        pushButtonBackRGB->setObjectName("pushButtonBackRGB");

        gridLayout->addWidget(pushButtonBackRGB, 3, 4, 1, 1);

        spinBoxBackG = new QSpinBox(layoutWidget1);
        spinBoxBackG->setObjectName("spinBoxBackG");
        spinBoxBackG->setMaximum(255);

        gridLayout->addWidget(spinBoxBackG, 3, 2, 1, 1);

        spinBoxForeB = new QSpinBox(layoutWidget1);
        spinBoxForeB->setObjectName("spinBoxForeB");
        spinBoxForeB->setMaximum(255);

        gridLayout->addWidget(spinBoxForeB, 2, 3, 1, 1);

        spinBoxForeG = new QSpinBox(layoutWidget1);
        spinBoxForeG->setObjectName("spinBoxForeG");
        spinBoxForeG->setMaximum(255);

        gridLayout->addWidget(spinBoxForeG, 2, 2, 1, 1);

        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label = new QLabel(layoutWidget1);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        spinBoxLatticeEditorTabs = new QSpinBox(layoutWidget1);
        spinBoxLatticeEditorTabs->setObjectName("spinBoxLatticeEditorTabs");

        gridLayout->addWidget(spinBoxLatticeEditorTabs, 0, 1, 1, 1);

        spinBoxForeR = new QSpinBox(layoutWidget1);
        spinBoxForeR->setObjectName("spinBoxForeR");
        spinBoxForeR->setMaximum(255);

        gridLayout->addWidget(spinBoxForeR, 2, 1, 1, 1);

        spinBoxCursor = new QSpinBox(layoutWidget1);
        spinBoxCursor->setObjectName("spinBoxCursor");
        spinBoxCursor->setMinimum(1);
        spinBoxCursor->setMaximum(12);

        gridLayout->addWidget(spinBoxCursor, 4, 1, 1, 1);

        label_9 = new QLabel(layoutWidget1);
        label_9->setObjectName("label_9");

        gridLayout->addWidget(label_9, 4, 0, 1, 1);

        checkBoxShowLines = new QCheckBox(layoutWidget1);
        checkBoxShowLines->setObjectName("checkBoxShowLines");

        gridLayout->addWidget(checkBoxShowLines, 5, 0, 1, 1);

        checkBoxCurLine = new QCheckBox(layoutWidget1);
        checkBoxCurLine->setObjectName("checkBoxCurLine");

        gridLayout->addWidget(checkBoxCurLine, 6, 0, 1, 1);

        buttonBox = new QDialogButtonBox(EditorPreferencesDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(144, 418, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(EditorPreferencesDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, EditorPreferencesDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, EditorPreferencesDialog, qOverload<>(&QDialog::reject));
        QObject::connect(pushButtonForeRGB, SIGNAL(clicked()), EditorPreferencesDialog, SLOT(selectForegroundHighlightColor()));
        QObject::connect(pushButtonBackRGB, SIGNAL(clicked()), EditorPreferencesDialog, SLOT(selectBackgroundHighlightColor()));

        QMetaObject::connectSlotsByName(EditorPreferencesDialog);
    } // setupUi

    void retranslateUi(QDialog *EditorPreferencesDialog)
    {
        EditorPreferencesDialog->setWindowTitle(QCoreApplication::translate("EditorPreferencesDialog", "Editor Preferences", nullptr));
        groupBox->setTitle(QCoreApplication::translate("EditorPreferencesDialog", "Editor Font", nullptr));
        pushButtonSelectFont->setText(QCoreApplication::translate("EditorPreferencesDialog", ".Select..", nullptr));
        label_7->setText(QCoreApplication::translate("EditorPreferencesDialog", "Size", nullptr));
        label_5->setText(QCoreApplication::translate("EditorPreferencesDialog", "Font", nullptr));
        label_6->setText(QCoreApplication::translate("EditorPreferencesDialog", "Style", nullptr));
        label_8->setText(QCoreApplication::translate("EditorPreferencesDialog", "Weight", nullptr));
        pushButtonForeRGB->setText(QCoreApplication::translate("EditorPreferencesDialog", "...", nullptr));
        label_2->setText(QCoreApplication::translate("EditorPreferencesDialog", "Tab Spacing for Text Output", nullptr));
        label_4->setText(QCoreApplication::translate("EditorPreferencesDialog", "Background Highlight Color  [R G B] ", nullptr));
        pushButtonBackRGB->setText(QCoreApplication::translate("EditorPreferencesDialog", "...", nullptr));
        label_3->setText(QCoreApplication::translate("EditorPreferencesDialog", "Foreground Highlight Color [ R G B ]", nullptr));
        label->setText(QCoreApplication::translate("EditorPreferencesDialog", "Tab Spacing for Lattice Editor", nullptr));
        label_9->setText(QCoreApplication::translate("EditorPreferencesDialog", "Cursor Width  [pxl]", nullptr));
        checkBoxShowLines->setText(QCoreApplication::translate("EditorPreferencesDialog", "Show Line Numbers", nullptr));
        checkBoxCurLine->setText(QCoreApplication::translate("EditorPreferencesDialog", "Highlight Current Line", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EditorPreferencesDialog: public Ui_EditorPreferencesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITOR_PREFERENCES_DIALOG_H
