#pragma once

#include <credentialprovider.h>

#include <string>
#include <memory>
#include <unordered_map>

#include "SimpleCredentialProviderFields.hpp"
#include "utils/ReleaserForIUnknown.hpp"

namespace crm
{
namespace credential_provider
{

class SimpleCredentialProviderCredentialBase
        : public ICredentialProviderCredential2
        , public ICredentialProviderCredentialWithFieldOptions
{
public:
    SimpleCredentialProviderCredentialBase();
    virtual ~SimpleCredentialProviderCredentialBase();

    // IUnknown
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;
    IFACEMETHODIMP QueryInterface(
            REFIID riid,
            void ** ppv) override;

    // ICredentialProviderCredential
    IFACEMETHODIMP Advise(
            ICredentialProviderCredentialEvents * credentialEvents) override;
    IFACEMETHODIMP UnAdvise() override;

    IFACEMETHODIMP SetSelected(
            BOOL * autoLogon) override;
    IFACEMETHODIMP SetDeselected() override;

    IFACEMETHODIMP GetFieldState(
            DWORD fieldID,
            CREDENTIAL_PROVIDER_FIELD_STATE * fieldState,
            CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE * fieldInteractiveState) override;

    IFACEMETHODIMP GetStringValue(
            DWORD fieldID,
            PWSTR * outString) override;
    /// NOTE: You should implement the method in the inheritors to override the default icon.
    /// The current implementation of the method returns E_NOTIMPL
    IFACEMETHODIMP GetBitmapValue(
            DWORD fieldID,
            HBITMAP * outBitmap) override;
    IFACEMETHODIMP GetCheckboxValue(
            DWORD fieldID,
            BOOL * checked,
            PWSTR * label) override;
    IFACEMETHODIMP GetComboBoxValueCount(
            DWORD,
            DWORD *,
            DWORD *) override;
    IFACEMETHODIMP GetComboBoxValueAt(
            DWORD,
            DWORD,
            PWSTR *) override;
    IFACEMETHODIMP GetSubmitButtonValue(
            DWORD fieldID,
            DWORD * adjacentTo) override;

    IFACEMETHODIMP SetStringValue(
            DWORD fieldID,
            PCWSTR value) override;
    IFACEMETHODIMP SetCheckboxValue(
            DWORD fieldID,
            BOOL checked) override;
    IFACEMETHODIMP SetComboBoxSelectedValue(
            DWORD fieldID,
            DWORD selectedItem) override;
    IFACEMETHODIMP CommandLinkClicked(
            DWORD fieldID) override;

    /// NOTE: the method returns E_NOTIMPL override it in inheritors
    IFACEMETHODIMP GetSerialization(
            CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *,
            CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *,
            PWSTR *,
            CREDENTIAL_PROVIDER_STATUS_ICON *) override;
    IFACEMETHODIMP ReportResult(
            NTSTATUS status,
            NTSTATUS substatus,
            PWSTR * optionalStatusText,
            CREDENTIAL_PROVIDER_STATUS_ICON * optionalStatusIcon) override;

    // ICredentialProviderCredential2
    IFACEMETHODIMP GetUserSid(
            PWSTR * sid) override;

    // ICredentialProviderCredentialWithFieldOptions
    /// NOTE: the method returns E_NOTIMPL override it in inheritors
    IFACEMETHODIMP GetFieldOptions(
            DWORD fieldID,
            CREDENTIAL_PROVIDER_CREDENTIAL_FIELD_OPTIONS * pcpcfo) override;

    virtual HRESULT init(
            CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
            const SimpleCredentialProviderFieldDescriptorArray & descriptorArray,
            const SimpleCredentialProviderFieldStatePairArray & fieldStatePairArray,
            ICredentialProviderUserArray * userArray,
            DWORD usersCount);

    struct CustomReleaser {
        void operator()(SimpleCredentialProviderCredentialBase * objectPtr) const
        {
            if (objectPtr)
                objectPtr->Release();
        }
    };

private:
    LONG mReferencesCounter;

protected:
    CREDENTIAL_PROVIDER_USAGE_SCENARIO mCPUS;
    SimpleCredentialProviderFieldDescriptorArray mCredentialProviderFieldDescriptors;
    SimpleCredentialProviderFieldStatePairArray mCredentialProviderFieldStatePairs;
    std::unordered_map<DWORD, std::wstring> mFieldStrings;
    std::unordered_map<std::wstring, bool> mQualifiedUserNames;
    std::wstring mSID;
    using ICredentialProviderCredentialEvents2UPtr =
        std::unique_ptr<ICredentialProviderCredentialEvents2, utils::ReleaserForIUnknown>;
    ICredentialProviderCredentialEvents2UPtr mCredentialEvents;
    DWORD mSelectedComboBoxIndex;
    bool mShowControls;
};

} // ~namespace credential_provider
} // ~namespace CRM
