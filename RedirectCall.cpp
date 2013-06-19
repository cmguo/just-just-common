// RedirectCall.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/RedirectCall.h"

namespace ppbox
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
} // namespace ppbox
