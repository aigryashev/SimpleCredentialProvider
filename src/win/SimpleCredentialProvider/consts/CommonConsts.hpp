#pragma once

#include <string>
#include <unordered_map>

#include "SimpleCredentialProviderFields.hpp"

namespace crm
{
namespace credential_provider
{
namespace consts
{

extern const std::wstring gGamerUserName;
extern const std::wstring gGamerUserPassword;
extern const std::wstring gSystemUserName;

extern const SimpleCredentialProviderFieldDescriptorArray gSimpleCredentialProviderFieldDescriptors;
extern const SimpleCredentialProviderFieldStatePairArray gSimpleCredentialProviderFieldStatePairs;

constexpr int gMaxSupportedInt(std::numeric_limits<int>::max());

} // ~namespace consts
} // ~namespace credential_provider
} // ~namespace crm
