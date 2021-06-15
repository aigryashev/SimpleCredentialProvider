#include <credentialprovider.h>

#include "GUID.hpp"
#include "CommonConsts.hpp"

namespace crm
{
namespace credential_provider
{
namespace consts
{

const std::wstring gGamerUserName(L"Gamer");
const std::wstring gGamerUserPassword(L"");
const std::wstring gSystemUserName(L"Other user");

//GUIDS was generated with MSVS guidgen.exe
const SimpleCredentialProviderFieldDescriptorArray gSimpleCredentialProviderFieldDescriptors
{
    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR{
        static_cast<DWORD>(SimpleCredentialProviderFields::LineEditUserName),
        CPFT_EDIT_TEXT,
        const_cast<wchar_t *>(L"User name"),
        // {F2F85E3F-6A89-4761-A6FB-817253C4AC47}
        GUID{0xf2f85e3f, 0x6a89, 0x4761, {0xa6, 0xfb, 0x81, 0x72, 0x53, 0xc4, 0xac, 0x47}}},

    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR{
        static_cast<DWORD>(SimpleCredentialProviderFields::LineEditPassword),
        CPFT_PASSWORD_TEXT,
        const_cast<wchar_t *>(L"Password"),
        // {F2F85E3F-6A89-4761-A6FB-817253C4AC47}
        GUID{0xf2f85e3f, 0x6a89, 0x4761, {0xa6, 0xfb, 0x81, 0x72, 0x53, 0xc4, 0xac, 0x47}}},

    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR{
        static_cast<DWORD>(SimpleCredentialProviderFields::ButtonSubmit),
        CPFT_SUBMIT_BUTTON,
        const_cast<wchar_t *>(L"Submit"),
        // {1AAC1D6D-7188-4707-BB99-5359745B68C2}
        GUID{0x1aac1d6d, 0x7188, 0x4707, {0xbb, 0x99, 0x53, 0x59, 0x74, 0x5b, 0x68, 0xc2}}},

    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR{
        static_cast<DWORD>(SimpleCredentialProviderFields::TileGamerUser),
        CPFT_TILE_IMAGE,
        const_cast<wchar_t *>(gGamerUserName.c_str()),
        // {C6E54B0A-2ACE-48AE-9D3C-741167490B89}
        GUID{0xc6e54b0a, 0x2ace, 0x48ae, { 0x9d, 0x3c, 0x74, 0x11, 0x67, 0x49, 0xb, 0x89 }}},

    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR{
        static_cast<DWORD>(SimpleCredentialProviderFields::TileSystemUser),
        CPFT_TILE_IMAGE,
        const_cast<wchar_t *>(gSystemUserName.c_str()),
        // {C590D117-F943-44AD-B69C-E0B89FA82D58}
        GUID{ 0xc590d117, 0xf943, 0x44ad, { 0xb6, 0x9c, 0xe0, 0xb8, 0x9f, 0xa8, 0x2d, 0x58 }}}
};

const SimpleCredentialProviderFieldStatePairArray gSimpleCredentialProviderFieldStatePairs
{
    SimpleCredentialProviderFieldStatePair{
        // SimpleCredentialProviderFields::LineEditUserName
        CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_FOCUSED},

    SimpleCredentialProviderFieldStatePair{
        // SimpleCredentialProviderFields::LineEditPassword
        CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_NONE},

    SimpleCredentialProviderFieldStatePair{
        // SimpleCredentialProviderFields::ButtonSubmit
        CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_NONE},

    SimpleCredentialProviderFieldStatePair{
        // SimpleCredentialProviderFields::GamerUser
        CPFS_DISPLAY_IN_BOTH, CPFIS_NONE},

    SimpleCredentialProviderFieldStatePair{
        // SimpleCredentialProviderFields::SystemUser
        CPFS_DISPLAY_IN_BOTH, CPFIS_NONE}
};

} // ~namespace consts
} // ~namespace credential_provider
} // ~namespace crm
