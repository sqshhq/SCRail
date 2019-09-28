/********************************************************************************
** Form generated from reading UI file 'printinfo.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRINTINFO_H
#define UI_PRINTINFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PrintInfo
{
public:
    QPushButton *Btn_close;
    QPushButton *btn_printInfo;
    QFrame *frame;
    QWidget *printAxis;
    QLabel *Lab_image;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QTableWidget *tableWidget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_13;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *lineName;
    QLabel *Lab_lineName;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lab_type;
    QLabel *Lab_typeName;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QLabel *Lab_track;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_13;
    QLabel *Lab_mileage;
    QHBoxLayout *horizontalLayout_9;
    QLabel *lab_type_2;
    QLabel *Lab_oversize;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_7;
    QLabel *Lab_verticalHalfWear;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_8;
    QLabel *Lab_verticalThirdsWear;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_15;
    QLabel *Lab_horizontalWear;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_11;
    QLabel *lab_totalWear;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_12;
    QLabel *Lab_GQIValue;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_14;
    QLabel *Lab_maxOffset;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_16;
    QLabel *Lab_minOffset;
    QLabel *label;
    QLabel *Lab_image2;
    QLabel *Lab_image3;
    QPushButton *btn_saveWith;

    void setupUi(QDialog *PrintInfo)
    {
        if (PrintInfo->objectName().isEmpty())
            PrintInfo->setObjectName(QStringLiteral("PrintInfo"));
        PrintInfo->resize(931, 698);
        PrintInfo->setStyleSheet(QStringLiteral("background-color: transparent;"));
        Btn_close = new QPushButton(PrintInfo);
        Btn_close->setObjectName(QStringLiteral("Btn_close"));
        Btn_close->setGeometry(QRect(710, 646, 100, 40));
        QFont font;
        font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font.setPointSize(12);
        Btn_close->setFont(font);
        Btn_close->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
"border-image: url(:/SCRail/importData/ImageFolder/button/j2.png);"));
        btn_printInfo = new QPushButton(PrintInfo);
        btn_printInfo->setObjectName(QStringLiteral("btn_printInfo"));
        btn_printInfo->setGeometry(QRect(460, 646, 100, 40));
        btn_printInfo->setFont(font);
        btn_printInfo->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
"border-image: url(:/SCRail/importData/ImageFolder/button/j2.png);"));
        frame = new QFrame(PrintInfo);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 931, 691));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        printAxis = new QWidget(frame);
        printAxis->setObjectName(QStringLiteral("printAxis"));
        printAxis->setGeometry(QRect(20, 22, 891, 621));
        Lab_image = new QLabel(printAxis);
        Lab_image->setObjectName(QStringLiteral("Lab_image"));
        Lab_image->setGeometry(QRect(10, 30, 414, 265));
        Lab_image->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 0);"));
        label_2 = new QLabel(printAxis);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(140, 317, 161, 16));
        label_2->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));
        label_3 = new QLabel(printAxis);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(570, 230, 181, 20));
        label_3->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));
        label_4 = new QLabel(printAxis);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(624, 430, 141, 16));
        label_4->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));
        tableWidget = new QTableWidget(printAxis);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setGeometry(QRect(440, 450, 441, 161));
        layoutWidget = new QWidget(printAxis);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(440, 11, 453, 193));
        horizontalLayout_13 = new QHBoxLayout(layoutWidget);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lineName = new QLabel(layoutWidget);
        lineName->setObjectName(QStringLiteral("lineName"));
        lineName->setMinimumSize(QSize(110, 0));
        lineName->setMaximumSize(QSize(110, 16777215));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font1.setPointSize(14);
        lineName->setFont(font1);
        lineName->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(lineName);

        Lab_lineName = new QLabel(layoutWidget);
        Lab_lineName->setObjectName(QStringLiteral("Lab_lineName"));
        Lab_lineName->setMinimumSize(QSize(90, 0));
        Lab_lineName->setMaximumSize(QSize(90, 16777215));
        Lab_lineName->setFont(font1);
        Lab_lineName->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(Lab_lineName);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        lab_type = new QLabel(layoutWidget);
        lab_type->setObjectName(QStringLiteral("lab_type"));
        lab_type->setMinimumSize(QSize(110, 0));
        lab_type->setMaximumSize(QSize(110, 16777215));
        lab_type->setFont(font1);
        lab_type->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_2->addWidget(lab_type);

        Lab_typeName = new QLabel(layoutWidget);
        Lab_typeName->setObjectName(QStringLiteral("Lab_typeName"));
        Lab_typeName->setMinimumSize(QSize(90, 0));
        Lab_typeName->setMaximumSize(QSize(90, 16777215));
        Lab_typeName->setFont(font1);
        Lab_typeName->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_2->addWidget(Lab_typeName);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMinimumSize(QSize(110, 0));
        label_5->setMaximumSize(QSize(110, 16777215));
        label_5->setFont(font1);
        label_5->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_3->addWidget(label_5);

        Lab_track = new QLabel(layoutWidget);
        Lab_track->setObjectName(QStringLiteral("Lab_track"));
        Lab_track->setMinimumSize(QSize(90, 0));
        Lab_track->setMaximumSize(QSize(90, 16777215));
        Lab_track->setFont(font1);
        Lab_track->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_3->addWidget(Lab_track);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_13 = new QLabel(layoutWidget);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setMinimumSize(QSize(110, 0));
        label_13->setMaximumSize(QSize(110, 16777215));
        label_13->setFont(font1);
        label_13->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_4->addWidget(label_13);

        Lab_mileage = new QLabel(layoutWidget);
        Lab_mileage->setObjectName(QStringLiteral("Lab_mileage"));
        Lab_mileage->setMinimumSize(QSize(90, 0));
        Lab_mileage->setMaximumSize(QSize(90, 16777215));
        Lab_mileage->setFont(font1);
        Lab_mileage->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_4->addWidget(Lab_mileage);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        lab_type_2 = new QLabel(layoutWidget);
        lab_type_2->setObjectName(QStringLiteral("lab_type_2"));
        lab_type_2->setMinimumSize(QSize(110, 0));
        lab_type_2->setMaximumSize(QSize(110, 16777215));
        lab_type_2->setFont(font1);
        lab_type_2->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_9->addWidget(lab_type_2);

        Lab_oversize = new QLabel(layoutWidget);
        Lab_oversize->setObjectName(QStringLiteral("Lab_oversize"));
        Lab_oversize->setMinimumSize(QSize(90, 0));
        Lab_oversize->setMaximumSize(QSize(90, 16777215));
        Lab_oversize->setFont(font1);
        Lab_oversize->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_9->addWidget(Lab_oversize);


        verticalLayout_2->addLayout(horizontalLayout_9);


        horizontalLayout_13->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setMinimumSize(QSize(110, 0));
        label_7->setMaximumSize(QSize(110, 16777215));
        label_7->setFont(font1);
        label_7->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_7->addWidget(label_7);

        Lab_verticalHalfWear = new QLabel(layoutWidget);
        Lab_verticalHalfWear->setObjectName(QStringLiteral("Lab_verticalHalfWear"));
        Lab_verticalHalfWear->setMinimumSize(QSize(90, 0));
        Lab_verticalHalfWear->setMaximumSize(QSize(90, 16777215));
        Lab_verticalHalfWear->setFont(font1);
        Lab_verticalHalfWear->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_7->addWidget(Lab_verticalHalfWear);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setMinimumSize(QSize(110, 0));
        label_8->setMaximumSize(QSize(110, 16777215));
        label_8->setFont(font1);
        label_8->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_8->addWidget(label_8);

        Lab_verticalThirdsWear = new QLabel(layoutWidget);
        Lab_verticalThirdsWear->setObjectName(QStringLiteral("Lab_verticalThirdsWear"));
        Lab_verticalThirdsWear->setMinimumSize(QSize(90, 0));
        Lab_verticalThirdsWear->setMaximumSize(QSize(90, 16777215));
        Lab_verticalThirdsWear->setFont(font1);
        Lab_verticalThirdsWear->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_8->addWidget(Lab_verticalThirdsWear);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_15 = new QLabel(layoutWidget);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setMinimumSize(QSize(110, 0));
        label_15->setMaximumSize(QSize(110, 16777215));
        label_15->setFont(font1);
        label_15->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_5->addWidget(label_15);

        Lab_horizontalWear = new QLabel(layoutWidget);
        Lab_horizontalWear->setObjectName(QStringLiteral("Lab_horizontalWear"));
        Lab_horizontalWear->setMinimumSize(QSize(90, 0));
        Lab_horizontalWear->setMaximumSize(QSize(90, 16777215));
        Lab_horizontalWear->setFont(font1);
        Lab_horizontalWear->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_5->addWidget(Lab_horizontalWear);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setMinimumSize(QSize(110, 0));
        label_11->setMaximumSize(QSize(110, 16777215));
        label_11->setFont(font1);
        label_11->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_6->addWidget(label_11);

        lab_totalWear = new QLabel(layoutWidget);
        lab_totalWear->setObjectName(QStringLiteral("lab_totalWear"));
        lab_totalWear->setMinimumSize(QSize(90, 0));
        lab_totalWear->setMaximumSize(QSize(90, 16777215));
        lab_totalWear->setFont(font1);
        lab_totalWear->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_6->addWidget(lab_totalWear);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        label_12 = new QLabel(layoutWidget);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setMinimumSize(QSize(110, 0));
        label_12->setMaximumSize(QSize(110, 16777215));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font2.setPointSize(16);
        label_12->setFont(font2);
        label_12->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_10->addWidget(label_12);

        Lab_GQIValue = new QLabel(layoutWidget);
        Lab_GQIValue->setObjectName(QStringLiteral("Lab_GQIValue"));
        Lab_GQIValue->setMinimumSize(QSize(105, 0));
        Lab_GQIValue->setMaximumSize(QSize(90, 16777215));
        Lab_GQIValue->setFont(font1);
        Lab_GQIValue->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_10->addWidget(Lab_GQIValue);


        verticalLayout->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label_14 = new QLabel(layoutWidget);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setMinimumSize(QSize(120, 0));
        label_14->setMaximumSize(QSize(110, 16777215));
        label_14->setFont(font2);
        label_14->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_11->addWidget(label_14);

        Lab_maxOffset = new QLabel(layoutWidget);
        Lab_maxOffset->setObjectName(QStringLiteral("Lab_maxOffset"));
        Lab_maxOffset->setMinimumSize(QSize(105, 0));
        Lab_maxOffset->setMaximumSize(QSize(90, 16777215));
        Lab_maxOffset->setFont(font1);
        Lab_maxOffset->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_11->addWidget(Lab_maxOffset);


        verticalLayout->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        label_16 = new QLabel(layoutWidget);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setMinimumSize(QSize(120, 0));
        label_16->setMaximumSize(QSize(110, 16777215));
        label_16->setFont(font2);
        label_16->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_12->addWidget(label_16);

        Lab_minOffset = new QLabel(layoutWidget);
        Lab_minOffset->setObjectName(QStringLiteral("Lab_minOffset"));
        Lab_minOffset->setMinimumSize(QSize(105, 0));
        Lab_minOffset->setMaximumSize(QSize(90, 16777215));
        Lab_minOffset->setFont(font1);
        Lab_minOffset->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_12->addWidget(Lab_minOffset);


        verticalLayout->addLayout(horizontalLayout_12);


        horizontalLayout_13->addLayout(verticalLayout);

        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(180, 30, 141, 16));
        label->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));
        Lab_image2 = new QLabel(PrintInfo);
        Lab_image2->setObjectName(QStringLiteral("Lab_image2"));
        Lab_image2->setGeometry(QRect(30, 369, 414, 265));
        Lab_image2->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 0);"));
        Lab_image3 = new QLabel(PrintInfo);
        Lab_image3->setObjectName(QStringLiteral("Lab_image3"));
        Lab_image3->setGeometry(QRect(460, 275, 441, 171));
        Lab_image3->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 0);"));
        btn_saveWith = new QPushButton(PrintInfo);
        btn_saveWith->setObjectName(QStringLiteral("btn_saveWith"));
        btn_saveWith->setGeometry(QRect(585, 646, 100, 40));
        btn_saveWith->setFont(font);
        btn_saveWith->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
"border-image: url(:/SCRail/importData/ImageFolder/button/j2.png);"));
        frame->raise();
        Btn_close->raise();
        btn_printInfo->raise();
        Lab_image2->raise();
        Lab_image3->raise();
        btn_saveWith->raise();

        retranslateUi(PrintInfo);

        QMetaObject::connectSlotsByName(PrintInfo);
    } // setupUi

    void retranslateUi(QDialog *PrintInfo)
    {
        PrintInfo->setWindowTitle(QApplication::translate("PrintInfo", "Dialog", Q_NULLPTR));
        Btn_close->setText(QApplication::translate("PrintInfo", "\351\200\200   \345\207\272", Q_NULLPTR));
        btn_printInfo->setText(QApplication::translate("PrintInfo", "\344\277\235   \345\255\230", Q_NULLPTR));
        Lab_image->setText(QString());
        label_2->setText(QString());
        label_3->setText(QString());
        label_4->setText(QString());
        lineName->setText(QApplication::translate("PrintInfo", " \347\272\277 \350\267\257 \345\220\215\357\274\232", Q_NULLPTR));
        Lab_lineName->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        lab_type->setText(QApplication::translate("PrintInfo", " \350\241\214    \345\210\253\357\274\232", Q_NULLPTR));
        Lab_typeName->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        label_5->setText(QApplication::translate("PrintInfo", " \350\202\241    \351\201\223\357\274\232", Q_NULLPTR));
        Lab_track->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        label_13->setText(QApplication::translate("PrintInfo", " \346\243\200\346\265\213\351\207\214\347\250\213\357\274\232K", Q_NULLPTR));
        Lab_mileage->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        lab_type_2->setText(QApplication::translate("PrintInfo", " \347\233\256\346\240\207\345\273\223\345\275\242\357\274\232", Q_NULLPTR));
        Lab_oversize->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        label_7->setText(QApplication::translate("PrintInfo", "\345\236\202\347\243\250 (1/2)\357\274\232", Q_NULLPTR));
        Lab_verticalHalfWear->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        label_8->setText(QApplication::translate("PrintInfo", "\345\236\202\347\243\250 (1/3)\357\274\232", Q_NULLPTR));
        Lab_verticalThirdsWear->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        label_15->setText(QApplication::translate("PrintInfo", "\344\276\247      \347\243\250\357\274\232", Q_NULLPTR));
        Lab_horizontalWear->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        label_11->setText(QApplication::translate("PrintInfo", "\346\200\273  \347\243\250  \350\200\227\357\274\232", Q_NULLPTR));
        lab_totalWear->setText(QApplication::translate("PrintInfo", "TextLabel", Q_NULLPTR));
        label_12->setText(QApplication::translate("PrintInfo", "GQI    \345\200\274\357\274\232", Q_NULLPTR));
        Lab_GQIValue->setText(QApplication::translate("PrintInfo", " TextLabel", Q_NULLPTR));
        label_14->setText(QApplication::translate("PrintInfo", "\346\234\200\345\244\247\345\201\217\345\267\256\345\200\274\357\274\232", Q_NULLPTR));
        Lab_maxOffset->setText(QApplication::translate("PrintInfo", " TextLabel", Q_NULLPTR));
        label_16->setText(QApplication::translate("PrintInfo", "\346\234\200\345\260\217\345\201\217\345\267\256\345\200\274\357\274\232", Q_NULLPTR));
        Lab_minOffset->setText(QApplication::translate("PrintInfo", " TextLabel", Q_NULLPTR));
        label->setText(QString());
        Lab_image2->setText(QString());
        Lab_image3->setText(QString());
        btn_saveWith->setText(QApplication::translate("PrintInfo", "\345\217\246 \345\255\230 \344\270\272", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PrintInfo: public Ui_PrintInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRINTINFO_H
