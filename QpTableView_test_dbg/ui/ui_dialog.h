/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created: Thu 23. Nov 18:40:59 2023
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QPushButton *btn_sqltableModel_On;
    QPushButton *btn_QStandardItemModel_On;
    QComboBox *cmb_SelectionMode;
    QComboBox *cmb_SelectionBehavoir;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QPlainTextEdit *txt1;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btn_init_sections;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_4;
    QPlainTextEdit *txt2;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *btn_init_sections_2;
    QSpacerItem *horizontalSpacer_5;
    QVBoxLayout *verticalLayout_5;
    QPlainTextEdit *txt3;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *btn_init_sections_3;
    QSpacerItem *horizontalSpacer_7;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *chk_grid_on;
    QCheckBox *chk_betweenRowsBorder;
    QPushButton *btn_logFile;
    QHBoxLayout *for_tv;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(1034, 789);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setContentsMargins(5, 5, 5, 5);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        btn_sqltableModel_On = new QPushButton(Dialog);
        btn_sqltableModel_On->setObjectName(QString::fromUtf8("btn_sqltableModel_On"));

        gridLayout->addWidget(btn_sqltableModel_On, 0, 0, 1, 1);

        btn_QStandardItemModel_On = new QPushButton(Dialog);
        btn_QStandardItemModel_On->setObjectName(QString::fromUtf8("btn_QStandardItemModel_On"));

        gridLayout->addWidget(btn_QStandardItemModel_On, 0, 1, 1, 1);

        cmb_SelectionMode = new QComboBox(Dialog);
        cmb_SelectionMode->setObjectName(QString::fromUtf8("cmb_SelectionMode"));
        cmb_SelectionMode->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(cmb_SelectionMode, 0, 4, 1, 1);

        cmb_SelectionBehavoir = new QComboBox(Dialog);
        cmb_SelectionBehavoir->setObjectName(QString::fromUtf8("cmb_SelectionBehavoir"));
        cmb_SelectionBehavoir->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(cmb_SelectionBehavoir, 0, 5, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setContentsMargins(5, 5, 5, 5);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 30);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(5, 5, 5, -1);
        txt1 = new QPlainTextEdit(Dialog);
        txt1->setObjectName(QString::fromUtf8("txt1"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(txt1->sizePolicy().hasHeightForWidth());
        txt1->setSizePolicy(sizePolicy);
        txt1->setMinimumSize(QSize(0, 0));
        txt1->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout_3->addWidget(txt1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        btn_init_sections = new QPushButton(Dialog);
        btn_init_sections->setObjectName(QString::fromUtf8("btn_init_sections"));

        horizontalLayout_3->addWidget(btn_init_sections);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout_3->addLayout(horizontalLayout_3);


        horizontalLayout_2->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(5, 5, 5, -1);
        txt2 = new QPlainTextEdit(Dialog);
        txt2->setObjectName(QString::fromUtf8("txt2"));
        sizePolicy.setHeightForWidth(txt2->sizePolicy().hasHeightForWidth());
        txt2->setSizePolicy(sizePolicy);
        txt2->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout_4->addWidget(txt2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        btn_init_sections_2 = new QPushButton(Dialog);
        btn_init_sections_2->setObjectName(QString::fromUtf8("btn_init_sections_2"));

        horizontalLayout_4->addWidget(btn_init_sections_2);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);


        verticalLayout_4->addLayout(horizontalLayout_4);


        horizontalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(5, 5, 5, -1);
        txt3 = new QPlainTextEdit(Dialog);
        txt3->setObjectName(QString::fromUtf8("txt3"));
        sizePolicy.setHeightForWidth(txt3->sizePolicy().hasHeightForWidth());
        txt3->setSizePolicy(sizePolicy);
        txt3->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout_5->addWidget(txt3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(5, 5, 5, -1);
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);

        btn_init_sections_3 = new QPushButton(Dialog);
        btn_init_sections_3->setObjectName(QString::fromUtf8("btn_init_sections_3"));

        horizontalLayout_5->addWidget(btn_init_sections_3);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_7);


        verticalLayout_5->addLayout(horizontalLayout_5);


        horizontalLayout_2->addLayout(verticalLayout_5);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setContentsMargins(5, 5, 5, 5);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        chk_grid_on = new QCheckBox(Dialog);
        chk_grid_on->setObjectName(QString::fromUtf8("chk_grid_on"));
        chk_grid_on->setChecked(true);

        verticalLayout_6->addWidget(chk_grid_on);

        chk_betweenRowsBorder = new QCheckBox(Dialog);
        chk_betweenRowsBorder->setObjectName(QString::fromUtf8("chk_betweenRowsBorder"));

        verticalLayout_6->addWidget(chk_betweenRowsBorder);

        btn_logFile = new QPushButton(Dialog);
        btn_logFile->setObjectName(QString::fromUtf8("btn_logFile"));

        verticalLayout_6->addWidget(btn_logFile);


        horizontalLayout_2->addLayout(verticalLayout_6);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 1);
        horizontalLayout_2->setStretch(2, 1);

        verticalLayout_2->addLayout(horizontalLayout_2);

        for_tv = new QHBoxLayout();
        for_tv->setContentsMargins(5, 5, 5, 5);
        for_tv->setObjectName(QString::fromUtf8("for_tv"));

        verticalLayout_2->addLayout(for_tv);

        verticalLayout_2->setStretch(0, 1);
        verticalLayout_2->setStretch(1, 10);

        verticalLayout->addLayout(verticalLayout_2);


        retranslateUi(Dialog);

        cmb_SelectionMode->setCurrentIndex(1);
        cmb_SelectionBehavoir->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        btn_sqltableModel_On->setText(QApplication::translate("Dialog", "QSqlTableModel", 0, QApplication::UnicodeUTF8));
        btn_QStandardItemModel_On->setText(QApplication::translate("Dialog", "QStandardItemModel", 0, QApplication::UnicodeUTF8));
        cmb_SelectionMode->clear();
        cmb_SelectionMode->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "SelectItems", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "SelectRows", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "SelectColumns", 0, QApplication::UnicodeUTF8)
        );
        cmb_SelectionBehavoir->clear();
        cmb_SelectionBehavoir->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "NoSelection", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "SingleSelection", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "MultiSelection", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "ExtendedSelection", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "ContiguousSelection", 0, QApplication::UnicodeUTF8)
        );
        txt1->setPlainText(QApplication::translate("Dialog", "0,1,\320\235\320\220\320\233:>,2,3,5,6,8,8,9,11,11\n"
"0,1,\320\221\320\235:>,  4,3,5,6,7,7,10,10,10", 0, QApplication::UnicodeUTF8));
        btn_init_sections->setText(QApplication::translate("Dialog", "init", 0, QApplication::UnicodeUTF8));
        txt2->setPlainText(QApplication::translate("Dialog", "1,\320\235\320\220\320\233:,2,2,\320\264\320\260\321\202\320\260,6,8,11\n"
"5,\320\221\320\235:,4,3,\320\262\321\200\320\265\320\274\321\217,7,9,10", 0, QApplication::UnicodeUTF8));
        btn_init_sections_2->setText(QApplication::translate("Dialog", "init", 0, QApplication::UnicodeUTF8));
        txt3->setPlainText(QApplication::translate("Dialog", "0,2,2,5,3\n"
"0,4,4,4,3\n"
"0,6,7,9,9", 0, QApplication::UnicodeUTF8));
        btn_init_sections_3->setText(QApplication::translate("Dialog", "init", 0, QApplication::UnicodeUTF8));
        chk_grid_on->setText(QApplication::translate("Dialog", "show grid", 0, QApplication::UnicodeUTF8));
        chk_betweenRowsBorder->setText(QApplication::translate("Dialog", "show between rows border", 0, QApplication::UnicodeUTF8));
        btn_logFile->setText(QApplication::translate("Dialog", "log file", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
