#include <Windows.h>
#include <synchapi.h>

#include <QApplication>

#include "ui/GamerUIWidget.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)

namespace
{
   const QString gAppDisplayName(QObject::tr("Club Logon UI"));

   struct SingleApplicationGuard
   {
       SingleApplicationGuard(const QString & applicationName)
           : mApplicationName(applicationName)
           , mMutexHandle(nullptr)
       {
       }

       ~SingleApplicationGuard()
       {
           unlock();
       }

       bool tryLock()
       {
           HANDLE mutexHandle = CreateMutex(nullptr,
                                            true,
                                            mApplicationName.toStdWString().c_str());
           if (NULL == mutexHandle)
               return false;

           mutexHandle = OpenMutex(SYNCHRONIZE,
                                   false,
                                   mApplicationName.toStdWString().c_str());
           if (NULL == mutexHandle)
               return false;

           DWORD result(WaitForSingleObject(mutexHandle, 0));
           switch (result)
           {
           case WAIT_ABANDONED:
           case WAIT_OBJECT_0:
               //Another instance doesn't exist
               break;
           default:
               return false;
           }

           mMutexHandle = mutexHandle;
           return true;
       }

       void unlock()
       {
           if (nullptr != mMutexHandle)
               CloseHandle(mMutexHandle);
       }

   private:
       const QString mApplicationName;
       HANDLE mMutexHandle;
   };
} // ~anonymous namespace

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName(gAppDisplayName);

    SingleApplicationGuard singleApplicationGuard(gAppDisplayName);
    if (!singleApplicationGuard.tryLock())
        //Possible another instance of the app is running. We have to finish the application
        return EXIT_FAILURE;

    GamerUIWidget widget;
    widget.showFullScreen();

    const int res(app.exec());
    return res;
}

#else
static_assert(false, "The file should be used on Windows");
#endif

