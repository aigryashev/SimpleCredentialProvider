#include "GUID.hpp"

namespace crm
{
namespace credential_provider
{
namespace consts
{
/// Be careful with changing of these values. The should be consistent pair
// {4A06C132-2361-4671-BB25-8F6CF7B883B7}
const GUID gSimpleCredentialProviderGUID{
    0x4a06c132, 0x2361, 0x4671, {0xbb, 0x25, 0x8f, 0x6c, 0xf7, 0xb8, 0x83, 0xb7}};
/// Be careful with changing of these values. The should be consistent pair
extern const std::wstring gSimpleCredentialProviderGUIDStr(L"{4A06C132-2361-4671-BB25-8F6CF7B883B7}");

} // ~namespace consts
} // ~namespace credential_provider
} // ~namespace crm
