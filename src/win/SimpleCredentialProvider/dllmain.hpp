#pragma once

#include <Windows.h>

namespace crm
{
namespace credential_provider
{

extern HINSTANCE gDllInstance;

void dllAddRef();
void dllRelease();

} // ~namespace credential_provider
} // ~namespace CRM
