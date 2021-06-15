#include <Shlwapi.h>
#include <propkey.h>
#include <wincred.h>
#include <NTSecAPI.h>

#include "logging.hpp"
#include "consts/CommonConsts.hpp"
#include "consts/GUID.hpp"
#include "utils/utils.hpp"
#include "resources/resources.h"
#include "dllmain.hpp"
#include "SimpleCredentialProviderCredentialSystemUser.hpp"

namespace crm
{
namespace credential_provider
{

SimpleCredentialProviderCredentialSystemUser::SimpleCredentialProviderCredentialSystemUser()
    : SimpleCredentialProviderCredentialBase()
{
}

HRESULT SimpleCredentialProviderCredentialSystemUser::init(
        CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
        const SimpleCredentialProviderFieldDescriptorArray & descriptorArray,
        const SimpleCredentialProviderFieldStatePairArray & fieldStatePairArray,
        ICredentialProviderUserArray * userArray,
        DWORD usersCount)
{
    HRESULT hr(SimpleCredentialProviderCredentialBase::init(cpus, descriptorArray, fieldStatePairArray,
                                                            userArray, usersCount));

    if (FAILED(hr))
        return hr;

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialSystemUser::GetBitmapValue(
        DWORD fieldID, HBITMAP * outBitmap)
{
    HRESULT hr;
    *outBitmap = nullptr;

    if (static_cast<DWORD>(SimpleCredentialProviderFields::TileSystemUser) == fieldID)
    {
        HBITMAP bmp = LoadBitmap(gDllInstance, MAKEINTRESOURCE(SIMPLECREDPROV_SYSTEMUSERTILEIMAGE));
        if (bmp != nullptr)
        {
            qDebug() << __FUNCTION__ << "The bitmap image sucessfully loaded";
            hr = S_OK;
            *outBitmap = bmp;
        }
        else
        {
            qDebug() << __FUNCTION__ << "Can't load bitmap image for the Gamer tile. Exiting";
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    else
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        hr = E_INVALIDARG;
    }

    return hr;
}

HRESULT SimpleCredentialProviderCredentialSystemUser::GetSerialization(
        CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE * getSerializationResponse,
        CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * serialization,
        PWSTR * optionalStatusText,
        CREDENTIAL_PROVIDER_STATUS_ICON * optionalStatusIcon)
{
    std::wstring enteredUserName(
                mFieldStrings[static_cast<DWORD>(SimpleCredentialProviderFields::LineEditUserName)]);

    if (consts::gGamerUserName == enteredUserName)
    {
        qDebug() << __FUNCTION__ << "Login with predefined user name is not allowed. User name: "
                 << enteredUserName.c_str();
        return E_FAIL;
    }

    std::string utf8UserName;
    std::wstring enteredPassword(
                mFieldStrings[static_cast<DWORD>(SimpleCredentialProviderFields::LineEditPassword)]);

    qDebug() << __FUNCTION__ << "Serializing username/password: " << enteredUserName << "/" << enteredPassword;

    HRESULT hr = E_UNEXPECTED;
    *getSerializationResponse = CPGSR_NO_CREDENTIAL_NOT_FINISHED;
    *optionalStatusText = nullptr;
    *optionalStatusIcon = CPSI_NONE;
    ZeroMemory(serialization, sizeof(*serialization));

    const auto it = mQualifiedUserNames.find(enteredUserName);
    bool isLocalUser(true);
    if (mQualifiedUserNames.cend() != it)
    {
        isLocalUser = it->second;
    }

    if (isLocalUser)
    {
        PWSTR pwzProtectedPassword;
        hr = utils::protectIfNecessaryAndCopyPassword(
                    enteredPassword.c_str(),
                    mCPUS, &pwzProtectedPassword);

        if (SUCCEEDED(hr))
        {
            PWSTR pszDomain;
            PWSTR pszUsername;
            hr = utils::splitDomainAndUsername(enteredUserName.c_str(), &pszDomain, &pszUsername);

            if (SUCCEEDED(hr))
            {
                KERB_INTERACTIVE_UNLOCK_LOGON kiul;
                hr = utils::kerbInteractiveUnlockLogonInit(pszDomain,
                                                           pszUsername,
                                                           pwzProtectedPassword, mCPUS, &kiul);

                if (SUCCEEDED(hr))
                {
                    hr = utils::kerbInteractiveUnlockLogonPack(kiul, &serialization->rgbSerialization, &serialization->cbSerialization);
                    if (SUCCEEDED(hr))
                    {
                        ULONG ulAuthPackage;
                        hr = utils::retrieveNegotiateAuthPackage(&ulAuthPackage);
                        if (SUCCEEDED(hr))
                        {
                            serialization->ulAuthenticationPackage = ulAuthPackage;
                            serialization->clsidCredentialProvider = consts::gSimpleCredentialProviderGUID;
                            *getSerializationResponse = CPGSR_RETURN_CREDENTIAL_FINISHED;
                        }
                    }
                }
                CoTaskMemFree(pszDomain);
                CoTaskMemFree(pszUsername);
            }
            CoTaskMemFree(pwzProtectedPassword);
        }
    }
    else
    {
        DWORD dwAuthFlags = CRED_PACK_PROTECTED_CREDENTIALS | CRED_PACK_ID_PROVIDER_CREDENTIALS;

        if (!CredPackAuthenticationBuffer(dwAuthFlags,
                                          const_cast<PWSTR>(enteredUserName.c_str()),
                                          const_cast<PWSTR>(enteredPassword.c_str()),
                                          nullptr,
                                          &serialization->cbSerialization)
                && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
        {
            serialization->rgbSerialization = static_cast<byte *>(CoTaskMemAlloc(serialization->cbSerialization));
            if (serialization->rgbSerialization != nullptr)
            {
                hr = S_OK;

                if (CredPackAuthenticationBuffer(dwAuthFlags,
                                                 const_cast<PWSTR>(enteredUserName.c_str()),
                                                 const_cast<PWSTR>(enteredPassword.c_str()),
                                                 serialization->rgbSerialization,
                                                 &serialization->cbSerialization))
                {
                    ULONG ulAuthPackage;
                    hr = utils::retrieveNegotiateAuthPackage(&ulAuthPackage);
                    if (SUCCEEDED(hr))
                    {
                        serialization->ulAuthenticationPackage = ulAuthPackage;
                        serialization->clsidCredentialProvider = consts::gSimpleCredentialProviderGUID;
                        *getSerializationResponse = CPGSR_RETURN_CREDENTIAL_FINISHED;
                    }
                }
                else
                {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                    if (SUCCEEDED(hr))
                    {
                        hr = E_FAIL;
                    }
                }

                if (FAILED(hr))
                {
                    CoTaskMemFree(serialization->rgbSerialization);
                }
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }
        }
    }

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << hr;
    return hr;
}

IFACEMETHODIMP SimpleCredentialProviderCredentialSystemUser::GetFieldOptions(
        DWORD fieldID,
        CREDENTIAL_PROVIDER_CREDENTIAL_FIELD_OPTIONS * options)
{
    if (nullptr == options)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *options = CPCFO_NONE;

    if (fieldID == static_cast<DWORD>(SimpleCredentialProviderFields::LineEditPassword))
    {
        *options = CPCFO_ENABLE_PASSWORD_REVEAL;
    }
    else if (fieldID == static_cast<DWORD>(SimpleCredentialProviderFields::TileSystemUser))
    {
        *options = CPCFO_ENABLE_TOUCH_KEYBOARD_AUTO_INVOKE;
    }

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

} // ~namespace credential_provider
} // ~namespace CRM
