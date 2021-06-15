#include <Shlwapi.h>
#include <propkey.h>
#include <wincred.h>
#include <NTSecAPI.h>

#include <cstdlib>
#include <fstream>

#include <QProcess>
#include <QDir>

#include <crm_dll.h>

#include "logging.hpp"
#include "consts/CommonConsts.hpp"
#include "consts/GUID.hpp"
#include "utils/utils.hpp"
#include "resources/resources.h"
#include "dllmain.hpp"

#include "SimpleCredentialProviderCredentialGamer.hpp"

namespace crm
{
namespace credential_provider
{

namespace
{
const std::wstring gUserLabel(L"Gamer");
const QString gUIAppExecutable("SimpleCredentialGamerUI.exe");
} //~anonymous namespace

SimpleCredentialProviderCredentialGamer::SimpleCredentialProviderCredentialGamer()
    : SimpleCredentialProviderCredentialBase()
    , mUIProcessWatchdog()
{
}

HRESULT SimpleCredentialProviderCredentialGamer::init(
        CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
        const SimpleCredentialProviderFieldDescriptorArray & descriptorArray,
        const SimpleCredentialProviderFieldStatePairArray & fieldStatePairArray,
        ICredentialProviderUserArray * userArray,
        DWORD usersCount)
{
    deleteCRMUserName();

    HRESULT hr(SimpleCredentialProviderCredentialBase::init(cpus, descriptorArray, fieldStatePairArray,
                                                            userArray, usersCount));

    if (FAILED(hr))
        return hr;

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialGamer::GetBitmapValue(
        DWORD fieldID, HBITMAP * outBitmap)
{
    HRESULT hr;
    *outBitmap = nullptr;

    if (static_cast<DWORD>(SimpleCredentialProviderFields::TileGamerUser) == fieldID)
    {
        HBITMAP bmp = LoadBitmap(gDllInstance, MAKEINTRESOURCE(SIMPLECREDPROV_GAMERTILEIMAGE));
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

bool SimpleCredentialProviderCredentialGamer::isCRMUser(
        const std::wstring & enteredUserName,
        const std::wstring & enteredPassword,
        std::string & outUTF8UserName)
{
    const std::string utf8UserName(utils::utf16ToUtf8(enteredUserName));
    const std::string utf8Password(utils::utf16ToUtf8(enteredPassword));

    const int crmCheckResult(loginAPI(const_cast<char *>(utf8UserName.c_str()),
                                      static_cast<int>(utf8UserName.size() + 1),
                                      const_cast<char *>(utf8Password.c_str()),
                                      static_cast<int>(utf8Password.size() + 1)));

    const int result(crmCheckResult);
    qDebug() << __FUNCTION__ << "CRM login code: " << result;

    if (1 != result)
        return false;

    outUTF8UserName = utf8UserName;
    return true;
}

bool SimpleCredentialProviderCredentialGamer::deleteCRMUserName()
{
    std::string filePath(std::getenv("ALLUSERSPROFILE"));
    filePath += "\\crm\\username.txt";

    qDebug() << __FUNCTION__ << "Writing empty user name to file " << filePath.c_str() << "";
    try {
        std::ofstream fstream(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
        fstream << "";
    }
    catch (std::exception & ex)
    {
        qDebug() << __FUNCTION__ << "Can't write the file " << filePath.c_str() << ". Exception: " << ex.what();
        return false;
    }
    catch (...)
    {
        qDebug() << __FUNCTION__ << "Can't write the file " << filePath.c_str();
        return false;
    }

    qDebug() << __FUNCTION__ << "The file " << filePath.c_str() << " was successfully saved";
    return true;
}

HRESULT SimpleCredentialProviderCredentialGamer::GetSerialization(
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

    if (!isCRMUser(enteredUserName, enteredPassword, utf8UserName))
    {
        qDebug() << __FUNCTION__ << "Can't use non-CRM user name to logon for this credential. Exiting";
        return E_FAIL;
    }

    /// Override the user name and password to use predefined non-privileged user name/password
    qDebug() << __FUNCTION__ << " Overriding entered user/password with predefined ones";
    enteredUserName = consts::gGamerUserName;
    enteredPassword = consts::gGamerUserPassword;

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

IFACEMETHODIMP SimpleCredentialProviderCredentialGamer::GetFieldOptions(
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
    else if (fieldID == static_cast<DWORD>(SimpleCredentialProviderFields::TileGamerUser))
    {
        *options = CPCFO_ENABLE_TOUCH_KEYBOARD_AUTO_INVOKE;
    }

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

IFACEMETHODIMP SimpleCredentialProviderCredentialGamer::SetSelected(
        BOOL * autoLogon)
{
    const HRESULT hr(SimpleCredentialProviderCredentialBase::SetSelected(autoLogon));

    if (FAILED(hr))
        return hr;

    if (mUIProcessWatchdog.isRunning())
        ///NOTE: the watchdog is only owned by this object so we may trust it. But it's still abnormal code block
        return S_OK;

    static const QString appPath(QString("%1%2crm%3%4")
                                 .arg(std::getenv("ALLUSERSPROFILE"))
                                 .arg(QDir::separator())
                                 .arg(QDir::separator())
                                 .arg(gUIAppExecutable));

    mUIProcessWatchdog.start(appPath);

    return hr;
}

} // ~namespace credential_provider
} // ~namespace CRM
