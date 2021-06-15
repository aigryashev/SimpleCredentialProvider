#ifndef CRM_DLL_H
#define CRM_DLL_H

#include "CRM_DLL_global.h"

#include <QtCore/qobject.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtCore/qeventloop.h>

CRM_DLL_EXPORT void setUrlCRM(const char *url, int url_len);
CRM_DLL_EXPORT const char *getUrlCRM();

CRM_DLL_EXPORT int loginAPI(const char *login, int login_len,
                            const char *password, int password_len,
                            int timeWaitAnswer_ms = 30000);

/* Блок дополнительных функций для регистрации пользователя */
CRM_DLL_EXPORT int registerAPI_getCountFields();
CRM_DLL_EXPORT const char *registerAPI_getCodeField(int index);
CRM_DLL_EXPORT const char *registerAPI_getNameField(int index);
CRM_DLL_EXPORT const char *registerAPI_getTypeField(int index);
CRM_DLL_EXPORT int registerAPI_getCountElementList(int index);
CRM_DLL_EXPORT const char *registerAPI_getKeyElementList(int indexField, int indexChoice);
CRM_DLL_EXPORT const char *registerAPI_getNameElementList(int indexField, int indexChoice);
CRM_DLL_EXPORT void registerAPI_clearDataSend();
CRM_DLL_EXPORT bool registerAPI_addDataSend(const char *name, int len_name, const char *value, int len_value);

CRM_DLL_EXPORT int registerGetFieldsAPI(int timeWaitAnswer_ms = 30000);

CRM_DLL_EXPORT int registerAPI(int timeWaitAnswer_ms = 30000);

CRM_DLL_EXPORT int restoreAPI(const char *login, int login_len,
                              int timeWaitAnswer_ms = 30000);

CRM_DLL_EXPORT int restoreCheckCodeAPI(const char *code, int code_len,
                                       int timeWaitAnswer_ms = 30000);

CRM_DLL_EXPORT int restoreSetPasswordAPI(const char *password, int password_len,
                                         int timeWaitAnswer_ms = 30000);

CRM_DLL_EXPORT const char *getLastError();

CRM_DLL_EXPORT const char *getSessionId(const char *login, int login_len,
                                        int timeWaitAnswer_ms = 30000);

class GetDataFromURL : public QObject
{
    Q_OBJECT
public:
    explicit GetDataFromURL(int timeWaitAnswer, QObject *parent = nullptr);
    ~GetDataFromURL();

    bool getDataFromURL(QString url, QByteArray *byteArray);

    static void writeLog(const QString &log);
    static QString qByteArrayToQString(const QByteArray &bA);

protected:
    void timerEvent(QTimerEvent* e) override;

private:
    QEventLoop *mEventLoop = nullptr;
    int mTimeWaitAnswer = 30000;
    int id_timer = -1;
    QByteArray *arr = nullptr;
    QNetworkAccessManager *mManager = nullptr;

private slots:
    void slot_mManager_finished(QNetworkReply *reply);
};

#endif // CRM_DLL_H
