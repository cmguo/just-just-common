// Debuger.h

#ifndef _PPBOX_COMMON_DEBUGER_H_
#define _PPBOX_COMMON_DEBUGER_H_

#include <framework/logger/LogMsgStream.h>
using namespace framework::logger;

#include <framework/process/MessageQueue.h>
using namespace framework::process;

namespace ppbox
{
    namespace common
    {

        typedef void ( * on_logdump_type )( char const *, boost::uint32_t );
        class OuterLogStream
            : public IWriteStream
        {
        public:
            OuterLogStream(
                on_logdump_type callback, boost::uint32_t level)
            : callback_( callback )
            {
            }

            void set_log_dump( on_logdump_type callback, boost::uint32_t level )
            {
                callback_ = callback;
            }

            virtual void write(
                char const * logmsg)
            {
                if ( !logmsg ) return;
                callback_( logmsg, strlen( logmsg ) );
            }

        private:
            on_logdump_type callback_;
        };

        extern OuterLogStream & outerLogStream;

        class MsgQueueStream;

        class Debuger
            : public ppbox::common::CommonModuleBase<Debuger>
        {
        public:
            Debuger(
                util::daemon::Daemon & daemon);

            ~Debuger();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            // 改变 Debug 模式
            void change_debug_mode(
                bool bdebug);

            // 获取 Debug 信息
            void get_debug_msg(
                MessageList & msgs, 
                boost::int32_t size, 
                char const * module, 
                boost::int32_t level);

        private:
            void check_debug_mode();

            void handle_timer();

        private:
            boost::uint32_t * debug_mode_;
            MsgQueueStream * debug_log_stream_;
            framework::timer::Timer * timer_;
            framework::process::MessageQueue msg_queue_;
            bool out_streamed;
        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_DEBUGER_H_
