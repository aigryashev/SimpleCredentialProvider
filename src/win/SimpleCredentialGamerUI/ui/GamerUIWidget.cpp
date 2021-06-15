#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <crm_dll.h>
#include "Windows.h"
#include "GamerUIWidget.h"
#include "ui_gameruiwidget.h"
#include <credentialprovider.h>

namespace
{
//TODO: remove variable duplication here and in CommonConsts module
const QString gGamerUserName("Gamer");
const QString gJSONResultObjectName("result");
const std::string INFO_TITLE = "Информация";
const std::string ERROR_TITLE = "Ошибка";
const QString INFO_FIELD_EMPTY("Заполните все поля");
const QString INFO_PASS_CONFIRM_FAILED("Подтверждающий пароль не соответствует паролю");
const int IS_OK = 1;
const int IS_EMPTY = 0;
} //~anonymous namespace

GamerUIWidget::GamerUIWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GamerUIWidget)
{
    ui->setupUi(this);
    connect(ui->pushButtonLogon, &QPushButton::clicked, this, &GamerUIWidget::onLogonClicked);
    connect(ui->pushButtonForgotPassword, &QPushButton::clicked, this, &GamerUIWidget::onForgotPasswordClicked);
    connect(ui->pushButtonRegister, &QPushButton::clicked, this, &GamerUIWidget::onRegisterClicked);
    connect(ui->pushButtonLogonByRegister, &QPushButton::clicked, this, &GamerUIWidget::onLogonByRegisterClicked);
    ui->lineEditRegistrationEmail->setHidden(true);
    ui->lineEditRegistrationPhone->setHidden(true);
    ui->comboBoxRegistrationSource->setHidden(true);
    initLanguageLabel();
    int res_get_fields = registerGetFieldsAPI();
    if(res_get_fields != IS_OK)
        QMessageBox::information(this, tr(INFO_TITLE.c_str()), getLastError());
    int res_get_count_fields = registerAPI_getCountFields();
    if(res_get_count_fields == IS_EMPTY)
        QMessageBox::information(this, tr(INFO_TITLE.c_str()), getLastError());
    for(int i=0; i< res_get_count_fields; ++i){
        //QMessageBox::critical(this, tr("Ошибка"), QString("%1-%2-%3").arg(registerAPI_getCodeField(i),registerAPI_getNameField(i),registerAPI_getTypeField(i)));
        std::string code_field = std::string(registerAPI_getCodeField(i));
        if(code_field == "EMAIL")
            ui->lineEditRegistrationEmail->setHidden(false);
        else if(code_field == "PHONE_NUMBER")
            ui->lineEditRegistrationPhone->setHidden(false);
        else if(code_field == "SOURCE_PLACE")
            ui->comboBoxRegistrationSource->setHidden(false);
        if(QString(registerAPI_getTypeField(i)) == "list"){
            int get_count_element = registerAPI_getCountElementList(i);
            ui->comboBoxRegistrationSource->clear();
            for(int j = 0; j < get_count_element; ++j){
                QString message_str = QString("%1").arg(registerAPI_getNameElementList(i, j));
                //QMessageBox::information(this, tr(INFO_TITLE.c_str()), message_str);
                ui->comboBoxRegistrationSource->addItem(message_str);
            }
        }
    }
    activateWindow();
}

GamerUIWidget::~GamerUIWidget()
{
    delete ui;
}

void GamerUIWidget::initLanguageLabel()
{
    return;
    QString label_text = "<html><head/><body><p>";
    QLocale sys_locale;
    QString localename_sys = sys_locale.system().name();
    localename_sys = localename_sys.mid(0, localename_sys.indexOf("_"));
    QStringList all_locales = sys_locale.uiLanguages();
    for(int i=0; i<all_locales.size();++i){
        QString locale_name = all_locales.at(i);
        locale_name = locale_name.mid(0, locale_name.indexOf("-"));
        QMessageBox::critical(this, tr("Ошибка"), localename_sys+"@"+locale_name);
        if(localename_sys == locale_name)
            locale_name = QString("<span style=\"font-weight:600;\">%1</span>").arg(locale_name);
        label_text.append(QString("%1/").arg(locale_name));
    }
    label_text.chop(1);
    label_text.append("</p></body></html>");
    ui->labelRusEng->setText(label_text);
}

void GamerUIWidget::onLogonClicked(bool)
{
    QString login = ui->lineEditLogonLogin->text();
    QString password = ui->lineEditLogonPassword->text();
    if (login.isEmpty() || login == gGamerUserName){
        QMessageBox::critical(this, tr(ERROR_TITLE.c_str()), "Введено некорректное имя пользователя");
        return;
    }
    if (password.isEmpty()){
        QMessageBox::critical(this, tr(ERROR_TITLE.c_str()), "Не введен пароль");
        return;
    }
    int res_login = loginAPI(login.toStdString().c_str(), login.size(),password.toStdString().c_str(), password.size());
    if (res_login != IS_OK){
        QMessageBox::critical(this, tr(ERROR_TITLE.c_str()), getLastError());
        return;
    }
    qDebug("!!!!!!!!!!![%s]",Q_FUNC_INFO);
//    ICredentialProvider a;
//    crm::credential_provider::SimpleCredentialProvider simple_credential_provider;
    qDebug("!!!!!!!!!!![%s]",Q_FUNC_INFO);
    /// TODO: find out the more secure way to send to dll the credential data (boost::interprocess for example)
    /// The application exit code 0 says that the login & password are correct. We have to save
    /// it to the file
    if (!saveCRMUserName(login)){
        QMessageBox::critical(this, tr("Ошибка"), "Возникла внутренняя ошибка сервиса. Обратитесь к администратору клуба");
        return;
    }
    allowUserToLogin();
}

void GamerUIWidget::onForgotPasswordClicked(bool)
{
    QInputDialog input_dialog(ui->tabWidgetMain);
    input_dialog.setGeometry(ui->tabWidgetMain->geometry());
    input_dialog.setWindowTitle(tr("Восстановление пароля"));
    QStringList labeltexts_list;
    labeltexts_list << tr("Введите логин:") << tr("Введите код из сообщения:") << tr("Введите новый пароль:");
    QList<QLineEdit::EchoMode> echomodes_list;
    echomodes_list << QLineEdit::Normal << QLineEdit::Password << QLineEdit::Password;
    QStringList errors_list;
    errors_list << "Не задан логин" << "Не задан код из сообщения" << "Не задан новый пароль";
    enum functions_list{RESTORE=0,CHECK_CODE=1,SET_PASS=2};
    for(int i=0; i<labeltexts_list.size();++i){
        input_dialog.setLabelText(labeltexts_list.at(i));
        input_dialog.setTextEchoMode(echomodes_list.at(i));
        input_dialog.setTextValue("");
        int is_accepted = input_dialog.exec();
        if(!is_accepted)
            return;
        QString return_value = input_dialog.textValue();
        if(return_value.isEmpty()){
            QMessageBox::critical(this, tr(ERROR_TITLE.c_str()), errors_list.at(i));
            return;
        }
        int restore_res = IS_EMPTY;
        if(i == RESTORE)
            restore_res = restoreAPI(return_value.toStdString().c_str(), return_value.size());
        else if(i == CHECK_CODE)
            restore_res = restoreCheckCodeAPI(return_value.toStdString().c_str(), return_value.size());
        else if(i == SET_PASS)
            restore_res = restoreSetPasswordAPI(return_value.toStdString().c_str(), return_value.size());
        if(restore_res != IS_OK){
            QMessageBox::critical(this, tr(ERROR_TITLE.c_str()), getLastError());
            return;
        }
    }
    QMessageBox::information(this, tr(INFO_TITLE.c_str()), "Смена пароля прошла успешно");
}

void GamerUIWidget::onRegisterClicked(bool)
{
    QList<QLineEdit*> lineedit_list;
    lineedit_list << ui->lineEditRegistrationEmail << ui->lineEditRegistrationLogin
                  << ui->lineEditRegistrationPassword << ui->lineEditRegistrationPasswordConfirm
                  << ui->lineEditRegistrationPhone;
    for(int i=0; i<lineedit_list.size();++i)
        if(lineedit_list.at(i)->text().isEmpty() && lineedit_list.at(i)->isVisible()){
            QMessageBox::information(this, tr(INFO_TITLE.c_str()), INFO_FIELD_EMPTY);
            return;
        }
    if(ui->comboBoxRegistrationSource->currentText().isEmpty() && ui->comboBoxRegistrationSource->isVisible()){
        QMessageBox::information(this, tr(INFO_TITLE.c_str()), INFO_FIELD_EMPTY);
        return;
    }
    if(ui->lineEditRegistrationPassword->text() != ui->lineEditRegistrationPasswordConfirm->text()){
        QMessageBox::information(this, tr(INFO_TITLE.c_str()), INFO_PASS_CONFIRM_FAILED);
        return;
    }
    for(int i = 0; i < registerAPI_getCountFields(); ++i){
        QString value;
        QString code_field = registerAPI_getCodeField(i);
        if(code_field == "LOGIN")
            value = ui->lineEditRegistrationLogin->text();
        else if(code_field == "EMAIL")
            value = ui->lineEditRegistrationEmail->text();
        else if(code_field == "PHONE_NUMBER")
            value = ui->lineEditRegistrationPhone->text();
        else if(code_field == "PASSWORD")
            value = ui->lineEditRegistrationPassword->text();
        else if(code_field == "CONFIRM_PASSWORD")
            value = ui->lineEditRegistrationPasswordConfirm->text();
        else if(code_field == "SOURCE_PLACE")
            value = ui->comboBoxRegistrationSource->currentText();
        else
            break;
        bool is_data_send = registerAPI_addDataSend(code_field.toStdString().c_str(), code_field.size(),
                                                    value.toStdString().c_str(), value.size());
        if(!is_data_send){
            QString error_msg = QString("Ошибка посылки данных [%1] на сервер").arg(code_field.toStdString().c_str());
            QMessageBox::critical(this, tr(ERROR_TITLE.c_str()), error_msg);
            return;
        }
    }
    if(registerAPI() != IS_OK){
        QMessageBox::critical(this, tr(ERROR_TITLE.c_str()), getLastError());
        return;
    }
    for(int i=0; i<lineedit_list.size();++i)
        lineedit_list.at(i)->clear();
    QMessageBox::information(this, tr(INFO_TITLE.c_str()), "Регистрация прошла успешно");
}

void GamerUIWidget::onLogonByRegisterClicked(bool)
{
    //TODO: implement me
    allowUserToLogin();
}

void GamerUIWidget::allowUserToLogin()
{
    QApplication::quit();
}

bool GamerUIWidget::saveCRMUserName(const QString & userName)
{
    //TODO: debug it with CRM user
    static const QString filePath(QString("%1/crm/username.txt").arg(std::getenv("ALLUSERSPROFILE")));
    QFile f(filePath);
    if (!f.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&f);
    out.setCodec("UTF-8");
    out << userName;
    out.flush();
    return true;
}

///NOTE: the method is to enable border-image painting
void GamerUIWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOption styleOption;
    styleOption.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
}

///NOTE: the method is to disable close by pressing Alt+F4 etc. By default if the login/register and
/// other routines was finished correctly the app should quit with return code 0 that tells the
/// credential provider to login with built-in Gamer user
void GamerUIWidget::closeEvent(QCloseEvent * event)
{
    if (!event)
        return;

    event->ignore();
}

void GamerUIWidget::switchLanguage()
{
    qDebug(ui->labelRusEng->text().toStdString().c_str());
    //int lang = LOWORD(GetKeyboardLayout(0));
    wchar_t buffer[KL_NAMELENGTH+1];
    memset(buffer,0,sizeof(buffer));
    GetKeyboardLayoutName(buffer);
    QMessageBox::information(this, tr("Ошибка"), QString::fromWCharArray(buffer));
}

void GamerUIWidget::mousePressEvent(QMouseEvent *event)
{
     if(childAt(event->pos().x(),event->pos().y()) == ui->labelRusEng)
         switchLanguage();
}

void GamerUIWidget::changeEvent(QEvent *event)
{
    qDebug(QString::number(event->type()).toStdString().c_str());
    if(event->type() == QEvent::KeyboardLayoutChange)
        switchLanguage();
}
