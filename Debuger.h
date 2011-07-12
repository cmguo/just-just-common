// Debuger.h

#ifndef _PPBOX_COMMON_DEBUGER_H_
#define _PPBOX_COMMON_DEBUGER_H_

#include <framework/process/MessageQueue.h>
using namespace framework::process;

namespace ppbox
{
    namespace common
    {

        class MsgQueueStream;

        class Debuger
            : public ppbox::common::CommonModuleBase<Debuger>
        {
        public:
            Debuger(
                util::daemon::Daemon & daemon);

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            // �ı� Debug ģʽ
            void change_debug_mode(
                bool bdebug);

            // ��ȡ Debug ��Ϣ
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
        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_DEBUGER_H_
