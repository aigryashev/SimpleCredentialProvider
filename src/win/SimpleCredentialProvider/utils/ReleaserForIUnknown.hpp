#pragma once

namespace crm
{
namespace credential_provider
{
namespace utils
{

struct ReleaserForIUnknown {
    void operator()(IUnknown * iUnknown) const
    {
        if (iUnknown)
            iUnknown->Release();
    }
};

} // ~namespace utils
} // ~namespace credential_provider
} // ~namespace CRM
