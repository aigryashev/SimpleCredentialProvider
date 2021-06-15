#pragma once

#include <memory>

#include "ProcessWatchdog.hpp"
#include "SimpleCredentialProviderCredentialBase.hpp"

namespace crm
{
namespace credential_provider
{

class ProcessWatchdog;

class SimpleCredentialProviderCredentialGamer
        : public SimpleCredentialProviderCredentialBase
{
public:
    SimpleCredentialProviderCredentialGamer();
    virtual ~SimpleCredentialProviderCredentialGamer() = default;

    IFACEMETHODIMP GetBitmapValue(
            DWORD fieldID,
            HBITMAP * outBitmap) override;

    IFACEMETHODIMP GetSerialization(
            CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE * getSerializationResponse,
            CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * serialization,
            PWSTR * optionalStatusText,
            CREDENTIAL_PROVIDER_STATUS_ICON * optionalStatusIcon) override;

    IFACEMETHODIMP GetFieldOptions(
            DWORD fieldID,
            CREDENTIAL_PROVIDER_CREDENTIAL_FIELD_OPTIONS * pcpcfo) override;

    IFACEMETHODIMP SetSelected(
            BOOL * autoLogon) override;

    HRESULT init(
            CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
            const SimpleCredentialProviderFieldDescriptorArray & descriptorArray,
            const SimpleCredentialProviderFieldStatePairArray & fieldStatePairArray,
            ICredentialProviderUserArray * userArray,
            DWORD usersCount) override;

private:
   static bool isCRMUser(
           const std::wstring & enteredUserName,
           const std::wstring & enteredPassword,
           std::string & outUTF8UserName);
   static bool deleteCRMUserName();

private:
   ProcessWatchdog mUIProcessWatchdog;
};

} // ~namespace credential_provider
} // ~namespace CRM
