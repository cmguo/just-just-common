// RedirectCall.cpp

#include "just/common/Common.h"
#include "just/common/RedirectCall.h"

namespace just
{
    namespace common
    {

        RedirectCall::RedirectCall(
            call_t call, 
            free_t free, 
            context_t context)
            : call_(call)
            , free_(free)
            , context_(context)
        {
        }

        void RedirectCall::set(
            call_t call, 
            free_t free, 
            context_t context)
        {
            call_ = call;
            free_ = free;
            context_ = context;
        }

    } // namespace common
} // namespace just
