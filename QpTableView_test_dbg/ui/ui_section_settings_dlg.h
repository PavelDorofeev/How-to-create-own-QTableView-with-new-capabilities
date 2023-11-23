/********************************************************************************
** Form generated from reading UI file 'section_settings_dlg.ui'
**
** Created: Thu 23. Nov 18:40:59 2023
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SECTION_SETTINGS_DLG_H
#define UI_SECTION_SETTINGS_DLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SectionSettingsDlg
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame;
    QGridLayout *gridLayout;
    QCheckBox *chk_ver_top;
    QCheckBox *chk_hor_left;
    QCheckBox *chk_center;
    QCheckBox *chk_hor_right;
    QCheckBox *chk_ver_bottom;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLineEdit *ledt_fntWeight;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *chk_fnt_light;
    QRadioButton *chk_fnt_normal;
    QRadioButton *chk_fnt_bold;
    QRadioButton *chk_fnt_demibold;
    QRadioButton *chk_fnt_black;
    QGroupBox *grbx_color;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *ledt_color;
    QRadioButton *rbtn_orig;
    QRadioButton *rbtn_blue;
    QRadioButton *rbtn_green;
    QRadioButton *rbtn_brown;
    QRadioButton *rbtn_red;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_4;
    QFormLayout *formLayout;
    QLabel *label_2;
    QSpinBox *spb_pointSize;
    QSpinBox *spb_pixelSize;
    QLabel *label_3;
    QRadioButton *rbtn_size_0_5;
    QRadioButton *rbtn_size_0_7;
    QRadioButton *rbtn_size_normal;
    QRadioButton *rbtn_size_1_5;
    QRadioButton *rbtn_size_2_0;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SectionSettingsDlg)
    {
        if (SectionSettingsDlg->objectName().isEmpty())
            SectionSettingsDlg->setObjectName(QString::fromUtf8("SectionSettingsDlg"));
        SectionSettingsDlg->resize(484, 485);
        verticalLayout_2 = new QVBoxLayout(SectionSettingsDlg);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(5, 5, 5, -1);
        frame = new QFrame(SectionSettingsDlg);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(frame);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        chk_ver_top = new QCheckBox(frame);
        chk_ver_top->setObjectName(QString::fromUtf8("chk_ver_top"));

        gridLayout->addWidget(chk_ver_top, 0, 2, 1, 1);

        chk_hor_left = new QCheckBox(frame);
        chk_hor_left->setObjectName(QString::fromUtf8("chk_hor_left"));

        gridLayout->addWidget(chk_hor_left, 1, 1, 1, 1);

        chk_center = new QCheckBox(frame);
        chk_center->setObjectName(QString::fromUtf8("chk_center"));

        gridLayout->addWidget(chk_center, 1, 2, 1, 1);

        chk_hor_right = new QCheckBox(frame);
        chk_hor_right->setObjectName(QString::fromUtf8("chk_hor_right"));

        gridLayout->addWidget(chk_hor_right, 1, 3, 1, 1);

        chk_ver_bottom = new QCheckBox(frame);
        chk_ver_bottom->setObjectName(QString::fromUtf8("chk_ver_bottom"));

        gridLayout->addWidget(chk_ver_bottom, 2, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 4, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);


        verticalLayout_3->addWidget(frame);


        verticalLayout->addLayout(verticalLayout_3);

        groupBox_2 = new QGroupBox(SectionSettingsDlg);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setContentsMargins(15, 15, 15, 15);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, -1);
        ledt_fntWeight = new QLineEdit(groupBox_2);
        ledt_fntWeight->setObjectName(QString::fromUtf8("ledt_fntWeight"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(ledt_fntWeight->sizePolicy().hasHeightForWidth());
        ledt_fntWeight->setSizePolicy(sizePolicy1);
        ledt_fntWeight->setInputMethodHints(Qt::ImhDigitsOnly);
        ledt_fntWeight->setMaxLength(3);

        horizontalLayout->addWidget(ledt_fntWeight);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout_2->addLayout(horizontalLayout, 0, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        chk_fnt_light = new QRadioButton(groupBox_2);
        chk_fnt_light->setObjectName(QString::fromUtf8("chk_fnt_light"));

        horizontalLayout_2->addWidget(chk_fnt_light);

        chk_fnt_normal = new QRadioButton(groupBox_2);
        chk_fnt_normal->setObjectName(QString::fromUtf8("chk_fnt_normal"));

        horizontalLayout_2->addWidget(chk_fnt_normal);

        chk_fnt_bold = new QRadioButton(groupBox_2);
        chk_fnt_bold->setObjectName(QString::fromUtf8("chk_fnt_bold"));

        horizontalLayout_2->addWidget(chk_fnt_bold);

        chk_fnt_demibold = new QRadioButton(groupBox_2);
        chk_fnt_demibold->setObjectName(QString::fromUtf8("chk_fnt_demibold"));

        horizontalLayout_2->addWidget(chk_fnt_demibold);

        chk_fnt_black = new QRadioButton(groupBox_2);
        chk_fnt_black->setObjectName(QString::fromUtf8("chk_fnt_black"));

        horizontalLayout_2->addWidget(chk_fnt_black);


        gridLayout_2->addLayout(horizontalLayout_2, 1, 1, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        grbx_color = new QGroupBox(SectionSettingsDlg);
        grbx_color->setObjectName(QString::fromUtf8("grbx_color"));
        horizontalLayout_3 = new QHBoxLayout(grbx_color);
        horizontalLayout_3->setContentsMargins(15, 15, 15, 15);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        ledt_color = new QLineEdit(grbx_color);
        ledt_color->setObjectName(QString::fromUtf8("ledt_color"));
        sizePolicy1.setHeightForWidth(ledt_color->sizePolicy().hasHeightForWidth());
        ledt_color->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(ledt_color);

        rbtn_orig = new QRadioButton(grbx_color);
        rbtn_orig->setObjectName(QString::fromUtf8("rbtn_orig"));

        horizontalLayout_3->addWidget(rbtn_orig);

        rbtn_blue = new QRadioButton(grbx_color);
        rbtn_blue->setObjectName(QString::fromUtf8("rbtn_blue"));

        horizontalLayout_3->addWidget(rbtn_blue);

        rbtn_green = new QRadioButton(grbx_color);
        rbtn_green->setObjectName(QString::fromUtf8("rbtn_green"));

        horizontalLayout_3->addWidget(rbtn_green);

        rbtn_brown = new QRadioButton(grbx_color);
        rbtn_brown->setObjectName(QString::fromUtf8("rbtn_brown"));

        horizontalLayout_3->addWidget(rbtn_brown);

        rbtn_red = new QRadioButton(grbx_color);
        rbtn_red->setObjectName(QString::fromUtf8("rbtn_red"));

        horizontalLayout_3->addWidget(rbtn_red);


        verticalLayout->addWidget(grbx_color);

        groupBox = new QGroupBox(SectionSettingsDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout_4 = new QHBoxLayout(groupBox);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        formLayout = new QFormLayout();
        formLayout->setContentsMargins(5, 5, 5, 5);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        spb_pointSize = new QSpinBox(groupBox);
        spb_pointSize->setObjectName(QString::fromUtf8("spb_pointSize"));
        spb_pointSize->setMinimum(-1);
        spb_pointSize->setMaximum(64);
        spb_pointSize->setValue(15);

        formLayout->setWidget(0, QFormLayout::FieldRole, spb_pointSize);

        spb_pixelSize = new QSpinBox(groupBox);
        spb_pixelSize->setObjectName(QString::fromUtf8("spb_pixelSize"));
        spb_pixelSize->setMinimum(-1);
        spb_pixelSize->setMaximum(64);
        spb_pixelSize->setValue(15);

        formLayout->setWidget(1, QFormLayout::FieldRole, spb_pixelSize);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);


        horizontalLayout_4->addLayout(formLayout);

        rbtn_size_0_5 = new QRadioButton(groupBox);
        rbtn_size_0_5->setObjectName(QString::fromUtf8("rbtn_size_0_5"));

        horizontalLayout_4->addWidget(rbtn_size_0_5);

        rbtn_size_0_7 = new QRadioButton(groupBox);
        rbtn_size_0_7->setObjectName(QString::fromUtf8("rbtn_size_0_7"));

        horizontalLayout_4->addWidget(rbtn_size_0_7);

        rbtn_size_normal = new QRadioButton(groupBox);
        rbtn_size_normal->setObjectName(QString::fromUtf8("rbtn_size_normal"));

        horizontalLayout_4->addWidget(rbtn_size_normal);

        rbtn_size_1_5 = new QRadioButton(groupBox);
        rbtn_size_1_5->setObjectName(QString::fromUtf8("rbtn_size_1_5"));

        horizontalLayout_4->addWidget(rbtn_size_1_5);

        rbtn_size_2_0 = new QRadioButton(groupBox);
        rbtn_size_2_0->setObjectName(QString::fromUtf8("rbtn_size_2_0"));

        horizontalLayout_4->addWidget(rbtn_size_2_0);


        verticalLayout->addWidget(groupBox);


        verticalLayout_2->addLayout(verticalLayout);

        buttonBox = new QDialogButtonBox(SectionSettingsDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(SectionSettingsDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), SectionSettingsDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SectionSettingsDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(SectionSettingsDlg);
    } // setupUi

    void retranslateUi(QDialog *SectionSettingsDlg)
    {
        SectionSettingsDlg->setWindowTitle(QApplication::translate("SectionSettingsDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        chk_ver_top->setText(QApplication::translate("SectionSettingsDlg", "Top", 0, QApplication::UnicodeUTF8));
        chk_hor_left->setText(QApplication::translate("SectionSettingsDlg", "Left", 0, QApplication::UnicodeUTF8));
        chk_center->setText(QApplication::translate("SectionSettingsDlg", "Center", 0, QApplication::UnicodeUTF8));
        chk_hor_right->setText(QApplication::translate("SectionSettingsDlg", "Right", 0, QApplication::UnicodeUTF8));
        chk_ver_bottom->setText(QApplication::translate("SectionSettingsDlg", "Bottom", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("SectionSettingsDlg", "font : ", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SectionSettingsDlg", "font weight:", 0, QApplication::UnicodeUTF8));
        ledt_fntWeight->setPlaceholderText(QApplication::translate("SectionSettingsDlg", "1..99", 0, QApplication::UnicodeUTF8));
        chk_fnt_light->setText(QApplication::translate("SectionSettingsDlg", "light", 0, QApplication::UnicodeUTF8));
        chk_fnt_normal->setText(QApplication::translate("SectionSettingsDlg", "normal", 0, QApplication::UnicodeUTF8));
        chk_fnt_bold->setText(QApplication::translate("SectionSettingsDlg", "bold", 0, QApplication::UnicodeUTF8));
        chk_fnt_demibold->setText(QApplication::translate("SectionSettingsDlg", "demibold", 0, QApplication::UnicodeUTF8));
        chk_fnt_black->setText(QApplication::translate("SectionSettingsDlg", "black", 0, QApplication::UnicodeUTF8));
        grbx_color->setTitle(QApplication::translate("SectionSettingsDlg", "color : ", 0, QApplication::UnicodeUTF8));
        rbtn_orig->setText(QApplication::translate("SectionSettingsDlg", "orig", 0, QApplication::UnicodeUTF8));
        rbtn_blue->setText(QApplication::translate("SectionSettingsDlg", "blue", 0, QApplication::UnicodeUTF8));
        rbtn_green->setText(QApplication::translate("SectionSettingsDlg", "green", 0, QApplication::UnicodeUTF8));
        rbtn_brown->setText(QApplication::translate("SectionSettingsDlg", "brown", 0, QApplication::UnicodeUTF8));
        rbtn_red->setText(QApplication::translate("SectionSettingsDlg", "red", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("SectionSettingsDlg", "font-size :", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SectionSettingsDlg", "point size : ", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("SectionSettingsDlg", "pixel size : ", 0, QApplication::UnicodeUTF8));
        rbtn_size_0_5->setText(QApplication::translate("SectionSettingsDlg", "x0.5", 0, QApplication::UnicodeUTF8));
        rbtn_size_0_7->setText(QApplication::translate("SectionSettingsDlg", "x0.7", 0, QApplication::UnicodeUTF8));
        rbtn_size_normal->setText(QApplication::translate("SectionSettingsDlg", "normal", 0, QApplication::UnicodeUTF8));
        rbtn_size_1_5->setText(QApplication::translate("SectionSettingsDlg", "x1.5", 0, QApplication::UnicodeUTF8));
        rbtn_size_2_0->setText(QApplication::translate("SectionSettingsDlg", "x2", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SectionSettingsDlg: public Ui_SectionSettingsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SECTION_SETTINGS_DLG_H
