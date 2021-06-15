/********************************************************************************
** Form generated from reading UI file 'GamerUIWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GAMERUIWIDGET_H
#define UI_GAMERUIWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GamerUIWidget
{
public:
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout_5;
    QSpacerItem *horizontalSpacer_5;
    QVBoxLayout *verticalLayout_6;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelWelcome;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QLabel *labelClubName;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_7;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidgetMain;
    QWidget *tabLogon;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *lineEditLogonLogin;
    QLineEdit *lineEditLogonPassword;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButtonLogon;
    QPushButton *pushButtonForgotPassword;
    QWidget *tabRegistration;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *lineEditRegistrationLogin;
    QLineEdit *lineEditRegistrationPhone;
    QLineEdit *lineEditRegistrationPassword;
    QLineEdit *lineEditRegistrationPasswordConfirm;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButtonRegister;
    QPushButton *pushButtonLogonByRegister;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_8;
    QSpacerItem *verticalSpacer_5;
    QVBoxLayout *verticalLayout_4;
    QLabel *labelRusEng;
    QSpacerItem *verticalSpacer_4;
    QSpacerItem *horizontalSpacer_6;

    void setupUi(QWidget *GamerUIWidget)
    {
        if (GamerUIWidget->objectName().isEmpty())
            GamerUIWidget->setObjectName(QString::fromUtf8("GamerUIWidget"));
        GamerUIWidget->resize(953, 666);
        GamerUIWidget->setStyleSheet(QString::fromUtf8("QWidget#GamerUIWidget\n"
"{\n"
"	border-image: url(:/ui/logon_background.bmp) 0 0 0 0 stretch stretch;\n"
"}\n"
"\n"
"QTabWidget\n"
"{\n"
"background-color: white;\n"
"}\n"
"\n"
"QTabWidget#TabWidgetMain\n"
"{\n"
"min-width: 400;\n"
"max-width: 400;\n"
"}\n"
"\n"
"QTabWidget::tab-bar\n"
"{\n"
"alignment: center;\n"
"}\n"
"\n"
"QTabBar::tab\n"
"{\n"
"background: transparent;\n"
"width: 199;\n"
"font: bold;\n"
"color: white;\n"
"}\n"
"\n"
"QTabBar::tab:selected\n"
"{\n"
"background: rgb(154, 40, 214)\n"
"}\n"
"\n"
"QLabel\n"
"{\n"
"color: white;\n"
"}\n"
"\n"
"QLineEdit\n"
"{\n"
"background-color: white;\n"
"border-color: black\n"
"}\n"
"\n"
"QPushButton\n"
"{\n"
"background-color: white;\n"
"border-color: black;\n"
"color: rgb(154, 40, 214);\n"
"font: bold;\n"
"}\n"
"\n"
"QPushButton::default\n"
"{\n"
"background-color: rgb(154, 40, 214);\n"
"border-color: black;\n"
"color: white\n"
"}\n"
"\n"
"QMessageBox\n"
"{\n"
"background-color: white;\n"
"border-color: black;\n"
"color: rgb(154, 40, 214);\n"
"font: bold;\n"
""
                        "}\n"
"\n"
"QMessageBox QLabel\n"
"{\n"
"color: black;\n"
"}\n"
""));
        horizontalLayout_6 = new QHBoxLayout(GamerUIWidget);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalSpacer_5 = new QSpacerItem(98, 643, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_5->addItem(horizontalSpacer_5);


        horizontalLayout_6->addLayout(verticalLayout_5);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        labelWelcome = new QLabel(GamerUIWidget);
        labelWelcome->setObjectName(QString::fromUtf8("labelWelcome"));

        horizontalLayout->addWidget(labelWelcome);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_6->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        labelClubName = new QLabel(GamerUIWidget);
        labelClubName->setObjectName(QString::fromUtf8("labelClubName"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        labelClubName->setFont(font);

        horizontalLayout_2->addWidget(labelClubName);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout_6->addLayout(horizontalLayout_2);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        verticalLayout_6->addItem(verticalSpacer_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_7);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidgetMain = new QTabWidget(GamerUIWidget);
        tabWidgetMain->setObjectName(QString::fromUtf8("tabWidgetMain"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidgetMain->sizePolicy().hasHeightForWidth());
        tabWidgetMain->setSizePolicy(sizePolicy);
        tabWidgetMain->setMinimumSize(QSize(400, 0));
        tabWidgetMain->setMaximumSize(QSize(400, 16777215));
        tabWidgetMain->setStyleSheet(QString::fromUtf8(""));
        tabWidgetMain->setTabPosition(QTabWidget::North);
        tabWidgetMain->setTabShape(QTabWidget::Rounded);
        tabWidgetMain->setDocumentMode(false);
        tabLogon = new QWidget();
        tabLogon->setObjectName(QString::fromUtf8("tabLogon"));
        verticalLayout_2 = new QVBoxLayout(tabLogon);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        lineEditLogonLogin = new QLineEdit(tabLogon);
        lineEditLogonLogin->setObjectName(QString::fromUtf8("lineEditLogonLogin"));

        verticalLayout_2->addWidget(lineEditLogonLogin);

        lineEditLogonPassword = new QLineEdit(tabLogon);
        lineEditLogonPassword->setObjectName(QString::fromUtf8("lineEditLogonPassword"));
        lineEditLogonPassword->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(lineEditLogonPassword);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButtonLogon = new QPushButton(tabLogon);
        pushButtonLogon->setObjectName(QString::fromUtf8("pushButtonLogon"));

        horizontalLayout_3->addWidget(pushButtonLogon);

        pushButtonForgotPassword = new QPushButton(tabLogon);
        pushButtonForgotPassword->setObjectName(QString::fromUtf8("pushButtonForgotPassword"));

        horizontalLayout_3->addWidget(pushButtonForgotPassword);


        verticalLayout_2->addLayout(horizontalLayout_3);

        tabWidgetMain->addTab(tabLogon, QString());
        tabRegistration = new QWidget();
        tabRegistration->setObjectName(QString::fromUtf8("tabRegistration"));
        verticalLayout_3 = new QVBoxLayout(tabRegistration);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        lineEditRegistrationLogin = new QLineEdit(tabRegistration);
        lineEditRegistrationLogin->setObjectName(QString::fromUtf8("lineEditRegistrationLogin"));
        lineEditRegistrationLogin->setStyleSheet(QString::fromUtf8(""));

        verticalLayout_3->addWidget(lineEditRegistrationLogin);

        lineEditRegistrationPhone = new QLineEdit(tabRegistration);
        lineEditRegistrationPhone->setObjectName(QString::fromUtf8("lineEditRegistrationPhone"));

        verticalLayout_3->addWidget(lineEditRegistrationPhone);

        lineEditRegistrationPassword = new QLineEdit(tabRegistration);
        lineEditRegistrationPassword->setObjectName(QString::fromUtf8("lineEditRegistrationPassword"));
        lineEditRegistrationPassword->setEchoMode(QLineEdit::Password);

        verticalLayout_3->addWidget(lineEditRegistrationPassword);

        lineEditRegistrationPasswordConfirm = new QLineEdit(tabRegistration);
        lineEditRegistrationPasswordConfirm->setObjectName(QString::fromUtf8("lineEditRegistrationPasswordConfirm"));
        lineEditRegistrationPasswordConfirm->setEchoMode(QLineEdit::Password);

        verticalLayout_3->addWidget(lineEditRegistrationPasswordConfirm);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        pushButtonRegister = new QPushButton(tabRegistration);
        pushButtonRegister->setObjectName(QString::fromUtf8("pushButtonRegister"));

        horizontalLayout_5->addWidget(pushButtonRegister);

        pushButtonLogonByRegister = new QPushButton(tabRegistration);
        pushButtonLogonByRegister->setObjectName(QString::fromUtf8("pushButtonLogonByRegister"));

        horizontalLayout_5->addWidget(pushButtonLogonByRegister);


        verticalLayout_3->addLayout(horizontalLayout_5);

        tabWidgetMain->addTab(tabRegistration, QString());

        verticalLayout->addWidget(tabWidgetMain);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_4->addLayout(verticalLayout);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_8);


        verticalLayout_6->addLayout(horizontalLayout_4);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_5);


        horizontalLayout_6->addLayout(verticalLayout_6);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        labelRusEng = new QLabel(GamerUIWidget);
        labelRusEng->setObjectName(QString::fromUtf8("labelRusEng"));
        labelRusEng->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(labelRusEng);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_4);

        horizontalSpacer_6 = new QSpacerItem(178, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_4->addItem(horizontalSpacer_6);


        horizontalLayout_6->addLayout(verticalLayout_4);


        retranslateUi(GamerUIWidget);

        tabWidgetMain->setCurrentIndex(0);
        pushButtonLogon->setDefault(true);
        pushButtonRegister->setDefault(true);


        QMetaObject::connectSlotsByName(GamerUIWidget);
    } // setupUi

    void retranslateUi(QWidget *GamerUIWidget)
    {
        GamerUIWidget->setWindowTitle(QString());
        labelWelcome->setText(QCoreApplication::translate("GamerUIWidget", "\320\224\320\276\320\261\321\200\320\276 \320\277\320\276\320\266\320\260\320\273\320\276\320\262\320\260\321\202\321\214 \320\262 \320\275\320\260\321\210 \320\272\320\273\321\203\320\261", nullptr));
        labelClubName->setText(QCoreApplication::translate("GamerUIWidget", "\320\235\320\260\320\267\320\262\320\260\320\275\320\270\320\265 \320\272\320\273\321\203\320\261\320\260", nullptr));
        lineEditLogonLogin->setInputMask(QString());
        lineEditLogonLogin->setText(QString());
        lineEditLogonLogin->setPlaceholderText(QCoreApplication::translate("GamerUIWidget", "\320\233\320\276\320\263\320\270\320\275", nullptr));
        lineEditLogonPassword->setPlaceholderText(QCoreApplication::translate("GamerUIWidget", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        pushButtonLogon->setText(QCoreApplication::translate("GamerUIWidget", "\320\222\320\236\320\231\320\242\320\230", nullptr));
        pushButtonForgotPassword->setText(QCoreApplication::translate("GamerUIWidget", "\320\227\320\220\320\221\320\253\320\233\320\230 \320\237\320\220\320\240\320\236\320\233\320\254?", nullptr));
        tabWidgetMain->setTabText(tabWidgetMain->indexOf(tabLogon), QCoreApplication::translate("GamerUIWidget", "\320\222\321\205\320\276\320\264 \320\277\320\276 \320\273\320\276\320\263\320\270\320\275\321\203", nullptr));
        lineEditRegistrationLogin->setPlaceholderText(QCoreApplication::translate("GamerUIWidget", "\320\233\320\276\320\263\320\270\320\275", nullptr));
        lineEditRegistrationPhone->setPlaceholderText(QCoreApplication::translate("GamerUIWidget", "\320\242\320\265\320\273\320\265\321\204\320\276\320\275", nullptr));
        lineEditRegistrationPassword->setPlaceholderText(QCoreApplication::translate("GamerUIWidget", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        lineEditRegistrationPasswordConfirm->setPlaceholderText(QCoreApplication::translate("GamerUIWidget", "\320\237\320\276\320\264\321\202\320\262\320\265\321\200\320\264\320\270\321\202\320\265 \320\277\320\260\321\200\320\276\320\273\321\214", nullptr));
        pushButtonRegister->setText(QCoreApplication::translate("GamerUIWidget", "\320\227\320\220\320\240\320\225\320\223\320\230\320\241\320\242\320\240\320\230\320\240\320\236\320\222\320\220\320\242\320\254\320\241\320\257", nullptr));
        pushButtonLogonByRegister->setText(QCoreApplication::translate("GamerUIWidget", "\320\222\320\236\320\231\320\242\320\230", nullptr));
        tabWidgetMain->setTabText(tabWidgetMain->indexOf(tabRegistration), QCoreApplication::translate("GamerUIWidget", "\320\240\320\265\320\263\320\270\321\201\321\202\321\200\320\260\321\206\320\270\321\217", nullptr));
        labelRusEng->setText(QCoreApplication::translate("GamerUIWidget", "Rus/Eng", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GamerUIWidget: public Ui_GamerUIWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMERUIWIDGET_H
