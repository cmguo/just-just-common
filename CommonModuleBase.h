// CommonModuleBase.h

#ifndef _JUST_COMMON_COMMON_MODULE_BASE_H_
#define _JUST_COMMON_COMMON_MODULE_BASE_H_

#include "just/common/CommonModule.h"

namespace just
{
    namespace common
    {

        template <
            typename ModuleType
        >
        class CommonModuleBase
            : public util::daemon::ModuleBase<CommonModuleBase<ModuleType> >
        {
        public:
            CommonModuleBase(
                util::daemon::Daemon & daemon, 
                std::string const & name = "")
                : util::daemon::ModuleBase<CommonModuleBase<ModuleType> >(daemon, name)
                , common_(util::daemon::use_module<CommonModule>(daemon))
            {
            }

        public:
            just::common::CommonModule & common()
            {
                return common_;
            }

            Environment const & environment() const
            {
                return common_.environment();
            }
            framework::memory::SharedMemory & shared_memory()
            {
                return common_.shared_memory();
            }

            framework::timer::TimerQueue & timer_queue()
            {
                return common_.timer_queue();
            }

            framework::process::MessageQueue & msg_queue()
            {
                return common_.msg_queue();
            }

        private:
            just::common::CommonModule & common_;
        };

    } // namespace common
} // namespace just

#endif // _JUST_COMMON_COMMON_MODULE_H_
