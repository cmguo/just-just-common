// PortManager.cpp

#include "just/common/Common.h"
#include "just/common/PortManager.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.common.PortManager", framework::logger::Debug);

#define PORTMANAGE_OBJECT_ID 107

namespace just
{
    namespace common
    {
        PortManager::PortManager(
            util::daemon::Daemon & daemon)
            : just::common::CommonModuleBase<PortManager>(daemon, "PortManager")
        {
            ports_ = (PortContainer *)shared_memory().alloc_with_id(PORTMANAGE_OBJECT_ID, sizeof(PortContainer));
            if (ports_) 
            {
                new (ports_) PortContainer();
            } 
            else 
            {
                ports_ = (PortContainer *)shared_memory().get_by_id(PORTMANAGE_OBJECT_ID);
            }
        }

        PortManager::~PortManager()
        {
        }

        bool PortManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool PortManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        boost::system::error_code PortManager::get_port(ModuleType type,boost::uint16_t& port)
        {
            boost::system::error_code ec;
            port = 0;
            switch(type)
            {
            case live:
                {
                    port = ports_->liveport;
                }
                break;
            case vod:
                {
                    port = ports_->vodport;
                }
                break;
            case alive:
                {
                    port = ports_->aliveport;
                }
                break;
            default:
                break;
            }
            return ec;
        }
        boost::system::error_code PortManager::set_port(ModuleType type,boost::uint16_t port)
        {
            boost::system::error_code ec;
            switch(type)
            {
            case live:
                {
                    ports_->liveport = port;
                }
                break;
            case vod:
                {
                    ports_->vodport = port;
                }
                break;
            case alive:
                {
                    ports_->aliveport = port;
                }
                break;
            default:
                break;
            }
            return ec;
        }
    }
}
