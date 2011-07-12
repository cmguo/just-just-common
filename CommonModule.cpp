// CommonModule.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/CommonModule.h"
#include "ppbox/common/ConfigMgr.h"

#include <framework/process/MessageQueue.h>
#include <framework/logger/LoggerStreamRecord.h>
#include <framework/configure/Config.h>
#include <framework/logger/LogMsgStream.h>
using namespace framework::logger;
using namespace framework::process;
using namespace framework::configure;


FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("CommonModule", 0)

namespace ppbox
{
    namespace common
    {

        CommonModule::CommonModule(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CommonModule>(daemon, "CommonModule")
            , shm_(SHARED_MEMORY_INST_ID)
            , tmgr_(io_svc(), boost::posix_time::milliseconds(500))
            , msg_queue_("CommonModule", shm_)
        {
            assert(0);
        }

        static framework::memory::SharedMemory & open_shm(
            framework::memory::SharedMemory & shm)
        {
            boost::system::error_code ec;
            framework::memory::SharedMemory::remove(SHARED_MEMORY_INST_ID);
            if (shm.open(framework::memory::SharedMemory::no_user_id, 
                framework::memory::SharedMemory::create 
                | framework::memory::SharedMemory::read_write, ec)) {
                    LOG_S(Logger::kLevelAlarm, "SharedMemory open: " << ec.message());
            }
            return shm;
        }

        CommonModule::CommonModule(
            util::daemon::Daemon & daemon, 
            std::string const & name)
            : util::daemon::ModuleBase<CommonModule>(daemon, "CommonModule")
            , shm_(SHARED_MEMORY_INST_ID)
            , tmgr_(io_svc(), boost::posix_time::milliseconds(500))
            , msg_queue_(name, open_shm(shm_))
        {
        }

        CommonModule::~CommonModule()
        {
            boost::system::error_code ec;
            shm_.close(ec);
        }

        boost::system::error_code CommonModule::startup()
        {
            tmgr_.start();
            return boost::system::error_code();
        }

        void CommonModule::shutdown()
        {
            tmgr_.stop();
        }

        void CommonModule::set_version(
            framework::system::Version const & version)
        {
            env_.version = version;
        }

    } // namespace common
} // namespace ppbox
