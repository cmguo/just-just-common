// Dispatcher.h
#ifndef _PPBOX_COMMON_DISPATCHER_H_
#define _PPBOX_COMMON_DISPATCHER_H_

#include "ppbox/common/MsgQueueCommon.h"

#include <framework/string/Url.h>

#include <util/stream/Sink.h>

namespace framework { namespace thread { template <typename _Ty> class MessageQueue; } }

namespace ppbox
{
    namespace common
    {  
        class Dispatcher
        {
        public:

            Dispatcher(
                util::daemon::Daemon & daemon);

            virtual ~Dispatcher();

        public:
            void async_open(
                framework::string::Url const & playlink
                , boost::uint32_t& session_id
                ,ppbox::common::session_callback_respone const &resp);

            virtual void setup(
                        boost::uint32_t session_id,
                            size_t control,
                            util::stream::Sink* sink,
                            std::string& info,
                            boost::system::error_code& ec); 

            void async_play(boost::uint32_t session_id
                , boost::uint32_t beg
                , boost::uint32_t end
                ,ppbox::common::session_callback_respone const &resp);

            void close(boost::uint32_t session_id,boost::system::error_code& ec);
            
            void pause(boost::uint32_t session_id,boost::system::error_code& ec);
            void resume(boost::uint32_t session_id,boost::system::error_code& ec);



            boost::system::error_code kill() ;

            boost::system::error_code stop() ;

            boost::system::error_code start();

            virtual boost::system::error_code get_media_info(
                ppbox::common::MediaInfo & info) = 0;

            virtual boost::system::error_code get_play_info(
                ppbox::common::PlayInfo & info) = 0;

        protected:
            void post(PlayInterface *player);

       //派生类实现的
        protected:
            virtual void async_open_playlink(std::string const &playlink,ppbox::common::session_callback_respone const &resp) = 0;
            virtual void cancel_open_playlink(boost::system::error_code& ec) = 0;
            virtual void close_playlink(boost::system::error_code& ec) = 0;

            virtual void open_format(std::string const &format,boost::system::error_code& ec) = 0;
            virtual void close_format(boost::system::error_code& ec) = 0;

            virtual void pause_moive(boost::system::error_code& ec) = 0;
            virtual void resume_moive(boost::system::error_code& ec) = 0;

            virtual void async_play_playlink(Session* session,ppbox::common::session_callback_respone const &resp) = 0;
            virtual void cancel_play_playlink(boost::system::error_code& ec) = 0;

            virtual void async_buffering(Session* session,ppbox::common::session_callback_respone const &resp) = 0;
            virtual void cancel_buffering(boost::system::error_code& ec) = 0;


            virtual void async_wait(
                boost::uint32_t wait_timer
                , ppbox::common::session_callback_respone const &resp) = 0;

            virtual void cancel_wait(boost::system::error_code& ec) = 0;

        private:
            void thread_dispatch();
            boost::system::error_code thread_command(PlayInterface* pMsgType);

            void async_open_Zero(
                framework::string::Url const & playlink
                , boost::uint32_t const session_id
                ,ppbox::common::session_callback_respone const &resp);

            void async_open_One(
                framework::string::Url const & playlink
                , boost::uint32_t const session_id
                ,ppbox::common::session_callback_respone const &resp);

            void async_open_Two(
                framework::string::Url const & playlink
                , boost::uint32_t const session_id
                ,ppbox::common::session_callback_respone const &resp);

            void open_callback_One(boost::system::error_code const & ec);
            void open_callback_Two(boost::system::error_code const & ec);

            void play_callback_One(boost::system::error_code const & ec);
            void play_callback_Two(boost::system::error_code const & ec);

            void close_one(Session* s);
            void close_two(Session* s);

            //状态处理模块 
        private:

            void open_callback(boost::system::error_code const & ec);
            void play_callback(boost::system::error_code const & ec);
            void buffering_callback(boost::system::error_code const & ec);
            void wait_callback(const boost::uint32_t time_id,boost::system::error_code const & ec);

//工具函数
            enum SessionFlag
            {
                SF_ALL,   //保存所有
                SF_FRONT_NO_RESP,  //保存最前面的，都不回调
                SF_NONE   //不保存 
            };

            void resonse_session(Movie* move,boost::system::error_code const & ec,SessionFlag sf = SF_NONE);

            enum PlayerFlag
            {
                PF_ALL,   //删除所有
                PF_FRONT, //删除最前面的一个
            };
            void resonse_player(Session* session,boost::system::error_code const & ec,PlayerFlag pf = PF_ALL);
            
            void clear_session(Movie* move,Session* session);


        protected:
            Movie* cur_mov_;
            Movie* append_mov_;

        private:
            util::daemon::Daemon& daemon_;
            bool exit_;
            boost::thread * dispatch_thread_;
            framework::thread::MessageQueue<PlayInterface*> * msgq_;
            boost::uint32_t time_id_;
        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_DISPATCHER_H_
