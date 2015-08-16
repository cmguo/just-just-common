// Debuger.h

#ifndef _JUST_COMMON_DEBUGER_H_
#define _JUST_COMMON_DEBUGER_H_

#include <framework/process/MessageQueue.h>

namespace just
{
    namespace common
    {

        class HookLogStream;

        class MsgQueueStream;

        class Debuger
            : public just::common::CommonModuleBase<Debuger>
        {
        public:
            typedef void (* on_logdump_type)(
                char const *, 
                boost::uint32_t);

        public:
            Debuger(
                util::daemon::Daemon & daemon);

            ~Debuger();

        public:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        public:
            // �ı� Debug ģʽ
            void change_debug_mode(
                bool bdebug);

            // ��ȡ Debug ��Ϣ
            void get_debug_msg(
                std::vector<framework::process::Message> & msgs, 
                boost::int32_t size, 
                char const * module, 
                boost::int32_t level);

            void set_log_hook(
                on_logdump_type hook, 
                size_t level);

        private:
            void check_debug_mode();

            void handle_timer();

        private:
            boost::uint32_t * debug_mode_;
            MsgQueueStream * debug_log_stream_;
            HookLogStream * hook_log_stream_;
            framework::timer::Timer * timer_;
            framework::process::MessageQueue msg_queue_;
            bool out_streamed_;
            bool hook_streamed_;
        };

    } // namespace common
} // namespace just

#endif // _JUST_COMMON_DEBUGER_H_
