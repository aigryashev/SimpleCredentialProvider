#pragma once

#include <string>
#include <functional>

#include <winnt.h>
#include <credentialprovider.h>
#include <ntsecapi.h>

namespace crm
{
namespace credential_provider
{
namespace utils
{

struct RAIIWrapperForPWSTR
{
    PWSTR str = nullptr;

    ~RAIIWrapperForPWSTR()
    {
        CoTaskMemFree(str);
    }
};

HRESULT splitDomainAndUsername(
        PCWSTR qualifiedUserName,
        PWSTR *domain,
        PWSTR *username);

HRESULT protectIfNecessaryAndCopyPassword(
        PCWSTR password,
        CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
        PWSTR * protectedPassword);

HRESULT readPWSTRIntoWString(
        std::function<HRESULT (PWSTR *)> readCallback,
        std::wstring & resultString);

HRESULT kerbInteractiveUnlockLogonInit(
        PWSTR domain,
        PWSTR username,
        PWSTR password,
        CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
        KERB_INTERACTIVE_UNLOCK_LOGON * kerbInteractiveUnlockLogon);

HRESULT kerbInteractiveUnlockLogonPack(
        const KERB_INTERACTIVE_UNLOCK_LOGON & kerbInteractiveUnlockLogon,
        BYTE ** packedResult,
        DWORD * packedResultSize);

HRESULT retrieveNegotiateAuthPackage(
        ULONG * authPackage);

std::string utf16ToUtf8(
        const std::wstring & source);

} // ~namespace utils
} // ~namespace credential_provider
} // ~namespace CRM
