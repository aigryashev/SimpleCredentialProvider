#include <Shlwapi.h>
#include <wincred.h>
#include <NTSecAPI.h>
#include <intsafe.h>
#define SECURITY_WIN32
#include <security.h>
#include <strsafe.h>
#include <credentialprovider.h>

#include <codecvt>

#include "utils.hpp"
#include "logging.hpp"

namespace crm
{
namespace credential_provider
{
namespace utils
{

namespace
{

void unicodeStringPackedUnicodeStringCopy(
        const UNICODE_STRING & source,
        PWSTR buffer,
        UNICODE_STRING * destination)
{
    destination->Length = source.Length;
    destination->MaximumLength = source.Length;
    destination->Buffer = buffer;

    CopyMemory(destination->Buffer, source.Buffer, destination->Length);
}

HRESULT unicodeStringInitWithString(
        PWSTR source,
        UNICODE_STRING * destination)
{
    HRESULT hr;
    if (source)
    {
        size_t lenString = wcslen(source);
        USHORT usCharCount;
        hr = SizeTToUShort(lenString, &usCharCount);
        if (SUCCEEDED(hr))
        {
            USHORT usSize;
            hr = SizeTToUShort(sizeof(wchar_t), &usSize);
            if (SUCCEEDED(hr))
            {
                hr = UShortMult(usCharCount, usSize, &(destination->Length)); // Explicitly NOT including NULL terminator
                if (SUCCEEDED(hr))
                {
                    destination->MaximumLength = destination->Length;
                    destination->Buffer = source;
                    hr = S_OK;
                }
                else
                {
                    hr = HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
                }
            }
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }
    return hr;
}

HRESULT initString(
        PCSTR sourceString,
        PSTRING destinationString
        )
{
    size_t cchLength = strlen(sourceString);
    USHORT usLength;
    HRESULT hr = SizeTToUShort(cchLength, &usLength);
    if (SUCCEEDED(hr))
    {
        destinationString->Buffer = (PCHAR)sourceString;
        destinationString->Length = usLength;
        destinationString->MaximumLength = destinationString->Length+1;
        hr = S_OK;
    }

    return hr;
}

HRESULT protectAndCopyString(
        PCWSTR source,
        PWSTR * destination)
{
    *destination = nullptr;

    PWSTR pwzToProtectCopy;
    HRESULT hr = SHStrDupW(source, &pwzToProtectCopy);
    if (SUCCEEDED(hr))
    {
        DWORD cchProtected = 0;
        if (!CredProtectW(FALSE, pwzToProtectCopy, (DWORD)wcslen(pwzToProtectCopy)+1, nullptr, &cchProtected, nullptr))
        {
            DWORD errorCode = GetLastError();

            if ((ERROR_INSUFFICIENT_BUFFER == errorCode) && (0 < cchProtected))
            {
                PWSTR pwzProtected = (PWSTR)CoTaskMemAlloc(cchProtected * sizeof(wchar_t));
                if (pwzProtected)
                {
                    if (CredProtectW(FALSE, pwzToProtectCopy, (DWORD)wcslen(pwzToProtectCopy)+1, pwzProtected, &cchProtected, nullptr))
                    {
                        *destination = pwzProtected;
                        hr = S_OK;
                    }
                    else
                    {
                        CoTaskMemFree(pwzProtected);

                        errorCode = GetLastError();
                        hr = HRESULT_FROM_WIN32(errorCode);
                    }
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }
            }
            else
            {
                hr = HRESULT_FROM_WIN32(errorCode);
            }
        }
        else
        {
            hr = E_UNEXPECTED;
        }

        CoTaskMemFree(pwzToProtectCopy);
    }

    return hr;
}

} // ~anonymous namespace

HRESULT splitDomainAndUsername(
        PCWSTR qualifiedUserName,
        PWSTR * domain,
        PWSTR * username)
{
    HRESULT hr = E_UNEXPECTED;
    *domain = nullptr;
    *username = nullptr;
    PWSTR tempDomain;
    PWSTR tempUsername;
    const wchar_t *pchWhack = wcschr(qualifiedUserName, L'\\');
    const wchar_t *pchEnd = qualifiedUserName + wcslen(qualifiedUserName) - 1;

    const wchar_t * pchDomainBegin = qualifiedUserName;
    const wchar_t * pchDomainEnd = (nullptr == pchWhack ? qualifiedUserName : pchWhack - 1);
    const wchar_t * pchUsernameBegin = (nullptr == pchWhack ? qualifiedUserName : pchWhack + 1);
    const wchar_t * pchUsernameEnd = pchEnd;

    size_t lenDomain = pchDomainEnd - pchDomainBegin + 1;
    tempDomain = static_cast<PWSTR>(CoTaskMemAlloc(sizeof(wchar_t) * (lenDomain + 1)));
    if (tempDomain != nullptr)
    {
        hr = StringCchCopyN(tempDomain, lenDomain + 1, pchDomainBegin, lenDomain);
        if (SUCCEEDED(hr))
        {
            size_t lenUsername = pchUsernameEnd - pchUsernameBegin + 1;
            tempUsername = static_cast<PWSTR>(CoTaskMemAlloc(sizeof(wchar_t) * (lenUsername + 1)));
            if (tempUsername != nullptr)
            {
                hr = StringCchCopyN(tempUsername, lenUsername + 1, pchUsernameBegin, lenUsername);
                if (SUCCEEDED(hr))
                {
                    * domain = tempDomain;
                    *username = tempUsername;
                }
                else
                {
                    CoTaskMemFree(tempUsername);
                }
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }
        }

        if (FAILED(hr))
        {
            CoTaskMemFree(tempDomain);
        }
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

HRESULT protectIfNecessaryAndCopyPassword(
        PCWSTR password,
        CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
        PWSTR * protectedPassword)
{
    *protectedPassword = nullptr;

    HRESULT hr;

    if (password && *password)
    {
        PWSTR passwordCopy;
        hr = SHStrDupW(password, &passwordCopy);
        if (SUCCEEDED(hr))
        {
            bool bCredAlreadyEncrypted = false;
            CRED_PROTECTION_TYPE protectionType;

            if (CredIsProtectedW(passwordCopy, &protectionType))
            {
                if (CredUnprotected != protectionType)
                {
                    bCredAlreadyEncrypted = true;
                }
            }

            if (CPUS_CREDUI == cpus || bCredAlreadyEncrypted)
            {
                hr = SHStrDupW(passwordCopy, protectedPassword);
            }
            else
            {
                hr = protectAndCopyString(passwordCopy, protectedPassword);
            }

            CoTaskMemFree(passwordCopy);
        }
    }
    else
    {
        hr = SHStrDupW(L"", protectedPassword);
    }

    return hr;
}

HRESULT readPWSTRIntoWString(
        std::function<HRESULT (PWSTR *)> readCallback,
        std::wstring & resultString)
{
    if (!readCallback)
        return E_INVALIDARG;

    HRESULT hr(S_OK);
    RAIIWrapperForPWSTR buffer;
    hr = readCallback(&(buffer.str));

    if (FAILED(hr))
        return hr;

    if (nullptr == buffer.str)
        resultString.clear();
    else
        resultString = buffer.str;

    return S_OK;
}

HRESULT kerbInteractiveUnlockLogonInit(
        PWSTR domain,
        PWSTR username,
        PWSTR password,
        CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
        KERB_INTERACTIVE_UNLOCK_LOGON * kerbInteractiveUnlockLogon)
{
    KERB_INTERACTIVE_UNLOCK_LOGON kerbInteractiveUnlockLogonSource;
    ZeroMemory(&kerbInteractiveUnlockLogonSource, sizeof(kerbInteractiveUnlockLogonSource));

    KERB_INTERACTIVE_LOGON * kerbInteractiveLogon = &kerbInteractiveUnlockLogonSource.Logon;

    HRESULT hr = unicodeStringInitWithString(domain, &kerbInteractiveLogon->LogonDomainName);
    if (SUCCEEDED(hr))
    {
        hr = unicodeStringInitWithString(username, &kerbInteractiveLogon->UserName);
        if (SUCCEEDED(hr))
        {
            hr = unicodeStringInitWithString(password, &kerbInteractiveLogon->Password);
            if (SUCCEEDED(hr))
            {
                switch (cpus)
                {
                case CPUS_UNLOCK_WORKSTATION:
                    kerbInteractiveLogon->MessageType = KerbWorkstationUnlockLogon;
                    hr = S_OK;
                    break;

                case CPUS_LOGON:
                    kerbInteractiveLogon->MessageType = KerbInteractiveLogon;
                    hr = S_OK;
                    break;

                case CPUS_CREDUI:
                    kerbInteractiveLogon->MessageType = (KERB_LOGON_SUBMIT_TYPE)0; // MessageType does not apply to CredUI
                    hr = S_OK;
                    break;

                default:
                    hr = E_FAIL;
                    break;
                }

                if (SUCCEEDED(hr))
                {
                    CopyMemory(kerbInteractiveUnlockLogon,
                               &kerbInteractiveUnlockLogonSource,
                               sizeof(*kerbInteractiveUnlockLogon));
                }
            }
        }
    }

    return hr;
}

HRESULT kerbInteractiveUnlockLogonPack(
        const KERB_INTERACTIVE_UNLOCK_LOGON & kerbInteractiveUnlockLogon,
        BYTE ** packedResult,
        DWORD * packedResultSize)
{
    if (nullptr == packedResult
            || nullptr == packedResultSize)
        return E_INVALIDARG;

    const KERB_INTERACTIVE_LOGON * kerbInteractiveLogin = &kerbInteractiveUnlockLogon.Logon;
    DWORD resultSize = sizeof(kerbInteractiveUnlockLogon) +
            kerbInteractiveLogin->LogonDomainName.Length +
            kerbInteractiveLogin->UserName.Length +
            kerbInteractiveLogin->Password.Length;

    KERB_INTERACTIVE_UNLOCK_LOGON * outKerbInteractiveLogin = (KERB_INTERACTIVE_UNLOCK_LOGON*)CoTaskMemAlloc(resultSize);
    if (outKerbInteractiveLogin)
    {
        ZeroMemory(&outKerbInteractiveLogin->LogonId, sizeof(outKerbInteractiveLogin->LogonId));
        BYTE * pbBuffer = (BYTE*)outKerbInteractiveLogin + sizeof(*outKerbInteractiveLogin);
        KERB_INTERACTIVE_LOGON *pkilOut = &outKerbInteractiveLogin->Logon;

        pkilOut->MessageType = kerbInteractiveLogin->MessageType;

        unicodeStringPackedUnicodeStringCopy(kerbInteractiveLogin->LogonDomainName, (PWSTR)pbBuffer, &pkilOut->LogonDomainName);
        pkilOut->LogonDomainName.Buffer = (PWSTR)(pbBuffer - (BYTE*)outKerbInteractiveLogin);
        pbBuffer += pkilOut->LogonDomainName.Length;

        unicodeStringPackedUnicodeStringCopy(kerbInteractiveLogin->UserName, (PWSTR)pbBuffer, &pkilOut->UserName);
        pkilOut->UserName.Buffer = (PWSTR)(pbBuffer - (BYTE*)outKerbInteractiveLogin);
        pbBuffer += pkilOut->UserName.Length;

        unicodeStringPackedUnicodeStringCopy(kerbInteractiveLogin->Password, (PWSTR)pbBuffer, &pkilOut->Password);
        pkilOut->Password.Buffer = (PWSTR)(pbBuffer - (BYTE*)outKerbInteractiveLogin);

        *packedResult = (BYTE*)outKerbInteractiveLogin;
        *packedResultSize = resultSize;

        return S_OK;
    }

    return E_OUTOFMEMORY;
}

HRESULT retrieveNegotiateAuthPackage(ULONG * authPackage)
{
    HRESULT hr;
    HANDLE hLsa;

    NTSTATUS status = LsaConnectUntrusted(&hLsa);
    if (SUCCEEDED(HRESULT_FROM_NT(status)))
    {
        ULONG lsaAuthPackage;
        LSA_STRING lsaszKerberosName;
        initString(NEGOSSP_NAME_A, &lsaszKerberosName);

        status = LsaLookupAuthenticationPackage(hLsa, &lsaszKerberosName, &lsaAuthPackage);
        if (SUCCEEDED(HRESULT_FROM_NT(status)))
        {
            *authPackage = lsaAuthPackage;
            hr = S_OK;
        }
        else
        {
            hr = HRESULT_FROM_NT(status);
        }
        LsaDeregisterLogonProcess(hLsa);
    }
    else
    {
        hr = HRESULT_FROM_NT(status);
    }

    return hr;
}

std::string utf16ToUtf8(const std::wstring & source)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    const std::string utf8String = convert.to_bytes(source);
    return utf8String;
}

} // ~namespace utils
} // ~namespace credential_provider
} // ~namespace CRM
