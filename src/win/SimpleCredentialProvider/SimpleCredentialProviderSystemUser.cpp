#include <string>
#include <cassert>

#include <tchar.h>
#include <Shlwapi.h>
#include <propkey.h>
#include <propvarutil.h>

#include "SimpleCredentialProviderSystemUser.hpp"
#include "logging.hpp"

#include "consts/GUID.hpp"

namespace crm
{
namespace credential_provider
{

namespace
{
const std::wstring gSystemUserSID(L"other-user-sid");

const std::wstring gSystemUserStr(L"Other user");
const std::wstring gSystemUserSIDStr(L"other-user-sid");

} // ~anonymous namespace

ULONG SimpleCredentialProviderSystemUser::AddRef()
{
    return ++mReferencesCounter;
}

ULONG SimpleCredentialProviderSystemUser::Release()
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

HRESULT SimpleCredentialProviderSystemUser::QueryInterface(
        REFIID riid,
        void ** ppv)
{
    static const QITAB qit[] = {
        QITABENT(SimpleCredentialProviderSystemUser, ICredentialProviderUser),
        {0, 0}};

    return QISearch(this, qit, riid, ppv);
}

    /// ICredentialProviderUser
HRESULT SimpleCredentialProviderSystemUser::GetProviderID(GUID * providerID)
{
    if (nullptr == providerID)
        return E_INVALIDARG;

    *providerID = consts::gSimpleCredentialProviderGUID;
    return S_OK;
}

HRESULT SimpleCredentialProviderSystemUser::GetSid(LPWSTR * sid)
{
    if (nullptr == sid)
        return E_INVALIDARG;

    *sid = const_cast<wchar_t *>(gSystemUserSID.c_str());
    return S_OK;
}

HRESULT SimpleCredentialProviderSystemUser::GetStringValue(REFPROPERTYKEY key, LPWSTR * stringValue)
{
    if (nullptr == stringValue)
        return E_INVALIDARG;

    if (PKEY_Identity_DisplayName == key)
    {
        *stringValue = const_cast<wchar_t *>(gSystemUserStr.c_str());
        return S_OK;
    }

    if (PKEY_Identity_UserName == key)
    {
        *stringValue = const_cast<wchar_t *>(gSystemUserStr.c_str());
        return S_OK;
    }

    if (PKEY_Identity_QualifiedUserName == key)
    {
        *stringValue = const_cast<wchar_t *>(gSystemUserStr.c_str());
        return S_OK;
    }

    if (PKEY_Identity_LogonStatusString == key)
    {
        *stringValue = const_cast<wchar_t *>(gSystemUserStr.c_str());
        return S_OK;
    }

    if (PKEY_Identity_PrimarySid == key)
    {
        *stringValue = const_cast<wchar_t *>(gSystemUserSIDStr.c_str());
        return S_OK;
    }

    if (PKEY_Identity_ProviderID == key)
    {
        *stringValue = const_cast<wchar_t *>(consts::gSimpleCredentialProviderGUIDStr.c_str());
        return S_OK;
    }

    return E_NOT_SET;
}

HRESULT SimpleCredentialProviderSystemUser::GetValue(REFPROPERTYKEY key, PROPVARIANT * value)
{
    if (nullptr == value)
        return E_INVALIDARG;

    if (PKEY_Identity_DisplayName == key)
        return InitPropVariantFromString(gSystemUserStr.c_str(), value);

    if (PKEY_Identity_UserName == key)
        return InitPropVariantFromString(gSystemUserStr.c_str(), value);

    if (PKEY_Identity_QualifiedUserName == key)
        return InitPropVariantFromString(gSystemUserStr.c_str(), value);

    if (PKEY_Identity_LogonStatusString == key)
        return InitPropVariantFromString(gSystemUserStr.c_str(), value);

    if (PKEY_Identity_PrimarySid == key)
        return InitPropVariantFromString(gSystemUserStr.c_str(), value);

    if (PKEY_Identity_ProviderID == key)
        return InitPropVariantFromGUIDAsString(consts::gSimpleCredentialProviderGUID, value);

    return E_NOT_SET;
}

} // ~namespace credential_provider
} // ~namespace CRM
