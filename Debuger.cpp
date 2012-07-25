// Debuger.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/Debuger.h"

#include <framework/timer/Timer.h>
#include <framework/timer/TimeCounter.h>
#include <framework/string/Format.h>
#include <framework/logger/LoggerStreamRecord.h>
using namespace framework::string;

#include <boost/bind.hpp>
using namespace boost::system;

#define MSGTYPEDEBUG    3
#define MSGTYPELOG      2
#define MSGLEVEL        0

#define DEBUG_OBJECT_ID 102

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("Debuger", 0)

namespace ppbox
{
    namespace common
    {

        OuterLogStream & getOuterLogStream()
        {
            static OuterLogStream ols( NULL, 0 );
            return ols;
        }

        OuterLogStream & outerLogStream = getOuterLogStream();

        // 写入消息队列
        class MsgQueueStream 
            : public IWriteStream
        {
        public:
            MsgQueueStream(
                framework::process::MessageQueue & msgqueue)
                : m_msgqueue_(msgqueue)
            {
            }

            virtual void write(
                char const * logmsg)
            {
                framework::process::Message msg;
                parse_log(logmsg, msg);
                if (!msg.receiver.empty()) {
                    m_msgqueue_.push(msg);
                }
            }

            // 解析日志
            void parse_log(
                std::string const & log, 
                framework::process::Message & msg)
            {
                if (log.empty()) return;

                std::string::size_type q = log.find('[');
                if (q == std::string::npos) return;
                std::string::size_type r = log.find(']', q);
                if (r == std::string::npos) return;
                std::string level = log.substr(q + 1, r - 1);
                if ("ERROR" == level) msg.level = 1;
                else if ("ALARM" == level) msg.level = 2;
                else if ("EVENT" == level) msg.level = 3;
                else if ("INFOR" == level) msg.level = 4;
                else if ("DEBUG" == level) msg.level = 5;
                else return;

                msg.receiver = "Debuger";
                msg.type = MSGTYPELOG;
                msg.data = log;
            }

            virtual ~MsgQueueStream()
            {
            }

        private:
            framework::process::MessageQueue & m_msgqueue_;
        };

        Debuger::Debuger(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<Debuger>(daemon, "Debuger")
            , debug_log_stream_(NULL)
            , timer_(NULL)
            , msg_queue_("Debuger", shared_memory())
            , out_streamed(false)
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

        // 停止心跳(keepalive)
        void Debuger::shutdown()
        {
            timer_->stop();
            delete timer_;
            timer_ = NULL;

            if (out_streamed) {
                LOG_S(Logger::kLevelEvent, "[shutdown] leave debug mode");
                global_logger().del_stream(debug_log_stream_);
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

        //内部调用
        void Debuger::check_debug_mode()
        {
            if (debug_mode_) {
                if (*debug_mode_ == 1) {
                    if (!debug_log_stream_) {
                        debug_log_stream_ = new MsgQueueStream(msg_queue());
                    }
                    out_streamed = true;
                    global_logger().add_stream(debug_log_stream_);
                    LOG_S(Logger::kLevelEvent, "[check_debug_mode] enter debug mode");
                } else  if (*debug_mode_ == 0 && debug_log_stream_) {
                    LOG_S(Logger::kLevelEvent, "[check_debug_mode] leave debug mode");
                    out_streamed = false;
                    global_logger().del_stream(debug_log_stream_);
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

                LOG_S(Logger::kLevelDebug, "[handle_timer] msg: data = " << msg_pop.data 
                    << ", sender = " << msg_pop.sender
                    << ", receiver = " << msg_pop.receiver 
                    << ", level = " << msg_pop.level);
            }
        }

    } // namespace common
} // namespace ppbox
