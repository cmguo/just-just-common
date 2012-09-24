#include "ppbox/common/Common.h"
#include "ppbox/common/Dispatcher.h"
#include "ppbox/common/ParseUrlTool.h"
#include "ppbox/common/CommonError.h"

#include <boost/bind.hpp>

namespace ppbox
{
    namespace common
    {  
        Dispatcher::Dispatcher(
            boost::asio::io_service& ios)
            :ios_(ios)
            , timer_(ios_)
            ,cur_mov_(NULL)
            ,append_mov_(NULL)
            , time_id_(0)
        {
        }

        Dispatcher::~Dispatcher()
        {

        }

        void Dispatcher::async_open(
            framework::string::Url const & playlink
            , boost::uint32_t& session_id
            ,ppbox::common::session_callback_respone const &resp)
        {
            static size_t g_session_id = rand();
            session_id = g_session_id++;

            boost::system::error_code ec;
            ppbox::common::ParseUrlTool url_tool(playlink);
            std::string play_link = url_tool.playlink();
            std::string format = url_tool.format();
            
            Session* s = new Session(session_id,format,resp);
            if (NULL == append_mov_)
            {
                cur_mov_ = new Movie(play_link,playlink,s);
                append_mov_ = cur_mov_;
                async_open_playlink(play_link,boost::bind(&Dispatcher::open_callback,this,_1));
            }
            else
            {
                if (append_mov_->play_link_ == play_link)
                {
                    if (append_mov_->openned_)
                    {//buffering
                        ios_.post(boost::bind(resp,ec));
                        if (append_mov_->sessions_.size() < 1)
                        {
                            ++time_id_;
                            cancel_wait(ec);
                            cancel_buffering(ec);
                        }
                    }
                    append_mov_->sessions_.push_back(s);
                }
                else
                {
                    if (append_mov_ == cur_mov_)
                    {
                        if (append_mov_->openned_
                            && append_mov_->append_session_ == NULL)
                        {
                            resonse_session(cur_mov_,SF_NONE_NO_RESP);
                            
                            close_playlink(ec);

                            async_open_playlink(play_link,boost::bind(&Dispatcher::open_callback,this,_1));

                            delete append_mov_;
                            append_mov_ = new Movie(play_link,playlink,s);
                            cur_mov_ = append_mov_;

                        }
                        else
                        {
                            cancel_session(cur_mov_);
                            append_mov_ = new Movie(play_link,playlink,s);
                        }
                    }
                    else
                    {
                        //替换append_
                        resonse_session(append_mov_,SF_NONE);
                        delete append_mov_;
                        append_mov_ = new Movie(play_link,playlink,s);
                    }

                    
                }
            }
        }

        void Dispatcher::cancel_session(Movie* move)
        {
            boost::system::error_code ec;
            //判断  playing buffering openning cancel_delay
            if (!move->openned_)
            {
                if (move->sessions_.size() > 0)
                {
                    resonse_session(move,SF_NONE);
                }

                cancel_open_playlink(ec);
            }
            else if (move->append_session_)
            {
                if (move->append_session_->playlist_.size() > 0)
                {
                    resonse_player(move->append_session_);
                }
                resonse_session(move,SF_FRONT_NO_RESP);
            }
            else if (move->cur_session_)
            {
                if (move->cur_session_->playlist_.size() > 0)
                {
                    resonse_player(move->cur_session_);
                }
                resonse_session(move,SF_NONE_NO_RESP);
                cancel_play_playlink(ec);
            }
            else
            {
                ++time_id_;
                cancel_wait(ec);

                cancel_buffering(ec);
            }
        }


        void Dispatcher::open_callback(boost::system::error_code const & ec)
        {
            assert(!cur_mov_->openned_);
            if (cur_mov_ == append_mov_)
            {
                open_callback_one(ec);
            }
            else
            {
                open_callback_two(ec);
            }
        }

        void Dispatcher::open_callback_one(boost::system::error_code const & ec)
        {//  openning  cancel_delay 
            if (!ec)
            {
                cur_mov_->openned_ = true;
                if (cur_mov_->sessions_.size() > 0)
                {//openning
                    boost::system::error_code ec1;
                    resonse_session(cur_mov_,SF_ALL,ec1);
                }
                else
                {//cancel_delay
                    Session* s = new Session();
                    cur_mov_->sessions_.push_back(s);
                    async_wait(10000,boost::bind(&Dispatcher::wait_callback,this,time_id_,_1));
                    async_buffering(s,boost::bind(&Dispatcher::buffering_callback,this,_1));
                }
            } 
            else
            {
                if (cur_mov_->sessions_.size() > 0)
                {//openning
                    resonse_session(cur_mov_,SF_NONE);
                }

                delete cur_mov_;
                cur_mov_ = append_mov_ = NULL;
            }

        }

        void Dispatcher::open_callback_two(boost::system::error_code const & ec)
        {// canceling
            assert(cur_mov_ != append_mov_);
            assert(cur_mov_->sessions_.size() < 1);


            delete cur_mov_;
            cur_mov_ = append_mov_;

            if (append_mov_->sessions_.size() < 1)
            {
                delete append_mov_;
                append_mov_ = cur_mov_ = NULL;
                return;
            }
            
            async_open_playlink(append_mov_->play_link_,boost::bind(&Dispatcher::open_callback,this,_1));
        }


        void Dispatcher::play_callback(boost::system::error_code const & ec)
        { // playling  next_session  play_canceling
            if (cur_mov_ == append_mov_)
            {
                play_callback_one(ec);
            }
            else
            {
                play_callback_two(ec);
            }
        }

        void Dispatcher::play_callback_one(boost::system::error_code const & ec)
        {//playling  next_session
            if (NULL != append_mov_->append_session_)
            {//next_session
                clear_session(append_mov_,append_mov_->cur_session_);
                append_mov_->cur_session_ = append_mov_->append_session_;
                append_mov_->append_session_ = NULL;
            }
            else
            {//playing
                //清除第0个play
                resonse_player(append_mov_->cur_session_,PF_FRONT);
            }

            if (append_mov_->cur_session_->playlist_.size() < 1)
            {
                clear_session(append_mov_,append_mov_->cur_session_);
                append_mov_->append_session_ = append_mov_->cur_session_ = NULL;

                if (append_mov_->sessions_.size() < 1)
                {//转close_delay
                    Session* s = new Session();
                    append_mov_->sessions_.push_back(s);
                    async_wait(10000,boost::bind(&Dispatcher::wait_callback,this,time_id_,_1));
                    async_buffering(s,boost::bind(&Dispatcher::buffering_callback,this,_1));
                }
                else
                {
                    //转openned
                }
            }
            else
            {
                async_play_playlink(append_mov_->cur_session_,boost::bind(&Dispatcher::play_callback,this,_1));
            }
        }

        void Dispatcher::play_callback_two(boost::system::error_code const & ec)
        {//play_canceling
            assert(cur_mov_->sessions_.size() > 0);
            boost::system::error_code ec1;
            close_playlink(ec1);

            clear_session(cur_mov_,cur_mov_->cur_session_);
            assert(cur_mov_->sessions_.size() < 1);
            delete cur_mov_;
            cur_mov_ = append_mov_;

            if (append_mov_->sessions_.size() < 1)
            {
                delete append_mov_;
                append_mov_ = cur_mov_ = NULL;
                return;
            }

            async_open_playlink(append_mov_->play_link_,boost::bind(&Dispatcher::open_callback,this,_1));
        }

        void Dispatcher::setup(
            boost::uint32_t session_id,
            size_t control,
            util::stream::Sink* sink,
            boost::system::error_code& ec)
        {
            Movie* m = NULL;
            Session* s = NULL;
            find_session(session_id,m,s,ec);
            if (!ec)
            {
                if (m == cur_mov_ && s != m->cur_session_)
                {
                     s->sinks_.add(control,sink);
                }
                else
                {
                    ec == error::wrong_status;
                }
            }
        }


        void Dispatcher::buffering_callback(boost::system::error_code const & ec)
        {
            assert(append_mov_ != cur_mov_);
            play_callback_two(ec);
        }

        void Dispatcher::wait_callback(const boost::uint32_t time_id,boost::system::error_code const & ec)
        {
            if (time_id != time_id_)
            {
                return;
            }

            ++time_id_;
            
            boost::system::error_code ec1;
            
            //cancel_delay buffering
            assert(cur_mov_ == append_mov_);

            if (cur_mov_->openned_)
            {//buffering
                cancel_buffering(ec1);
            }
            else
            {//cancel_delay
                cancel_open_playlink(ec1);
            }
            append_mov_ = new Movie();
        }

        boost::system::error_code Dispatcher::async_play
            (Movie* move 
            ,Session* s
            , boost::uint32_t beg
            , boost::uint32_t end
            ,ppbox::common::session_callback_respone const &resp)
        {
            boost::system::error_code ec;
            if (move->openned_)
            {
                Player player(beg,end,resp);

                if (move->append_session_ == NULL)
                {
                    move->cur_session_ = move->append_session_ = s;
                    s->playlist_.push_back(player);
                    async_play_playlink(s,boost::bind(&Dispatcher::play_callback,this,_1));
                } 
                else
                {
                    if (move->append_session_ == s )
                    {
                        if (s->playlist_.size() > 0)
                        {
                            resonse_player(s);
                        }
                    }
                    else if (move->cur_session_ == s)
                    {
                        ec = error::wrong_status;
                    }
                    else
                    {
                        if(move->append_session_ != move->cur_session_)
                        {
                            resonse_player(move->append_session_);
                        }
                        move->append_session_ = s;
                    }

                }

                if (!ec)
                {
                    s->playlist_.push_back(player);
                }
            }
            else
            {
                ec = error::play_not_open_moive;
            }
            return ec;

        }

        void Dispatcher::async_play(boost::uint32_t session_id
            , boost::uint32_t beg
            , boost::uint32_t end
            ,ppbox::common::session_callback_respone const &resp)
        {
            boost::system::error_code ec;

            Movie* m = NULL;
            Session* s = NULL;
            find_session(session_id,m,s,ec);
            
            if (!ec)
            {
                if (m == cur_mov_)
                {
                    ec = async_play(m,s,beg,end,resp);
                }
                else
                {
                    ec = error::canceled_moive;
                }

            }

            if (ec)
            {
                assert(0);
                resp(ec);
            }
        }

        void Dispatcher::close(boost::uint32_t session_id,boost::system::error_code& ec)
        {
            Movie* m = NULL;
            Session* s = NULL;
            find_session(session_id,m,s,ec);

            if (!ec)
            {
                if (m == append_mov_)
                {
                    close(m,s,ec);
                }
                else
                {
                    ec = error::canceled_moive;
                }
            }

        }

        void Dispatcher::close(Movie* m,Session* s,boost::system::error_code const & ec)
        { //openning openned  next_session playling
            if (append_mov_ == cur_mov_)
            {
                if (s->playlist_.size() > 0)
                {
                    resonse_player(s);
                }

                if (!m->openned_)
                {
                    response(s);
                }
                
                clear_session(m,s);
                Session* sTemp = new Session();

                if (m->cur_session_ == s) //1 2 | 1 1
                {
                    m->cur_session_ = m->append_session_ = sTemp;
                } 
                else if (m->append_session_ == s) //two session
                {
                    m->append_session_ = sTemp;
                }
                else 
                {
                    if (m->sessions_.size() < 1)
                    {
                        async_wait(10000,boost::bind(&Dispatcher::wait_callback,this,time_id_,_1));
                        if (m->openned_)
                        {
                            async_buffering(s,boost::bind(&Dispatcher::buffering_callback,this,_1));
                        }
                        else
                        {//cancel_delay
                            delete sTemp;
                            return;
                        }
                    }
                    else
                    {//关闭一个没播放的session
                        delete sTemp;
                        return;
                    }
                }
                m->sessions_.push_back(sTemp);
            }
            else
            {
                response(s);
                clear_session(m,s);
                if (m->sessions_.size() < 1)
                {
                    Session* sTemp = new Session();
                    m->sessions_.push_back(sTemp);
                }
            }
        }


        void Dispatcher::pause(boost::uint32_t session_id,boost::system::error_code& ec)
        {

            if(NULL == cur_mov_ 
                || cur_mov_->sessions_.size() < 1 
                || cur_mov_->sessions_[0]->playlist_.size() < 1
                || cur_mov_->sessions_[0]->session_id_ != session_id)
            {
                assert(0);
                ec = boost::asio::error::operation_aborted;
            }
            else
            {
                pause_moive(ec);
            }
        }

        void Dispatcher::resume(boost::uint32_t session_id,boost::system::error_code& ec)
        {
            if(NULL == cur_mov_ 
                || cur_mov_->sessions_.size() < 1 
                || cur_mov_->sessions_[0]->playlist_.size() < 1
                || cur_mov_->sessions_[0]->session_id_ != session_id)
            {
                ec = boost::asio::error::operation_aborted;
            }
            else
            {
                resume_moive(ec);
            }
        }

        boost::system::error_code Dispatcher::kill() 
        {
            boost::system::error_code ec;
            if (append_mov_ && append_mov_->append_session_)
            {
                close(append_mov_->append_session_->session_id_,ec);
            }
            
            return boost::system::error_code();
        }

        boost::system::error_code Dispatcher::stop() 
        {
            kill();
            worker_.run();
            return boost::system::error_code();
        }

        boost::system::error_code Dispatcher::start()
        {
            worker_.run();
            return boost::system::error_code();
        }

        void Dispatcher::async_wait(
            boost::uint32_t wait_timer
            , ppbox::common::session_callback_respone const &resp)
        {
            timer_.expires_from_now(boost::posix_time::seconds(wait_timer));
            timer_.async_wait(resp);
        }

        void Dispatcher::cancel_wait(boost::system::error_code& ec)
        {
            timer_.cancel();
        }


        void Dispatcher::find_session(
            const boost::uint32_t session_id,
            Movie* &move,
            Session* &s,
            boost::system::error_code & ec)
        {
            if (NULL == append_mov_)
            {
                assert(0);
                ec = error::wrong_status;
                return;
            }

            Movie::Iter iter = std::find_if(append_mov_->sessions_.begin(),
                append_mov_->sessions_.end(),FindBySession(session_id));
            if (iter != append_mov_->sessions_.end())
            {
                move = append_mov_;
                s = *iter;
            }
            else if (append_mov_ != cur_mov_ && cur_mov_->sessions_.end() != 
                (iter = std::find_if(cur_mov_->sessions_.begin(),
                cur_mov_->sessions_.end(),FindBySession(session_id)))
                )
            {
                move = cur_mov_;
                s = *iter;
            }
            else
            {
                ec = error::not_find_session;
            }
        }

        void Dispatcher::resonse_player(Session* session,PlayerFlag pf )
        {
            boost::system::error_code ec = boost::asio::error::operation_aborted;
            switch (pf)
            {
            case PF_ALL:
                {
                    for(Session::Iter iter = session->playlist_.begin();
                        iter != session->playlist_.begin();
                        ++iter)
                    {
                        ios_.post(boost::bind((*iter).resp_,ec));
                    }
                    session->playlist_.clear();
                }
                break;
            case PF_FRONT:
                {
                    Session::Iter iter = session->playlist_.begin();
                    ios_.post(boost::bind((*iter).resp_,ec));
                    session->playlist_.erase(iter);
                }
                break;
            }

        }

        void Dispatcher::response(Session* s,boost::system::error_code const & ec)
        {
            ios_.post(boost::bind(s->resp_,ec));
        }

        void Dispatcher::resonse_session(Movie* move,SessionFlag sf,boost::system::error_code const & ec)
        {
            /*
            SF_ALL,   //保存所有
            SF_FRONT, //保存最前面的一个
            SF_FRONT_NO_RESP, //保留最前面的，都不回调
            SF_NONE   //不保存 
            */
            Session* s = NULL;
            
            if (move->sessions_.size() < 1)
            {
                return;
            }

            switch (sf)
            {
            case SF_ALL:
                {
                    for (Movie::Iter iter = move->sessions_.begin();
                        iter != move->sessions_.end();
                        ++iter)
                    {
                        s = (*iter);
                        ios_.post(boost::bind(s->resp_,ec));
                    }
                }
                break;
            case SF_FRONT_NO_RESP:
                {
                    Session* front = move->cur_session_;
                    for (Movie::Iter iter = move->sessions_.begin(); iter != move->sessions_.end();++iter)
                    {
                        if (front != *iter)
                        {
                            s = (*iter);
                            delete s;
                        }
                    }
                    move->sessions_.clear();
                    move->sessions_.push_back(front);
                }
                break;
            case SF_NONE:
                {
                    for (Movie::Iter iter = move->sessions_.begin();
                        iter != move->sessions_.end();
                        ++iter)
                    {
                        s = (*iter);
                        ios_.post(boost::bind(s->resp_,ec));
                        delete s;
                    }
                    move->sessions_.clear();
                }
                break;
            case SF_NONE_NO_RESP:
                {
                    for (Movie::Iter iter = move->sessions_.begin();
                        iter != move->sessions_.end();
                        ++iter)
                    {
                        s = (*iter);
                        delete s;
                    }
                    move->sessions_.clear();
                }
                break;
            default:
                break;
            }
        }

        void Dispatcher::clear_session(Movie* move,Session* session)
        {
            Movie::Iter iter = std::remove_if(move->sessions_.begin(),move->sessions_.end(),FindBySession(session->session_id_));
            if (iter != move->sessions_.end())
            {
                move->sessions_.erase(iter);
            }
            delete session;
        }

    } // namespace common
} // namespace ppbox

