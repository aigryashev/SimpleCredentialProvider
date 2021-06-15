#include <Shlwapi.h>
#include <ShlGuid.h>
#include <combaseapi.h>

#include <cassert>
#include <array>

#include "consts/CommonConsts.hpp"
#include "SimpleCredentialProvider.hpp"
#include "SimpleCredentialProviderSystemUser.hpp"
#include "SimpleCredentialProviderCredentialGamer.hpp"
#include "SimpleCredentialProviderCredentialSystemUser.hpp"
#include "logging.hpp"
#include "dllmain.hpp"

namespace crm
{
namespace credential_provider
{

SimpleCredentialProvider::SimpleCredentialProvider()
    : mReferencesCounter(1)
    , mCPUS()
    , mRecreateEnumeratedCredentials(false)
    , mBuiltInCredentials()
    , mCredentialProviderUserArray(nullptr)
    , mGamerUser(nullptr)
    , mSystemUser()
{
    dllAddRef();

    mSystemUser.reset(new SimpleCredentialProviderSystemUser());
    mSystemUser->AddRef();
}

SimpleCredentialProvider::~SimpleCredentialProvider()
{
    dllRelease();
}

ULONG SimpleCredentialProvider::AddRef()
{
    return ++mReferencesCounter;
}

ULONG SimpleCredentialProvider::Release()
{
    assert(0 < mReferencesCounter);
    --mReferencesCounter;

    if (0 == mReferencesCounter)
    {
        delete this;
        return 0;
    }

    return mReferencesCounter;
}

HRESULT SimpleCredentialProvider::QueryInterface(REFIID riid, void ** ppv)
{
    static const QITAB qit[] = {
        QITABENT(SimpleCredentialProvider, ICredentialProvider),
        QITABENT(SimpleCredentialProvider, ICredentialProviderSetUserArray),
        QITABENT(SimpleCredentialProvider, ICredentialProviderUserArray),
        {0, 0}};

    return QISearch(this, qit, riid, ppv);
}

HRESULT SimpleCredentialProvider::Advise(ICredentialProviderEvents *, UINT_PTR)
{
    //TODO: add here the signaling about GUI app finished with 0 result code and the provider is
    // needed to logon with the Gamer built-in user (for additional info see the MS Credential
    // provider documentation
    // https://docs.microsoft.com/en-us/windows/win32/api/credentialprovider/nf-credentialprovider-icredentialprovider-advise)
    qDebug("!!!!!!!!!!![%s]",Q_FUNC_INFO);
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProvider::UnAdvise()
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProvider::GetCredentialAt(
        DWORD index,
        ICredentialProviderCredential ** credentials)
{
    if (nullptr == credentials)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *credentials = nullptr;

    if (mBuiltInCredentials.size() <= index)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    auto & builtInCredentialUPtr = mBuiltInCredentials[index];
    if (nullptr == builtInCredentialUPtr)
    {
        qDebug() << __FUNCTION__ << "Requested credential field is in unexpected state. Exiting";
        return E_UNEXPECTED;
    }

    OLECHAR* guidString;
    StringFromCLSID(__uuidof(**(credentials)), &guidString);
    IID_PPV_ARGS_Helper(credentials);
    HRESULT hr(builtInCredentialUPtr->QueryInterface(IID_PPV_ARGS(credentials)));
    ::CoTaskMemFree(guidString);

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << hr;
    return hr;
}

HRESULT SimpleCredentialProvider::SetUserArray(ICredentialProviderUserArray * users)
{
    if (nullptr == users)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    mCredentialProviderUserArray.reset(users);
    mCredentialProviderUserArray->AddRef();

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}


HRESULT SimpleCredentialProvider::GetCredentialCount(
        DWORD * count,
        DWORD * defaultCount,
        BOOL * autoLogonWithDefault)
{
    if (nullptr == count)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    if (nullptr != defaultCount)
        *defaultCount = CREDENTIAL_PROVIDER_NO_DEFAULT;

    if (nullptr != autoLogonWithDefault)
        *autoLogonWithDefault = FALSE;

    if (mRecreateEnumeratedCredentials)
    {
        mRecreateEnumeratedCredentials = false;
        resetEnumeratedCredentials();
        initEnumeratedCredentials();
    }

    *count = gSimpleCredentialProviderBuiltInUsersCount;

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProvider::GetFieldDescriptorAt(
        DWORD index,
        CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR ** fieldDescriptor)
{
    if (nullptr == fieldDescriptor)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *fieldDescriptor = nullptr;
    if (index >= static_cast<DWORD>(SimpleCredentialProviderFields::FieldsCount))
    {
        qDebug() << __FUNCTION__ << "Invalid index detected. Exiting";
        return E_INVALIDARG;
    }

    const DWORD sizeOfFieldDescriptor = sizeof(**fieldDescriptor);
    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR * providerFieldDescriptor =
            static_cast<CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR *>(CoTaskMemAlloc(sizeOfFieldDescriptor));
    if (nullptr == providerFieldDescriptor)
    {
        qDebug() << __FUNCTION__ << "Out of memory. Exiting";
        return  E_OUTOFMEMORY;
    }

    const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR & descriptor =
            consts::gSimpleCredentialProviderFieldDescriptors[index];
    providerFieldDescriptor->dwFieldID = descriptor.dwFieldID;
    providerFieldDescriptor->cpft = descriptor.cpft;
    providerFieldDescriptor->guidFieldType = descriptor.guidFieldType;

    HRESULT hr(E_INVALIDARG);
    if (descriptor.pszLabel)
    {
        hr = SHStrDupW(descriptor.pszLabel, &providerFieldDescriptor->pszLabel);
    }
    else
    {
        providerFieldDescriptor->pszLabel = nullptr;
        hr = S_OK;
    }

    if (SUCCEEDED(hr))
    {
        *fieldDescriptor = providerFieldDescriptor;
    }
    else
    {
        CoTaskMemFree(providerFieldDescriptor);
    }

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << hr;
    return hr;
}

HRESULT SimpleCredentialProvider::GetFieldDescriptorCount(
        DWORD * count)
{
    if (nullptr == count)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *count = static_cast<DWORD>(SimpleCredentialProviderFields::FieldsCount);

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProvider::SetSerialization(
        const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProvider::SetUsageScenario(
        CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
        DWORD)
{
    switch (cpus)
    {
    case CPUS_LOGON:
    case CPUS_UNLOCK_WORKSTATION:
        mCPUS = cpus;
        mRecreateEnumeratedCredentials = true;
        qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
        return S_OK;

    case CPUS_CHANGE_PASSWORD:
    case CPUS_CREDUI:
        qDebug() << __FUNCTION__ << "Finishing. Result code:" << E_NOTIMPL;
        return E_NOTIMPL;

    default:
        break;
    }

    qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
    return E_INVALIDARG;
}

void SimpleCredentialProvider::resetEnumeratedCredentials()
{
    SimpleCredentialProviderBuiltInCredentialsArray clearCredentials;
    mBuiltInCredentials.swap(clearCredentials);
}

void SimpleCredentialProvider::initEnumeratedCredentials()
{
    switch (mCPUS)
    {
    case CPUS_LOGON:
    case CPUS_UNLOCK_WORKSTATION:
        qDebug() << __FUNCTION__ << "Enumerating credentials";
        enumerateCredentials();
        break;

    default:
        break;
    }
}

void SimpleCredentialProvider::enumerateCredentials()
{
    if (nullptr == mCredentialProviderUserArray)
        return;

    DWORD dwUserCount;
    mCredentialProviderUserArray->GetCount(&dwUserCount);
    if (0 >= dwUserCount)
        return;

    auto & credentialGamerUPtrRef
            = mBuiltInCredentials[static_cast<std::size_t>(SimpleCredentialProviderBuiltInUsers::Gamer)];
    auto & credentialSystemUserUPtrRef
            = mBuiltInCredentials[static_cast<std::size_t>(SimpleCredentialProviderBuiltInUsers::SystemUser)];

    credentialGamerUPtrRef.reset(new(std::nothrow) SimpleCredentialProviderCredentialGamer());
    credentialSystemUserUPtrRef.reset(new(std::nothrow) SimpleCredentialProviderCredentialSystemUser());

    if (nullptr == credentialGamerUPtrRef.get()
            || nullptr == credentialSystemUserUPtrRef.get())
        return;

    if (FAILED(credentialGamerUPtrRef->init(mCPUS,
                                            consts::gSimpleCredentialProviderFieldDescriptors,
                                            consts::gSimpleCredentialProviderFieldStatePairs,
                                            mCredentialProviderUserArray.get(),
                                            dwUserCount)))
    {
        credentialGamerUPtrRef.reset();
    }

    if (FAILED(credentialSystemUserUPtrRef->init(mCPUS,
                                                 consts::gSimpleCredentialProviderFieldDescriptors,
                                                 consts::gSimpleCredentialProviderFieldStatePairs,
                                                 mCredentialProviderUserArray.get(),
                                                 dwUserCount)))
    {
        credentialSystemUserUPtrRef.reset();
    }
}

HRESULT SimpleCredentialProvider::GetAccountOptions(
        CREDENTIAL_PROVIDER_ACCOUNT_OPTIONS *)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProvider::GetAt(DWORD userIndex, ICredentialProviderUser ** user)
{
    if (static_cast<DWORD>(SimpleCredentialProviderBuiltInUsers::Count) <= userIndex)
        return E_INVALIDARG;

    switch (userIndex)
    {
    case static_cast<DWORD>(SimpleCredentialProviderBuiltInUsers::Gamer):
        *user = mGamerUser;
        break;

    case static_cast<DWORD>(SimpleCredentialProviderBuiltInUsers::SystemUser):
        *user = mSystemUser.get();
        break;

    default:
        assert(false && "unreachable code detected. If you added a user to the SimpleCredentialProviderBuiltInUsers you have to handle it here");
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT SimpleCredentialProvider::GetCount(DWORD * userCount)
{
    if (nullptr == userCount)
        return E_INVALIDARG;

    *userCount = static_cast<DWORD>(SimpleCredentialProviderBuiltInUsers::Count);

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProvider::SetProviderFilter(REFGUID/* guidProviderToFilterTo*/)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProvider_CreateInstance(_In_ REFIID riid, _Outptr_ void ** ppv)
{
    HRESULT hr;
    SimpleCredentialProvider *provider = new(std::nothrow) SimpleCredentialProvider();
    if (provider)
    {
        hr = provider->QueryInterface(riid, ppv);
        provider->Release();
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return hr;
}

} // ~namespace credential_provider
} // ~namespace CRM
