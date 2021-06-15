#pragma once

#include <credentialprovider.h>

#include <array>

namespace crm
{
namespace credential_provider
{

enum class SimpleCredentialProviderFields : DWORD
{
    LineEditUserName,
    LineEditPassword,
    ButtonSubmit,
    TileGamerUser,
    TileSystemUser,

    // This is the last element and should be placed as last element of this enum
    FieldsCount
};

constexpr std::size_t getSimpleCredentialProviderFieldsEnumSize()
{
    return std::size_t(SimpleCredentialProviderFields::FieldsCount);
}

template<typename T>
using SimpleCredentialProviderTArray = std::array<T, getSimpleCredentialProviderFieldsEnumSize()>;

using SimpleCredentialProviderFieldDescriptorArray =
    SimpleCredentialProviderTArray<CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR>;

struct SimpleCredentialProviderFieldStatePair
{
    CREDENTIAL_PROVIDER_FIELD_STATE state;
    CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE interactiveState;
};
using SimpleCredentialProviderFieldStatePairArray =
    SimpleCredentialProviderTArray<SimpleCredentialProviderFieldStatePair>;

} // ~namespace credential_provider
} // ~namespace CRM
