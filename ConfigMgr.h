// ConfigMgr.h

#ifndef _JUST_COMMON_CONFIG_MGR_H_
#define _JUST_COMMON_CONFIG_MGR_H_


namespace framework
{
    namespace timer
    {
        class Timer;
    }
}

using namespace boost::system;


namespace just
{
    namespace common
    {

        class ConfigMgr
            : public just::common::CommonModuleBase<ConfigMgr>
        {
        public:
            ConfigMgr(
                util::daemon::Daemon & daemon);

        public:
            // ¿ªÆôÐÄÌø(keepalive)
            virtual boost::system::error_code startup();

            // Í£Ö¹ÐÄÌø(keepalive)
            virtual void shutdown();

        public:
            void set_config(
                char const * module, 
                char const * section, 
                char const * key, 
                char const * value);

        private:
            void handle_timer();

        private:
            framework::timer::Timer * timer_;
        };

    } // namespace common
} // namespace just

#endif // _JUST_COMMON_CONFIG_MGR_H_
