/********************************************************************************
** Form generated from reading UI file 'analysisinfochoose.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ANALYSISINFOCHOOSE_H
#define UI_ANALYSISINFOCHOOSE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_analysisInfoChoose
{
public:
    QFrame *frame;
    QLabel *label;
    QPushButton *btn_sure;
    QPushButton *btn_cancel;
    QWidget *layoutWidget_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QComboBox *comboBox4;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QComboBox *comboBox1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QComboBox *comboBox2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QComboBox *comboBox3;

    void setupUi(QWidget *analysisInfoChoose)
    {
        if (analysisInfoChoose->objectName().isEmpty())
            analysisInfoChoose->setObjectName(QStringLiteral("analysisInfoChoose"));
        analysisInfoChoose->resize(690, 208);
        analysisInfoChoose->setStyleSheet(QStringLiteral(""));
        frame = new QFrame(analysisInfoChoose);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(-3, -3, 696, 215));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(196, 36, 401, 21));
        QFont font;
        font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font.setPointSize(19);
        label->setFont(font);
        label->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));
        btn_sure = new QPushButton(frame);
        btn_sure->setObjectName(QStringLiteral("btn_sure"));
        btn_sure->setGeometry(QRect(460, 150, 90, 31));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font1.setPointSize(12);
        btn_sure->setFont(font1);
        btn_sure->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
"border-image: url(:/SCRail/importData/ImageFolder/button/j2.png);"));
        btn_cancel = new QPushButton(frame);
        btn_cancel->setObjectName(QStringLiteral("btn_cancel"));
        btn_cancel->setGeometry(QRect(560, 150, 90, 31));
        btn_cancel->setFont(font1);
        btn_cancel->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
"border-image: url(:/SCRail/importData/ImageFolder/button/j2.png); "));
        layoutWidget_3 = new QWidget(frame);
        layoutWidget_3->setObjectName(QStringLiteral("layoutWidget_3"));
        layoutWidget_3->setGeometry(QRect(12, 150, 242, 27));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget_3);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(layoutWidget_3);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMinimumSize(QSize(104, 0));
        label_5->setMaximumSize(QSize(104, 16777215));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font2.setPointSize(16);
        label_5->setFont(font2);
        label_5->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_5->addWidget(label_5);

        comboBox4 = new QComboBox(layoutWidget_3);
        comboBox4->setObjectName(QStringLiteral("comboBox4"));
        comboBox4->setMinimumSize(QSize(130, 25));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font3.setPointSize(14);
        comboBox4->setFont(font3);
        comboBox4->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_5->addWidget(comboBox4);

        layoutWidget = new QWidget(frame);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 90, 671, 29));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font2);
        label_4->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_3->addWidget(label_4);

        comboBox1 = new QComboBox(layoutWidget);
        comboBox1->setObjectName(QStringLiteral("comboBox1"));
        comboBox1->setFont(font3);
        comboBox1->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_3->addWidget(comboBox1);


        horizontalLayout_4->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font2);
        label_3->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_2->addWidget(label_3);

        comboBox2 = new QComboBox(layoutWidget);
        comboBox2->setObjectName(QStringLiteral("comboBox2"));
        comboBox2->setFont(font3);
        comboBox2->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_2->addWidget(comboBox2);


        horizontalLayout_4->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font2);
        label_2->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(label_2);

        comboBox3 = new QComboBox(layoutWidget);
        comboBox3->setObjectName(QStringLiteral("comboBox3"));
        comboBox3->setFont(font3);
        comboBox3->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(comboBox3);


        horizontalLayout_4->addLayout(horizontalLayout);


        retranslateUi(analysisInfoChoose);

        QMetaObject::connectSlotsByName(analysisInfoChoose);
    } // setupUi

    void retranslateUi(QWidget *analysisInfoChoose)
    {
        analysisInfoChoose->setWindowTitle(QApplication::translate("analysisInfoChoose", "analysisInfoChoose", Q_NULLPTR));
        label->setText(QApplication::translate("analysisInfoChoose", "\350\257\267\351\200\211\346\213\251\345\210\206\346\236\220\346\212\245\350\241\250\347\232\204\345\273\223\345\275\242\344\277\241\346\201\257", Q_NULLPTR));
        btn_sure->setText(QApplication::translate("analysisInfoChoose", "\347\241\256   \345\256\232", Q_NULLPTR));
        btn_cancel->setText(QApplication::translate("analysisInfoChoose", "\345\217\226    \346\266\210", Q_NULLPTR));
        label_5->setText(QApplication::translate("analysisInfoChoose", "\346\243\200\346\265\213\351\207\214\347\250\213\357\274\232", Q_NULLPTR));
        label_4->setText(QApplication::translate("analysisInfoChoose", "\347\272\277    \345\220\215\357\274\232", Q_NULLPTR));
        label_3->setText(QApplication::translate("analysisInfoChoose", "\350\241\214\345\210\253/\350\275\246\347\253\231\357\274\232", Q_NULLPTR));
        label_2->setText(QApplication::translate("analysisInfoChoose", "\345\267\246/\345\217\263\350\275\250\357\274\232", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class analysisInfoChoose: public Ui_analysisInfoChoose {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ANALYSISINFOCHOOSE_H
