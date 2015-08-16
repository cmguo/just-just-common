// ScheduleManager.cpp

#include "just/common/Common.h"
#include "just/common/ScheduleManager.h"

#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>

namespace just
{
    namespace common
    {

        ScheduleManager::ScheduleManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<ScheduleManager>(daemon, "ScheduleManager")
        {
        }

        bool ScheduleManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool ScheduleManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        void * ScheduleManager::schedule_callback(
            boost::uint32_t delay, // ms
            void const * user_data, 
            callback_t const & callback)
        {
            boost::asio::deadline_timer * timer2 = new boost::asio::deadline_timer(io_svc());
            timer2->expires_from_now(boost::posix_time::milliseconds(delay));
            timer2->async_wait(boost::bind(callback, user_data, _1));
            return timer2;
        }

        void ScheduleManager::cancel_callback(
            void const * handle, 
            boost::system::error_code & ec)
        {
            boost::asio::deadline_timer * timer = (boost::asio::deadline_timer *)handle;
            timer->cancel(ec);
            delete timer;
        }

    } // namespace common
} // namespace just
