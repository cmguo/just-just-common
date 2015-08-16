// CommonModule.cpp

#include "just/common/Common.h"
#include "just/common/CommonModule.h"
#include "just/common/ConfigMgr.h"

#include <framework/process/MessageQueue.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/configure/Config.h>
using namespace framework::process;
using namespace framework::configure;


FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.common.CommonModule", framework::logger::Debug)

namespace just
{
    namespace common
    {

        static framework::memory::SharedMemory & open_shm(
            framework::memory::SharedMemory & shm)
        {
            boost::system::error_code ec;
            framework::memory::SharedMemory::remove(SHARED_MEMORY_INST_ID);
            if (shm.open(framework::memory::SharedMemory::no_user_id, 
#if defined( JUST_SINGLE_PROCESS )
                framework::memory::SharedMemory::private_ 
#else
                framework::memory::SharedMemory::default_ 
#endif
                | framework::memory::SharedMemory::create 
                | framework::memory::SharedMemory::read_write, ec)) {
                    LOG_WARN("SharedMemory open: " << ec.message());
            }
            return shm;
        }

        CommonModule::CommonModule(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CommonModule>(daemon, "CommonModule")
            , shm_(SHARED_MEMORY_INST_ID)
            , tmgr_(io_svc(), boost::posix_time::milliseconds(500))
            , msg_queue_("CommonModule", open_shm(shm_))
        {
//            assert(0);
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

        bool CommonModule::startup(
            boost::system::error_code & ec)
        {
            tmgr_.start();
            return true;
        }

        bool CommonModule::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.stop();
            return true;
        }

        void CommonModule::set_version(
            framework::system::Version const & version)
        {
            env_.version = version;
        }

    } // namespace common
} // namespace just
