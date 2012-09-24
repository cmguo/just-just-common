// Dispatcher.h
#ifndef _PPBOX_COMMON_DISPATCHER_H_
#define _PPBOX_COMMON_DISPATCHER_H_

#include "ppbox/common/MsgQueueCommon.h"

#include <framework/string/Url.h>

#include <util/stream/Sink.h>

#include <boost/asio/deadline_timer.hpp>

namespace ppbox
{
    namespace common
    {  
        class Dispatcher
        {
        public:

            Dispatcher(
                boost::asio::io_service & ios);

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
            template <typename CompletionHandler>
            void post(CompletionHandler& handler)
            {
                worker_.post(handler);
            }

            virtual void async_wait(
                boost::uint32_t wait_timer
                , ppbox::common::session_callback_respone const &resp) ;

            virtual void cancel_wait(boost::system::error_code& ec);

       //派生类实现的
        protected:
            virtual void async_open_playlink(std::string const &playlink,ppbox::common::session_callback_respone const &resp) = 0;
            virtual void cancel_open_playlink(boost::system::error_code& ec) = 0;
            virtual void close_playlink(boost::system::error_code& ec) = 0;

            virtual void pause_moive(boost::system::error_code& ec) = 0;
            virtual void resume_moive(boost::system::error_code& ec) = 0;

            virtual void async_play_playlink(Session* session,ppbox::common::session_callback_respone const &resp) = 0;
            virtual void cancel_play_playlink(boost::system::error_code& ec) = 0;

            virtual void async_buffering(Session* session,ppbox::common::session_callback_respone const &resp) = 0;
            virtual void cancel_buffering(boost::system::error_code& ec) = 0;

        private:

            boost::system::error_code Dispatcher::async_play
                (Movie* move 
                ,Session* s
                , boost::uint32_t beg
                , boost::uint32_t end
                ,ppbox::common::session_callback_respone const &resp);

            void find_session(
                const boost::uint32_t session_id,
                Movie* &move,
                Session* &s,
                boost::system::error_code & ec);

            void open_callback_one(boost::system::error_code const & ec);
            void open_callback_two(boost::system::error_code const & ec);

            void play_callback_one(boost::system::error_code const & ec);
            void play_callback_two(boost::system::error_code const & ec);

            void close(Movie* m,Session* s,boost::system::error_code const & ec);
            void cancel_session(Movie* move);

            //状态处理模块 
        private:

            void open_callback(boost::system::error_code const & ec);
            void play_callback(boost::system::error_code const & ec);
            void buffering_callback(boost::system::error_code const & ec);
            void wait_callback(const boost::uint32_t time_id,boost::system::error_code const & ec);

//工具函数
            void response(Session* s,boost::system::error_code const & ec = boost::asio::error::operation_aborted);
            void clear_session(Movie* move,Session* session);

            enum SessionFlag
            {
                SF_ALL,   //保存所有
                SF_FRONT_NO_RESP,  //保存最前面的，都不回调
                SF_NONE,   //不保存 
                SF_NONE_NO_RESP
            };

            void resonse_session(Movie* move,SessionFlag sf = SF_NONE,boost::system::error_code const & ec = boost::asio::error::operation_aborted);

            enum PlayerFlag
            {
                PF_ALL,   //删除所有
                PF_FRONT, //删除最前面的一个
            };

            void resonse_player(Session* session,PlayerFlag pf = PF_ALL);


        protected:
            Movie* cur_mov_;
            Movie* append_mov_;

        private:
            boost::asio::io_service& ios_;
            boost::asio::deadline_timer timer_;
            boost::asio::io_service worker_;
            bool exit_;
            boost::uint32_t time_id_;
        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_DISPATCHER_H_
