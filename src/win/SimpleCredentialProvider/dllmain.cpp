#include <thread>
#include <future>

#include <Shlwapi.h>
#include <processthreadsapi.h>

#include <QCoreApplication>

#include "dllmain.hpp"
#include "logging.hpp"
#include "consts/CommonConsts.hpp"
#include "consts/GUID.hpp"
#include "utils/utils.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)

namespace crm
{
namespace credential_provider
{

HINSTANCE gDllInstance(nullptr);
long gDllRefsCount(0);
std::future<int> gRunApplicationResult;

extern HRESULT SimpleCredentialProvider_CreateInstance(__in REFIID riid, __deref_out void ** ppv);

DWORD WINAPI runApplication(LPVOID)
{
    if (nullptr != QCoreApplication::instance())
        /// There is nothing to do the application has been started before
        return 1;

    int argc(0);
    QCoreApplication app(argc, nullptr);
    std::string libraryDirPath(std::getenv("ALLUSERSPROFILE"));
    libraryDirPath += "\\crm";
    app.addLibraryPath(libraryDirPath.c_str());

    return app.exec();
}

void dllAddRef()
{
    InterlockedIncrement(&gDllRefsCount);
}

void dllRelease()
{
    InterlockedDecrement(&gDllRefsCount);
}

class ClassFactory : public IClassFactory
{
public:
    ClassFactory() : mReferencesCounter(1)
    {
    }

    // IUnknown
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(ClassFactory, IClassFactory),
            {0, 0}
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&mReferencesCounter);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&mReferencesCounter);
        if (!cRef)
            delete this;

        return cRef;
    }

    // IClassFactory
    IFACEMETHODIMP CreateInstance(__in IUnknown * pUnkOuter, __in REFIID riid, __deref_out void ** ppv)
    {
        HRESULT hr;
        if (!pUnkOuter)
        {
            hr = SimpleCredentialProvider_CreateInstance(riid, ppv);
        }
        else
        {
            *ppv = NULL;
            hr = CLASS_E_NOAGGREGATION;
        }

        return hr;
    }

    IFACEMETHODIMP LockServer(__in BOOL bLock)
    {
        if (bLock)
        {
            dllAddRef();
        }
        else
        {
            dllRelease();
        }

        return S_OK;
    }

private:
    virtual ~ClassFactory() = default;

    long mReferencesCounter;
};

HRESULT ClassFactory_CreateInstance(__in REFCLSID rclsid, __in REFIID riid, __deref_out void **ppv)
{
    qDebug() << "ClassFactory_CreateInstance call detected";

    *ppv = NULL;

    HRESULT hr;

    if (consts::gSimpleCredentialProviderGUID == rclsid)
    {
        ClassFactory* pcf = new ClassFactory();
        if (pcf)
        {
            hr = pcf->QueryInterface(riid, ppv);
            pcf->Release();
        }
        else
        {
            qDebug() << "ClassFactory_CreateInstance finishing with out of memory";
            hr = E_OUTOFMEMORY;
        }
    }
    else
    {
        qDebug() << "ClassFactory_CreateInstance finishing with class not available";
        hr = CLASS_E_CLASSNOTAVAILABLE;
    }
    return hr;
}

} // ~namespace credential_provider
} // ~namespace CRM

STDAPI DllCanUnloadNow()
{
    qDebug() << __FUNCTION__ << "current refs count: " << crm::credential_provider::gDllRefsCount;
    // workaround for freezed QCoreApplication
    if (0 == crm::credential_provider::gDllRefsCount)
    {
        QCoreApplication * app(QCoreApplication::instance());
        if (app)
        {
            app->quit();
        }
    }

    return (crm::credential_provider::gDllRefsCount > 0) ? S_FALSE : S_OK;
}

STDAPI DllGetClassObject(__in REFCLSID rclsid, __in REFIID riid, __deref_out void** ppv)
{
    return crm::credential_provider::ClassFactory_CreateInstance(rclsid, riid, ppv);
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hInstance);
        crm::logging::initCRMLogging("SimpleCredentialProvider");

        CreateThread(nullptr, 0,  crm::credential_provider::runApplication,
                     hInstance, 0, nullptr);
        break;
    }

    case DLL_THREAD_ATTACH:
        break;

    case DLL_PROCESS_DETACH:
        break;

    case DLL_THREAD_DETACH:
        break;
    }

    crm::credential_provider::gDllInstance = hInstance;

    return true;
}

#else
static_assert(false, "The file should be used on Windows");
#endif
