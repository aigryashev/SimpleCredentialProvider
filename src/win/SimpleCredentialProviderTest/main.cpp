#include <QLibrary>
#include <QDebug>

#include <thread>

int main()
{
    //std::this_thread::sleep_for(std::chrono::seconds(30));
    // try to load lib
    std::this_thread::sleep_for(std::chrono::seconds(60));

    QLibrary lib("SimpleCredentialProvider.dll");

    if (lib.load())
        qDebug() << "not loaded: " << lib.errorString();
    else
        qDebug() << "loaded";

	return 0;
}
