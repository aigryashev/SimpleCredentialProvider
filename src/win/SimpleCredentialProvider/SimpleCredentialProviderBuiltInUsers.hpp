#pragma once

#include <minwindef.h>

namespace crm
{
namespace credential_provider
{

enum class SimpleCredentialProviderBuiltInUsers : DWORD
{
    Gamer = 0,
    SystemUser,

    // This is the last element and should be placed as last element of this enum
    Count
};

constexpr std::size_t gSimpleCredentialProviderBuiltInUsersCount(
        static_cast<std::size_t>(SimpleCredentialProviderBuiltInUsers::Count));

} // ~namespace credential_provider
} // ~namespace CRM
