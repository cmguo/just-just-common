// ScheduleManager.h

#ifndef _JUST_COMMON_SCHEDULE_MANAGER_H_
#define _JUST_COMMON_SCHEDULE_MANAGER_H_

#include <util/daemon/Module.h>

namespace just
{
    namespace common
    {

        class ScheduleManager
            : public util::daemon::ModuleBase<ScheduleManager>
        {
        public:
            typedef boost::function<
                void (void const *, boost::system::error_code const & ec)> callback_t;

        public:
            ScheduleManager(
                util::daemon::Daemon & daemon);

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            void * schedule_callback(
                boost::uint32_t delay, // ms
                void const * user_data, 
                callback_t const & callback);

            void cancel_callback(
                void const * handle, 
                boost::system::error_code & ec);
        };

    } // namespace common
} // namespace just

#endif // _JUST_COMMON_SCHEDULE_MANAGER_H_
