#pragma once

#include <credentialprovider.h>

#include <array>
#include <memory>

#include "utils/ReleaserForIUnknown.hpp"
#include "SimpleCredentialProviderCredentialBase.hpp"
#include "SimpleCredentialProviderBuiltInUsers.hpp"

namespace crm
{
namespace credential_provider
{

class SimpleCredentialProvider : public ICredentialProvider,
        public ICredentialProviderSetUserArray,
        public ICredentialProviderUserArray
{
public:
    SimpleCredentialProvider();
    virtual ~SimpleCredentialProvider();

    /// IUnknown

    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;
    IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv) override;

    /// ICredentialProvider

    IFACEMETHODIMP Advise(ICredentialProviderEvents *, UINT_PTR upAdviseContext) override;
    IFACEMETHODIMP UnAdvise() override;
    IFACEMETHODIMP GetCredentialAt(
            DWORD index,
            ICredentialProviderCredential ** credentials) override;
    IFACEMETHODIMP SetUserArray(
            ICredentialProviderUserArray * users) override;
    IFACEMETHODIMP GetCredentialCount(
            DWORD * count,
            DWORD * defaultCount,
            BOOL * autoLogonWithDefault) override;
    IFACEMETHODIMP GetFieldDescriptorAt(
            DWORD index,
            CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR ** fieldDescriptor) override;
    IFACEMETHODIMP GetFieldDescriptorCount(
            DWORD * count) override;
    IFACEMETHODIMP SetSerialization(
            const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *) override;
    IFACEMETHODIMP SetUsageScenario(
            CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
            DWORD) override;

    friend HRESULT SimpleCredentialProvider_CreateInstance(REFIID riid, void ** ppv);

    /// ICredentialProviderUserArray
    IFACEMETHODIMP GetAccountOptions(CREDENTIAL_PROVIDER_ACCOUNT_OPTIONS *) override;
    IFACEMETHODIMP GetAt(DWORD userIndex, ICredentialProviderUser ** user) override;
    IFACEMETHODIMP GetCount(DWORD * userCount) override;
    IFACEMETHODIMP SetProviderFilter(REFGUID guidProviderToFilterTo) override;

protected:
    void resetEnumeratedCredentials();
    void initEnumeratedCredentials();
    void enumerateCredentials();

private:
    LONG mReferencesCounter;
    CREDENTIAL_PROVIDER_USAGE_SCENARIO mCPUS;
    bool mRecreateEnumeratedCredentials;

    using SimpleCredentialProviderCredentialBaseUPtr =
        std::unique_ptr<SimpleCredentialProviderCredentialBase, SimpleCredentialProviderCredentialBase::CustomReleaser>;
    using SimpleCredentialProviderBuiltInCredentialsArray =
        std::array<SimpleCredentialProviderCredentialBaseUPtr, gSimpleCredentialProviderBuiltInUsersCount>;
    SimpleCredentialProviderBuiltInCredentialsArray mBuiltInCredentials;

    using ICredentialProviderUserArrayUPtr =
        std::unique_ptr<ICredentialProviderUserArray, utils::ReleaserForIUnknown>;
    ICredentialProviderUserArrayUPtr mCredentialProviderUserArray;

    ICredentialProviderUser * mGamerUser;

    using SimpleCredentialProviderUserUPtr =
        std::unique_ptr<ICredentialProviderUser, utils::ReleaserForIUnknown>;
    SimpleCredentialProviderUserUPtr mSystemUser;
};

} // ~namespace credential_provider
} // ~namespace CRM
