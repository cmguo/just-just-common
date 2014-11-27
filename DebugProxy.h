// DebugProxy.h

#ifndef _JUST_COMMON_DEBUG_PROXY_H_
#define _JUST_COMMON_DEBUG_PROXY_H_

#include <util/protocol/http/HttpSocket.h>

#include <framework/network/Acceptor.h>

namespace just
{
    namespace common
    {

        class Debuger;

        class DebugProxy
            : public util::daemon::ModuleBase<DebugProxy>
        {
        public:
            DebugProxy(
                util::daemon::Daemon & daemon);

            ~DebugProxy();

        private:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        private:
            void start();

        private:
            Debuger & debuger_;
            framework::network::NetName addr_;
            util::protocol::HttpSocket socket_;
            framework::network::Acceptor acceptor_;
            boost::thread * work_thread_;
            bool exit_;
            boost::mutex mutex_;
        };

    } // namespace common
} // namespace just

#endif // _JUST_COMMON_DEBUG_PROXY_H_
