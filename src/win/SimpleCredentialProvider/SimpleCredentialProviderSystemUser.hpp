#pragma once

#include <unordered_map>

#include <credentialprovider.h>

namespace crm
{
namespace credential_provider
{

class SimpleCredentialProviderSystemUser
        : public ICredentialProviderUser
{
public:
    SimpleCredentialProviderSystemUser() = default;
    virtual ~SimpleCredentialProviderSystemUser() = default;

    /// IUnknown
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;
    IFACEMETHODIMP QueryInterface(
            REFIID riid,
            void ** ppv) override;

    /// ICredentialProviderUser
    IFACEMETHODIMP GetProviderID(GUID * providerID) override;
    IFACEMETHODIMP GetSid(LPWSTR * sid) override;
    IFACEMETHODIMP GetStringValue(REFPROPERTYKEY key, LPWSTR * stringValue) override;
    IFACEMETHODIMP GetValue(REFPROPERTYKEY key, PROPVARIANT * value) override;

private:
    LONG mReferencesCounter;
};

} // ~namespace credential_provider
} // ~namespace CRM
