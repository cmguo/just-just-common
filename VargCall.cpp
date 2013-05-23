// VargCall.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/VargCall.h"

namespace ppbox
{
    namespace common
    {

        VargCall::VargCall(
            call_t call, 
            free_t free, 
            context_t context)
            : call_(call)
            , free_(free)
            , context_(context)
        {
        }

        void VargCall::set(
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
