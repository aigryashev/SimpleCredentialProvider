Логирование идет в Z:/log_CRM_DLL.txt
Если Z:/ не существует то C:/

ОСНОВНЫЕ ФУНКЦИИ: *API
ДОПОЛНИТЕЛЬНЫЕ ФУНКЦИИ: *API_*
ФУНЦИИ URL:
{
   CRM_DLL_EXPORT void setUrlCRM(const char *url, int url_len); - задать Url 
   CRM_DLL_EXPORT const char *getUrlCRM(); - считать текущий URL (по умолчанию URL установлен)

   пример:
   std::cout << getUrlCRM() << "\n";
   std::string url = "test";
   setUrlCRM(url.c_str(), url.size());
   std::cout << getUrlCRM() << "\n";
}

Если какая-та из основных функций завершилась с ошибкой (res != 1), причину можно узнать,
вызвав функцию const char *getLastError().

Результаты функций *API:
 1 - ok
-1 - ошибка входных данных
-2 - ошибка сети (нет связи ссерверо или сервер не ответил)
-3 - сервер не принял данные (getLastError() - записана причина отказа). 

ОСНОВНЫЕ ФУНКЦИИ:

Вход 
loginAPI: (В функцию необходимо передать логин, пароль и время ожидания ответа в милисекундах.)
Пример:
int res = loginAPI(login.c_str(), login.size(), password.c_str(), password.size());
td::cout << "loginApi: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));

Регистрация
registerGetFieldsAPI: (Запрос на получение полей)
registerAPI: (Запрос на регистрацию пользователя: в функцию необходимо передать время ожидания ответа в милисекундах.)
Пример:
int res = registerAPI(login.c_str(), login.size(), password.c_str(), password.size(), phone.c_str(), phone.size());
td::cout << registerAPI" << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));

Восстановление пароля
restoreAPI: (Запрос на восстановление: в функцию необходимо передать логин, в ответе приходит SessionId, который запоминается внутри)
restoreCheckCodeAPI: (Запрос на проверку пароля для восстановления: в функцию передается код введеный пользователем (получен из смс/почты). На сервер передается SessionId полученный в предыдущей функции)
restoreSetPasswordAPI: (Запрос на задание нового пароля: в функцию передается новый пароль введенный пользователем. На сервер передается SessionId)
Выполняется в 3 этапа, используются три функции, логично использовать три функции подряд
Пример:
std::string login = "login";
    res = restoreAPI(login.c_str(), login.size());
    std::cout << "restoreAPI: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));
    if(res == 1)
    {
        //2 - передаем Код (из смс или почты)
        std::string code = "1234";
        res = restoreCheckCodeAPI(code.c_str(), code.size());
        std::cout << "restoreCheckCodeAPI: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));
        if(res == 1)
        {
            //3 - передаем новый пароль
            std::string password = "newPass";
            res = restoreSetPasswordAPI(password.c_str(), password.size());
            std::cout << "restoreSetPasswordAPI: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));
        }
    }

Взять SessionId
В отличаи от других ОСНОВНЫХ функций, возвращает const char *
Если вернула 0 -> значит ошибка, информацию об ошибке можно узнать из const char *getLastError().
Пример:
login = "NewLog1";
    const char *sessionId = getSessionId(login.c_str(), login.size());
    if(sessionId)//не равно 0, значит не ошибка
        std::cout << "getSessionId: " << sessionId;
    else//ошибка!!!
        std::cout << "getSessionId: error - " << getLastError();

ДОПОЛНИТЕЛЬЫНЕ ФУНКЦИИ

Функции для регистрации
!!!НЕОБХОДИМО ИСПОЛЬЗОВАТЬ ПОСЛЕ registerGetFieldsAPI, НО ПЕРЕД registerAPI: 

int registerAPI_getCountFields() - взять количество полученных полей                                 
const char *registerAPI_getCodeField(int index) - взять CODE поля по индексу поля
const char *registerAPI_getNameField(int index) - взять NAME поля по индексу поля
const char *registerAPI_getTypeField(int index) - взять TYPE поля по индексу поля (text/list)
int registerAPI_getCountElementList(int index) - взять количество элементов выбора, для TYPE=list по индексу поля
const char *registerAPI_getKeyElementList(int indexField, int indexChoice) - взять KEY элемента выбора для TYPE=list по индексу поля и элемента 
const char *registerAPI_getNameElementList(int indexField, int indexChoice) - взять NAME элемента выбора для TYPE=list по индексу поля и элемента 
void registerAPI_clearDataSend() - очистить вектор данных для регистрации на срм                               
bool registerAPI_addDataSend(const char *name, int len_name, const char *value, int len_value) - добвить параметр для регистрации на срм

Пример использования функций:
#include <stdio.h>
#include <iostream>

int main(int argc, char *argv[])
{
/* вход в систему */
    std::string login = "NewLog2";
    std::string password = "newPass";
    int res = loginAPI(login.c_str(), login.size(), password.c_str(), password.size());
    std::cout << "loginApi: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));

/* запрос на поля данных */
    res = registerGetFieldsAPI();
    std::cout << "registerAPI_getFields: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));

/* отображаем все поля считанные с сервера */
    //количество полей
    int countFields = registerAPI_getCountFields();
    std::cout << "\tCount Fields: " << countFields << "\n\n";

    for(int i = 0; i < countFields; ++i)
    {
        std::cout << "\t--- index = " << i << "\n";
        //Код поля
        std::cout << "\tCODE = " << registerAPI_getCodeField(i) << "\n";
        //Имя
        std::cout << "\tNAME = " << registerAPI_getNameField(i) << "\n";
        //Тип
        std::cout << "\tType = " << registerAPI_getTypeField(i) << "\n";

        //если тип list
        if(std::string(registerAPI_getTypeField(i)) == "list")
        {
            //количество элементов в листе
            int countElementsList = registerAPI_getCountElementList(i);
            std::cout << "\tCount Element in List = " << countElementsList << ":\n";
            for(int j = 0; j < countElementsList; ++j)
            {
                std::cout << "\t\tKey = " << registerAPI_getKeyElementList(i, j) << "; " << "Name = " << registerAPI_getNameElementList(i, j) << ":\n";
            }
        }
    }

/* заполняем данные для отправки на сервер */
    for(int i = 0; i < registerAPI_getCountFields(); ++i)
    {
        //если логин
        if(std::string(registerAPI_getCodeField(i)) == "LOGIN")
        {
            std::string login = "newUser3";
            registerAPI_addDataSend(registerAPI_getCodeField(i), std::string(registerAPI_getCodeField(i)).size(), login.c_str(), login.size());
        }
        //если номер телефона
        else if(std::string(registerAPI_getCodeField(i)) == "PHONE_NUMBER")
        {
            std::string phone = "+79138884455";
            registerAPI_addDataSend(registerAPI_getCodeField(i), std::string(registerAPI_getCodeField(i)).size(), phone.c_str(), phone.size());
        }
        //если пароль
        else if(std::string(registerAPI_getCodeField(i)) == "PASSWORD")
        {
            std::string password = "password";
            registerAPI_addDataSend(registerAPI_getCodeField(i), std::string(registerAPI_getCodeField(i)).size(), password.c_str(), password.size());
        }
        //если подтверждение пароля
        else if(std::string(registerAPI_getCodeField(i)) == "CONFIRM_PASSWORD")
        {
            std::string password = "password";
            registerAPI_addDataSend(registerAPI_getCodeField(i), std::string(registerAPI_getCodeField(i)).size(), password.c_str(), password.size());
        }
        //если Откуда узнали
        else if(std::string(registerAPI_getCodeField(i)) == "SOURCE_PLACE")
        {
            //НЕОБХОДИМО ПЕРЕДАТЬ КЛЮЧ!!!'
            registerAPI_addDataSend(registerAPI_getCodeField(i), std::string(registerAPI_getCodeField(i)).size(), registerAPI_getKeyElementList(i, 0), std::string(registerAPI_getKeyElementList(i, 0)).size());
        }
    }

/* регистрация */
    res = registerAPI();
    std::cout << "registerAPI: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));

/* востановление пароля */
    //1 - указываем Логин
    login = "NewLog2";
    res = restoreAPI(login.c_str(), login.size());
    std::cout << "restoreAPI: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));
    if(res == 1)
    {
        //2 - передаем Код (из смс или почты!!!)
        std::string code = "1234";
        res = restoreCheckCodeAPI(code.c_str(), code.size());
        std::cout << "restoreCheckCodeAPI: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));
        if(res == 1)
        {
            //3 - передаем новый пароль
            password = "newPass";
            res = restoreSetPasswordAPI(password.c_str(), password.size());
            std::cout << "restoreSetPasswordAPI: " << res << (res == 1 ? "\n" : (std::string(" - ") + getLastError() + "\n"));
        }
    }

/* взять SessionId */
    login = "NewLog1";
    const char *sessionId = getSessionId(login.c_str(), login.size());
    if(sessionId)//не равно 0, значит не ошибка
        std::cout << "getSessionId: " << sessionId;
    else//ошибка!!!
        std::cout << "getSessionId: error - " << getLastError();

    return 0;
}
