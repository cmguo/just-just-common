// Debuger.cpp

#include "just/common/Common.h"
#include "just/common/Debuger.h"

#include <util/buffers/BuffersCopy.h>

#include <framework/timer/Timer.h>
#include <framework/timer/TimeCounter.h>
#include <framework/string/Format.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/Stream.h>
using namespace framework::string;
using namespace framework::process;

#include <boost/bind.hpp>
using namespace boost::system;

#define MSGTYPEDEBUG    3
#define MSGTYPELOG      2
#define MSGLEVEL        0

#define DEBUG_OBJECT_ID 102

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.common.Debuger", framework::logger::Debug)

namespace just
{
    namespace common
    {

        class HookLogStream
            : public framework::logger::Stream
        {
        public:
            HookLogStream()
                : callback_(NULL)
            {
            }

            void set_log_dump(
                Debuger::on_logdump_type callback, 
                boost::uint32_t level)
            {
                callback_ = callback;
                level_ = level;
            }

            virtual void write(
                buffers_t const & buffers)
            {
                if (!callback_)
                    return;
                size_t size = util::buffers::buffers_copy(
                    boost::asio::buffer((void *)buf_, sizeof(buf_) - 1), 
                    buffers);
                buf_[size] = 0;
                callback_(buf_, size);
            }

        private:
            Debuger::on_logdump_type callback_;
            char buf_[1024];
        };

        // д����Ϣ����
        class MsgQueueStream 
            : public framework::logger::Stream
        {
        public:
            MsgQueueStream(
                framework::process::MessageQueue & msgqueue)
                : m_msgqueue_(msgqueue)
            {
            }

            virtual void write(
                buffers_t const & buffers)
            {
                framework::process::Message msg;
                parse_log(msg, buffers);
                if (!msg.receiver.empty()) {
                    m_msgqueue_.push(msg);
                }
            }

            // ������־
            void parse_log(
                framework::process::Message & msg, 
                buffers_t const & buffers)
            {
                msg.level = 1;
                msg.receiver = "Debuger";
                msg.type = MSGTYPELOG;
                msg.data.resize(1024);
                size_t size = util::buffers::buffers_copy(
                    boost::asio::buffer(&msg.data[0], msg.data.size()), 
                    buffers);
                msg.data.resize(size);
            }

            virtual ~MsgQueueStream()
            {
            }

        private:
            framework::process::MessageQueue & m_msgqueue_;
        };

        Debuger::Debuger(
            util::daemon::Daemon & daemon)
            : just::common::CommonModuleBase<Debuger>(daemon, "Debuger")
            , debug_log_stream_(NULL)
            , hook_log_stream_(NULL)
            , timer_(NULL)
            , msg_queue_("Debuger", shared_memory())
            , out_streamed_(false)
            , hook_streamed_(false)
        {
            debug_mode_ = (boost::uint32_t *)shared_memory().alloc_with_id(DEBUG_OBJECT_ID, sizeof(boost::uint32_t));
            if (debug_mode_) {
                new (debug_mode_) boost::uint32_t(0);
            } else {
                debug_mode_ = (boost::uint32_t *)shared_memory().get_by_id(DEBUG_OBJECT_ID);
            }

            check_debug_mode();
        }

        Debuger::~Debuger()
        {
            if (hook_log_stream_) {
                delete hook_log_stream_;
                hook_log_stream_ = NULL;
            }
            if (debug_log_stream_) {
                delete debug_log_stream_;
                debug_log_stream_ = NULL;
            }
        }

        error_code Debuger::startup()
        {
            error_code ec;
            timer_ = new framework::timer::PeriodicTimer(
                timer_queue(), 1000, boost::bind(&Debuger::handle_timer, this));
            timer_->start();
            check_debug_mode();
            return ec;
        }

        // ֹͣ����(keepalive)
        void Debuger::shutdown()
        {
            timer_->stop();
            delete timer_;
            timer_ = NULL;

            if (hook_streamed_) {
                framework::logger::del_stream(*hook_log_stream_);
            }

            if (out_streamed_) {
                LOG_INFO("[shutdown] leave debug mode");
                framework::logger::del_stream(*debug_log_stream_);
            }
       }

        void Debuger::get_debug_msg(
            std::vector<Message> & msgs, 
            boost::int32_t size, 
            char const * module, 
            boost::int32_t level)
        {
            Message msg;
            msg.type = MSGTYPELOG;
            msg.level = level;
            if (module)
                msg.sender = module;
            msg_queue_.pop(msgs, msg, size);
        }

        void Debuger::set_log_hook(
            on_logdump_type hook, 
            size_t level)
        {
            if (hook == NULL) {
                if (hook_log_stream_) {
                    framework::logger::del_stream(*hook_log_stream_);
                    hook_log_stream_->set_log_dump(hook, 0);
                }
                hook_streamed_ = false;
            } else {
                if (!hook_log_stream_) {
                    hook_log_stream_ = new HookLogStream;
                }
                hook_log_stream_->set_log_dump(hook, level);
                hook_streamed_ = true;
                framework::logger::add_stream(*hook_log_stream_);
            }
        }

        void Debuger::change_debug_mode(
            bool bdebug)
        {
            if (bdebug) {
                //if (*debug_mode_ == 1)
                //    return;
                *debug_mode_ = 1;
            } else {
                *debug_mode_ = 0;
            }

            msg_queue_.clear();

            Message msg_push;
            msg_push.level = MSGLEVEL;
            msg_push.type = MSGTYPEDEBUG;
            boost::uint32_t number = msg_queue_.push(msg_push);

            size_t time_out = 30;
            while (number != 0 && time_out != 0) {
                Message msg_pop;
                msg_pop.level = MSGLEVEL;
                msg_pop.type = MSGTYPEDEBUG;
                if (msg_queue_.pop(msg_pop)) {
                    //std::cout << "[change_debug_mode] msg pop sender:" << msg_pop.sender << std::endl;
                    --number;
                } else {
                    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                    --time_out;
                }
            }
        }

        //�ڲ�����
        void Debuger::check_debug_mode()
        {
            if (debug_mode_) {
                if (*debug_mode_ == 1) {
                    if (!debug_log_stream_) {
                        debug_log_stream_ = new MsgQueueStream(msg_queue());
                    }
                    out_streamed_ = true;
                    framework::logger::add_stream(*debug_log_stream_);
                    LOG_INFO("[check_debug_mode] enter debug mode");
                } else  if (*debug_mode_ == 0 && debug_log_stream_) {
                    LOG_INFO("[check_debug_mode] leave debug mode");
                    out_streamed_ = false;
                    framework::logger::del_stream(*debug_log_stream_);
                }
            }
        }

        void Debuger::handle_timer()
        {
            Message msg_pop;
            msg_pop.level = MSGLEVEL;
            msg_pop.type = MSGTYPEDEBUG;
            if (msg_queue().pop(msg_pop)) {
                check_debug_mode();

                Message msg_push;
                msg_push.level = MSGLEVEL;
                msg_push.type = MSGTYPEDEBUG;
                msg_push.receiver = msg_pop.sender;
                msg_queue().push(msg_push);

                LOG_DEBUG("[handle_timer] msg: data = " << msg_pop.data 
                    << ", sender = " << msg_pop.sender
                    << ", receiver = " << msg_pop.receiver 
                    << ", level = " << msg_pop.level);
            }
        }

    } // namespace common
} // namespace just
