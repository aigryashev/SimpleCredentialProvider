#pragma once

#include "SimpleCredentialProviderCredentialBase.hpp"

namespace crm
{
namespace credential_provider
{

class SimpleCredentialProviderCredentialSystemUser
        : public SimpleCredentialProviderCredentialBase
{
public:
    SimpleCredentialProviderCredentialSystemUser();
    virtual ~SimpleCredentialProviderCredentialSystemUser() = default;

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

    HRESULT init(
            CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
            const SimpleCredentialProviderFieldDescriptorArray & descriptorArray,
            const SimpleCredentialProviderFieldStatePairArray & fieldStatePairArray,
            ICredentialProviderUserArray * userArray,
            DWORD usersCount) override;
};

} // ~namespace credential_provider
} // ~namespace CRM
