// CommonModule.h

#ifndef _JUST_COMMON_COMMON_MODULE_H_
#define _JUST_COMMON_COMMON_MODULE_H_

#include "just/common/Environment.h"

#define SHARED_MEMORY_INST_ID MESSAGE_QUEUE_SHARED_MEMORY_INST_ID

#include <util/daemon/Module.h>
#include <util/daemon/Daemon.h>

#include <framework/memory/SharedMemory.h>
#include <framework/timer/AsioTimerManager.h>
#include <framework/process/MessageQueue.h>

namespace framework
{
    namespace process
    {
        class MessageQueue;
    }
}

namespace just
{
    namespace common
    {

        class ConfigMgr;

        class CommonModule
            : public util::daemon::ModuleBase<CommonModule>
        {
        public:
            CommonModule(
                util::daemon::Daemon & daemon);

            CommonModule(
                util::daemon::Daemon & daemon, 
                std::string const & name);

            ~CommonModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            void set_version(
                framework::system::Version const & version);

        public:
            Environment const & environment() const
            {
                return env_;
            }

            framework::memory::SharedMemory & shared_memory()
            {
                return shm_;
            }

            framework::timer::TimerQueue & timer_queue()
            {
                return tmgr_;
            }

            framework::process::MessageQueue & msg_queue()
            {
                return msg_queue_;
            }

        private:
            Environment env_;
            framework::memory::SharedMemory shm_;
            framework::timer::AsioTimerManager tmgr_;
            framework::process::MessageQueue msg_queue_;
        };

    } // namespace common
} // namespace just

#endif // _JUST_COMMON_COMMON_MODULE_H_
