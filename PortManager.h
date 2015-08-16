// PortManager.h

#ifndef _JUST_COMMON_PORTMANAGER_H_
#define _JUST_COMMON_PORTMANAGER_H_

#include <just/common/CommonModuleBase.h>
#include <boost/asio/io_service.hpp>



namespace just
{
    namespace common
    {
        enum ModuleType
        {
            live,
            vod,
            alive
        };

        class PortManager
            : public just::common::CommonModuleBase<PortManager>
        {
        public:
            PortManager(
                util::daemon::Daemon & daemon);
            ~PortManager();

            boost::system::error_code get_port(ModuleType type,boost::uint16_t& port);
            boost::system::error_code set_port(ModuleType type,boost::uint16_t port);
        private:
            struct PortContainer
            {
                PortContainer()
                    : liveport(0)
                    , vodport(0)
                    , aliveport(0)
                {
                }
                boost::uint16_t liveport;
                boost::uint16_t vodport;
                boost::uint16_t aliveport;
            };
        private:
            PortContainer* ports_;
        public:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);
        };

    } // namespace PortManager
} // namespace just

#endif    // _JUST_COMMON_PORTMANAGER_H_
