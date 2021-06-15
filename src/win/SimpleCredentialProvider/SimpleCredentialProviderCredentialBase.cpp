#include <Shlwapi.h>
#include <propkey.h>
#include <wincred.h>
#include <NTSecAPI.h>

#include "logging.hpp"
#include "consts/CommonConsts.hpp"
#include "utils/utils.hpp"
#include "resources/resources.h"
#include "dllmain.hpp"
#include "SimpleCredentialProviderCredentialBase.hpp"

namespace crm
{
namespace credential_provider
{
namespace
{

struct ReportResultStatusInfo
{
    const NTSTATUS status;
    const NTSTATUS substatus;
    const std::wstring message;
    const CREDENTIAL_PROVIDER_STATUS_ICON cpsi;
};

const std::array<ReportResultStatusInfo, 2> gLogonStatusInfo =
{
    ReportResultStatusInfo{STATUS_LOGON_FAILURE,
                           static_cast<NTSTATUS>(0),
                           L"Incorrect password or username.",
                           CPSI_ERROR},

    ReportResultStatusInfo{STATUS_ACCOUNT_RESTRICTION,
                           STATUS_ACCOUNT_DISABLED,
                           L"The account is disabled.",
                           CPSI_WARNING}
};

const GUID gLocalUserProviderGUID{0xA198529B, 0x730F, 0x4089, {0xB6, 0x46, 0xA1, 0x25, 0x57, 0xF5, 0x66, 0x5E}};

const std::array<CREDENTIAL_PROVIDER_FIELD_TYPE, 2> gSupportedSettableFieldTypes
{
    CPFT_EDIT_TEXT,
    CPFT_PASSWORD_TEXT
};

} // ~anonymous namespace

ULONG SimpleCredentialProviderCredentialBase::AddRef()
{
    return ++mReferencesCounter;
}

ULONG SimpleCredentialProviderCredentialBase::Release()
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

HRESULT SimpleCredentialProviderCredentialBase::QueryInterface(
        REFIID riid,
        void ** ppv)
{
    static const QITAB qit[] =
    {
        QITABENTMULTI(SimpleCredentialProviderCredentialBase, ICredentialProviderCredential, ICredentialProviderCredential2),
        QITABENTMULTI(SimpleCredentialProviderCredentialBase, ICredentialProviderCredential2, ICredentialProviderCredential2),
        QITABENTMULTI(SimpleCredentialProviderCredentialBase, ICredentialProviderCredentialWithFieldOptions, ICredentialProviderCredentialWithFieldOptions),
        {0, 0}};

    return QISearch(this, qit, riid, ppv);
}

SimpleCredentialProviderCredentialBase::SimpleCredentialProviderCredentialBase()
    : mReferencesCounter(1)
    , mCPUS()
    , mCredentialProviderFieldDescriptors()
    , mCredentialProviderFieldStatePairs()
    , mFieldStrings()
    , mQualifiedUserNames()
    , mSID()
    , mCredentialEvents(nullptr)
{
    dllAddRef();
}

SimpleCredentialProviderCredentialBase::~SimpleCredentialProviderCredentialBase()
{
    dllRelease();
}

HRESULT SimpleCredentialProviderCredentialBase::init(
        CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
        const SimpleCredentialProviderFieldDescriptorArray & descriptorArray,
        const SimpleCredentialProviderFieldStatePairArray & fieldStatePairArray,
        ICredentialProviderUserArray * userArray,
        DWORD usersCount)
{
    if (0 >= usersCount
            || nullptr == userArray)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    mCPUS = cpus;

    GUID guidProvider;
    mQualifiedUserNames.clear();
    for (DWORD i(0); i < usersCount; ++i)
    {
        ICredentialProviderUser * credentialsUserRawPtr(nullptr);
        if (FAILED(userArray->GetAt(i, &credentialsUserRawPtr)))
        {
            continue;
        }

        if (FAILED(credentialsUserRawPtr->GetProviderID(&guidProvider)))
        {
            continue;
        }

        std::wstring qualifiedUserName;
        if (FAILED(utils::readPWSTRIntoWString(
                       [credentialsUserRawPtr](PWSTR * str){ return credentialsUserRawPtr->GetStringValue(PKEY_Identity_UserName, str); },
                       qualifiedUserName)))
        {
            continue;
        }
        const bool isLocalUser(gLocalUserProviderGUID == guidProvider);
        mQualifiedUserNames.insert_or_assign(qualifiedUserName, isLocalUser);
    }

    if (mQualifiedUserNames.empty())
    {
        qDebug() << __FUNCTION__ << "Can't get any user info. Exiting";
        return E_INVALIDARG;
    }

    mFieldStrings[static_cast<DWORD>(SimpleCredentialProviderFields::LineEditUserName)] = L"";
    mFieldStrings[static_cast<DWORD>(SimpleCredentialProviderFields::LineEditPassword)] = L"";
    mFieldStrings[static_cast<DWORD>(SimpleCredentialProviderFields::ButtonSubmit)] = L"Submit";
    mFieldStrings[static_cast<DWORD>(SimpleCredentialProviderFields::TileGamerUser)] = consts::gGamerUserName;
    mFieldStrings[static_cast<DWORD>(SimpleCredentialProviderFields::TileSystemUser)] = consts::gSystemUserName;

    mCredentialProviderFieldDescriptors = descriptorArray;
    mCredentialProviderFieldStatePairs = fieldStatePairArray;

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialBase::Advise(ICredentialProviderCredentialEvents * credentialEvents)
{
    ICredentialProviderCredentialEvents2 * credentialEventsRAWPtr(nullptr);
    const HRESULT hr(credentialEvents->QueryInterface(IID_PPV_ARGS(&credentialEventsRAWPtr)));

    mCredentialEvents.reset(credentialEventsRAWPtr);

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << hr;
    return hr;
}

HRESULT SimpleCredentialProviderCredentialBase::UnAdvise()
{
    mCredentialEvents.reset();

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialBase::SetSelected(BOOL * autoLogon)
{
    if (nullptr == autoLogon)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *autoLogon = FALSE;

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialBase::SetDeselected()
{
    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialBase::GetFieldState(
        DWORD fieldID,
        CREDENTIAL_PROVIDER_FIELD_STATE * fieldState,
        CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE * fieldInteractiveState)
{
    if ((nullptr == fieldState)
            || (nullptr == fieldInteractiveState)
            || (fieldID >= mCredentialProviderFieldStatePairs.size()))
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *fieldState = mCredentialProviderFieldStatePairs[fieldID].state;
    *fieldInteractiveState = mCredentialProviderFieldStatePairs[fieldID].interactiveState;

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialBase::GetStringValue(
        DWORD fieldID,
        PWSTR * outString)
{
    if (nullptr == outString)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *outString = nullptr;
    const auto it(mFieldStrings.find(fieldID));
    if (mFieldStrings.cend() == it)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    const HRESULT hr(SHStrDupW(it->second.c_str(), outString));
    qDebug() << __FUNCTION__ << "Finishing. Result code:" << hr;
    return hr;
}

HRESULT SimpleCredentialProviderCredentialBase::GetBitmapValue(
        DWORD, HBITMAP *)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProviderCredentialBase::GetSubmitButtonValue(
        DWORD fieldID,
        DWORD * adjacentTo)
{
    if (nullptr == adjacentTo)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    if (static_cast<DWORD>(SimpleCredentialProviderFields::ButtonSubmit) == fieldID)
    {
        *adjacentTo = static_cast<DWORD>(SimpleCredentialProviderFields::LineEditPassword);

        qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
        return S_OK;
    }

    qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
    return E_INVALIDARG;
}

HRESULT SimpleCredentialProviderCredentialBase::SetStringValue(DWORD fieldID, PCWSTR value)
{
    if (fieldID >= mCredentialProviderFieldDescriptors.size())
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    auto it = std::find(gSupportedSettableFieldTypes.cbegin(), gSupportedSettableFieldTypes.cend(), mCredentialProviderFieldDescriptors[fieldID].cpft);
    if (gSupportedSettableFieldTypes.cend() == it)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    mFieldStrings[fieldID] = value;

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialBase::GetCheckboxValue(
        DWORD, BOOL *, PWSTR *)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProviderCredentialBase::SetCheckboxValue(DWORD, BOOL)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProviderCredentialBase::GetComboBoxValueCount(
        DWORD, DWORD *, DWORD *)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProviderCredentialBase::GetComboBoxValueAt(
        DWORD, DWORD, PWSTR *)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProviderCredentialBase::SetComboBoxSelectedValue(
        DWORD, DWORD)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProviderCredentialBase::CommandLinkClicked(DWORD)
{
    return E_NOTIMPL;
}


HRESULT SimpleCredentialProviderCredentialBase::GetSerialization(
        CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *,
        CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *,
        PWSTR *,
        CREDENTIAL_PROVIDER_STATUS_ICON *)
{
    return E_NOTIMPL;
}

HRESULT SimpleCredentialProviderCredentialBase::ReportResult(
        NTSTATUS status,
        NTSTATUS substatus,
        PWSTR * optionalStatusText,
        CREDENTIAL_PROVIDER_STATUS_ICON * optionalStatusIcon)
{
    if (nullptr == optionalStatusText
            || nullptr == optionalStatusIcon)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *optionalStatusText = nullptr;
    *optionalStatusIcon = CPSI_NONE;

    DWORD statusInfo = (DWORD)-1;

    for (DWORD i = 0; i < gLogonStatusInfo.size(); ++i)
    {
        if (gLogonStatusInfo[i].status == status
                && gLogonStatusInfo[i].substatus == substatus)
        {
            statusInfo = i;
            break;
        }
    }

    if ((DWORD)-1 != statusInfo)
    {
        if (SUCCEEDED(SHStrDupW(static_cast<PWSTR>(const_cast<wchar_t *>(gLogonStatusInfo[statusInfo].message.c_str())),
                                optionalStatusText)))
        {
            *optionalStatusIcon = gLogonStatusInfo[statusInfo].cpsi;
        }
    }

    if (FAILED(HRESULT_FROM_NT(status)))
    {
        if (mCredentialEvents)
        {
            mCredentialEvents->SetFieldString(this,
                                              static_cast<DWORD>(SimpleCredentialProviderFields::LineEditPassword),
                                              L"");
        }
    }

    qDebug() << __FUNCTION__ << "Finishing. Result code:" << S_OK;
    return S_OK;
}

HRESULT SimpleCredentialProviderCredentialBase::GetUserSid(PWSTR * sid)
{
    if (nullptr == sid)
    {
        qDebug() << __FUNCTION__ << "Invalid args detected. Exiting";
        return E_INVALIDARG;
    }

    *sid = nullptr;
    if (mSID.empty())
    {
        qDebug() << __FUNCTION__ << "The SID field is not set. Exiting";
        return E_UNEXPECTED;
    }

    const HRESULT hr(SHStrDupW(&(mSID[0]), sid));
    qDebug() << __FUNCTION__ << "Finishing. Result code:" << hr;
    return hr;
}

HRESULT SimpleCredentialProviderCredentialBase::GetFieldOptions(
        DWORD,
        CREDENTIAL_PROVIDER_CREDENTIAL_FIELD_OPTIONS *)
{
    return E_NOTIMPL;
}

} // ~namespace credential_provider
} // ~namespace CRM
