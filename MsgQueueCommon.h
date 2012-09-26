//MsgQueueCommon.h
#ifndef _PPBOX_COMMON_MSGQ_COMMON_H_
#define _PPBOX_COMMON_MSGQ_COMMON_H_


#include "ppbox/common/Sinks.h"
#include "ppbox/common/CommonError.h"

#include <framework/string/Url.h>

#include <string>

namespace ppbox
{
    namespace common
    {
        typedef boost::function<void (
            boost::system::error_code const &)
        >  session_callback_respone;

        typedef boost::function<void (
            const boost::uint32_t,
            boost::system::error_code const &)
        >  wait_callback_respone;

        struct MediaInfo
        {
            boost::uint32_t duration;
            boost::uint32_t filesize;
            void * attachment;
        };

        struct PlayInfo
        {
            boost::uint32_t head_size; 
            boost::uint32_t tail_size; 
            boost::uint64_t body_size; 
            boost::uint64_t cur_offset; 
            boost::uint32_t speed; 
            boost::uint32_t area; 
            boost::uint32_t percent; 
            boost::uint32_t headpercent;
        };

        class PlayInterface
        {
        public:
            PlayInterface(boost::asio::io_service & ios)
                : ios_(ios)
                , exit_(true)
                , playing_(false)
            {

            }

            virtual ~PlayInterface()
            {

            }

            void stop()
            {
                exit_ = true;
            }

            void pause()
            {
                playing_ = false;
            }

            void resume()
            {
                playing_ = true;
            }

            bool is_working()
            {
                return (!exit_);
            }
    
        protected:
            bool exit_;
            bool playing_;
            boost::asio::io_service & ios_;
        };

        struct Player
        {
            Player()
                : beg_(0)
                , end_(-1)
            {
            }

            Player(
                boost::uint32_t beg
                ,boost::uint32_t end
                ,ppbox::common::session_callback_respone const & resp)
                : beg_(beg)
                , end_(end)
                , resp_(resp)
            {
            }

            boost::uint32_t beg_;
            boost::uint32_t end_;
            ppbox::common::session_callback_respone resp_;  //play �ص�
        };

        struct Session
        {
            Session(boost::asio::io_service & ios)
                : session_id_(-1)
                , sinks_(ios)
            {
            }

            Session(boost::asio::io_service & ios
                ,boost::uint32_t session_id
                , std::string format
                , ppbox::common::session_callback_respone resp)
                :session_id_(session_id)
                , sinks_(ios)
                , format_(format)
                ,resp_(resp)
            {

            }

            std::string format_;
            boost::uint32_t session_id_; 
            Sinks sinks_;
            typedef std::vector<Player>::iterator Iter;
            std::vector<Player> playlist_;
            ppbox::common::session_callback_respone resp_;  //async_open �ص� 
        };

        struct Movie
        {
            Movie()
                : cur_session_(NULL)
                , append_session_(NULL)
                , openned_(false)
            {
            }

             Movie(
                 const std::string& play_link,
                 const framework::string::Url& params,
                 Session *s)
                 : play_link_(play_link)
                 ,params_(params)
                 , cur_session_(NULL)
                 , append_session_(NULL)
                 , openned_(false)
             {
                sessions_.push_back(s);
             }

            virtual ~Movie()
            {

            }

            std::string play_link_;
            framework::string::Url params_;
            Session* cur_session_;
            Session* append_session_;
            bool openned_;
            typedef std::vector<Session*>::iterator Iter;
            std::vector<Session*> sessions_;
        };


        class FindBySession
        {
        public:
            FindBySession(boost::uint32_t session_id):session_id_(session_id){}
            ~FindBySession(){}
            bool operator()(const Session* msg) const
            {
                return (msg->session_id_ == session_id_);
            }
        private:
            boost::uint32_t session_id_;
        };
    }
}

#endif
