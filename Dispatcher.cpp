#include "ppbox/common/Common.h"
#include "ppbox/common/Dispatcher.h"
#include "ppbox/common/ParseUrlTool.h"
#include "ppbox/common/CommonError.h"

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <framework/thread/MessageQueue.h>

namespace ppbox
{
    namespace common
    {  
        Dispatcher::Dispatcher(
            util::daemon::Daemon & daemon)
            :daemon_(daemon)
            ,cur_mov_(NULL)
            ,append_mov_(NULL)
            , time_id_(0)
        {
            msgq_ = new framework::thread::MessageQueue<PlayInterface*>;
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


            if (NULL == append_mov_)
            {
                async_open_zero(playlink,session_id,resp);
            } 
            else
            {
                if (append_mov_ == cur_mov_)
                {
                    async_open_one(playlink,session_id,resp);
                } 
                else
                {
                    async_open_two(playlink,session_id,resp);
                }
            }
        }

        void Dispatcher::async_open_zero(
            framework::string::Url const & playlink
            , boost::uint32_t const session_id
            ,ppbox::common::session_callback_respone const &resp)
        {
            boost::system::error_code ec;

            ppbox::common::ParseUrlTool url_tool(playlink);
            std::string play_link = url_tool.playlink();
            std::string format = url_tool.format();

            Session* s = new Session(session_id,resp);

            cur_mov_ = new Movie();
            cur_mov_->play_link_= play_link;
            cur_mov_->format_ = format;
            cur_mov_->params_ = playlink;
            cur_mov_->sessions_.push_back(s);

            append_mov_ = cur_mov_;
            async_open_playlink(play_link,boost::bind(&Dispatcher::open_callback,this,_1));
        }

        //已经有一部电影了
        void Dispatcher::async_open_one(
            framework::string::Url const & playlink
            , boost::uint32_t const session_id
            ,ppbox::common::session_callback_respone const &resp)
        {
            boost::system::error_code ec;

            ppbox::common::ParseUrlTool url_tool(playlink);
            std::string play_link = url_tool.playlink();
            std::string format = url_tool.format();

            Session* s = new Session(session_id,resp);

            if (play_link != cur_mov_->play_link_
                || format != cur_mov_->format_)
            {
                append_mov_ = new Movie();
                append_mov_->play_link_= play_link;
                append_mov_->format_ = format;
                append_mov_->params_ = playlink;
                append_mov_->sessions_.push_back(s);

                if (cur_mov_->openned_)
                {// openned playling close_delay next_session
                    if (cur_mov_->sessions_.size() > 0)
                    {//openned playling next_session
                        if (cur_mov_->append_session_ != NULL)
                        {//next_session
                            resonse_player(cur_mov_->append_session_,boost::asio::error::operation_aborted);
                            resonse_session(cur_mov_,boost::asio::error::operation_aborted,SF_FRONT_NO_RESP);
                            cur_mov_->append_session_ = NULL;
                        }
                        else
                        {
                            if (cur_mov_->sessions_[0]->playlist_.size() > 0)
                            {//playing
                                resonse_player(cur_mov_->sessions_[0],boost::asio::error::operation_aborted);
                                resonse_session(cur_mov_,boost::asio::error::operation_aborted,SF_FRONT_NO_RESP);
                                cancel_play_playlink(ec);

                            } 
                            else
                            {//openned  转openning状态
                                resonse_session(cur_mov_,boost::asio::error::operation_aborted,SF_NONE);
                                close_format(ec);
                                close_playlink(ec);
                                delete cur_mov_;
                                cur_mov_ = append_mov_;

                                async_open_playlink(play_link,boost::bind(&Dispatcher::open_callback,this,_1));
                                append_mov_->openned_ = false;
                            }
                        }
                    } 
                    else
                    {//close_delay
                        ++time_id_;
                        cancel_wait(ec);
                        cancel_buffering(ec);
                    }
                } 
                else
                {// openning cancel_delay
                    if (cur_mov_->sessions_.size() > 0)
                    {//openning
                        resonse_session(cur_mov_,boost::asio::error::operation_aborted,SF_NONE);
                    }
                    else
                    {//cancel_delay
                        ++time_id_;
                        cancel_wait(ec);
                    }
                    cancel_open_playlink(ec);
                }
            }
            else
            {//
                if (cur_mov_->sessions_.size() < 1)
                {
                    ++time_id_;
                    cancel_wait(ec);
                } 
                cur_mov_->sessions_.push_back(s);

                if (cur_mov_->openned_)
                {// openned playling close_delay next_session
                    daemon_.io_svc().post(boost::bind(resp,ec));
                } 
            }
        }

        //已经有两部电影
        void Dispatcher::async_open_two(
            framework::string::Url const & playlink
            , boost::uint32_t const session_id
            ,ppbox::common::session_callback_respone const &resp)
        {
            ppbox::common::ParseUrlTool url_tool(playlink);
            std::string play_link = url_tool.playlink();
            std::string format = url_tool.format();

            Session* s = new Session(session_id,resp);

//canceling  play_canceling
            if (play_link != cur_mov_->play_link_
                || format != cur_mov_->format_)
            {
                resonse_session(append_mov_,boost::asio::error::operation_aborted,SF_NONE);
                append_mov_->play_link_= play_link;
                append_mov_->format_ = format;
                append_mov_->params_ = playlink;
            }

            append_mov_->sessions_.push_back(s);
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
                    open_format(cur_mov_->format_,ec1);
                    assert(!ec1);
                    resonse_session(cur_mov_,ec,SF_ALL);
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
                    resonse_session(cur_mov_,ec,SF_NONE);
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
            if (NULL != cur_mov_->append_session_)
            {//next_session
                clear_session(cur_mov_,cur_mov_->cur_session_);
                cur_mov_->cur_session_ = cur_mov_->append_session_;
                cur_mov_->append_session_ = NULL;
            }
            else
            {//playing
                //清除第0个play
                resonse_player(cur_mov_->cur_session_,ec,PF_FRONT);
            }

            if (cur_mov_->cur_session_->playlist_.size() < 1)
            {
                clear_session(cur_mov_,cur_mov_->cur_session_);
                cur_mov_->cur_session_ = NULL;

                if (cur_mov_->sessions_.size() < 1)
                {//转close_delay
                    Session* s = new Session();
                    cur_mov_->sessions_.push_back(s);
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
                async_play_playlink(cur_mov_->cur_session_,boost::bind(&Dispatcher::play_callback,this,_1));
            }
        }

        void Dispatcher::play_callback_two(boost::system::error_code const & ec)
        {//play_canceling
            assert(cur_mov_->sessions_.size() > 0);
            boost::system::error_code ec1;
            close_format(ec1);
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
            assert(NULL != cur_mov_);
            ec.clear();
            Movie::Iter iter = std::find_if(cur_mov_->sessions_.begin(),
                cur_mov_->sessions_.end(),FindBySession(session_id));
            if (iter == cur_mov_->sessions_.end())
            {
                assert(0);
                ec = error::not_find_session;
            }
            else
            {
                Session* s = *iter;
                if(s == cur_mov_->cur_session_)
                {
                    ec == error::wrong_status;
                }
                else
                {
                    s->sinks_.add(control,sink);
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
            append_mov_->play_link_= "zhang";
            append_mov_->format_ = "kuang";
        }

        void Dispatcher::async_play(boost::uint32_t session_id
            , boost::uint32_t beg
            , boost::uint32_t end
            ,ppbox::common::session_callback_respone const &resp)
        {
            boost::system::error_code ec = boost::asio::error::operation_aborted;
            if (append_mov_ != cur_mov_ || !cur_mov_->openned_)
            {
                assert(0);
                ec = error::wrong_status;
                resp(ec);
                return;
            }
            
            Movie::Iter iter = std::find_if(cur_mov_->sessions_.begin(),
                cur_mov_->sessions_.end(),FindBySession(session_id));
            if (iter == cur_mov_->sessions_.end())
            {
                assert(0);
                ec = error::not_find_session;
                resp(ec);
                return;
            }

            Session * s = *iter;

            Player player(beg,end,resp);

            //openned next_session playling 
            if (cur_mov_->append_session_ != NULL)
            {//next_session
                if (cur_mov_->cur_session_ == s)
                {
                    resp(ec);
                    return;
                }
                else if (cur_mov_->append_session_ != s)
                {
                    //清除排队的
                    resonse_player(cur_mov_->append_session_,ec);
                    clear_session(cur_mov_,cur_mov_->append_session_);

                    //s加入排队播放
                    cur_mov_->append_session_ = s;
                    s->playlist_.push_back(player);
                }
            
            }
            else if (cur_mov_->cur_session_ != NULL)
            {//playling
                if (cur_mov_->cur_session_ != s)
                {
                    resonse_player(cur_mov_->cur_session_,ec);
                    cancel_open_playlink(ec);
                    cur_mov_->append_session_ = s;
                } 
                s->playlist_.push_back(player);
            }
            else
            {//openned
                cur_mov_->cur_session_ = s;
                s->playlist_.push_back(player);
                async_play_playlink(s,boost::bind(&Dispatcher::play_callback,this,_1));
            }
            
        }

        void Dispatcher::close(boost::uint32_t session_id,boost::system::error_code& ec)
        {
            Movie::Iter iter = std::find_if(append_mov_->sessions_.begin(),
                append_mov_->sessions_.end(),FindBySession(session_id));
            if (iter == append_mov_->sessions_.end())
            {
                ec = boost::asio::error::operation_aborted;
            }
            else
            {

                if (append_mov_ == cur_mov_)
                {
                    close_one(*iter);
                } 
                else
                {
                    close_two(*iter);
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

        void Dispatcher::close_one(Session* s)
        {
            boost::system::error_code ec = boost::asio::error::operation_aborted;
            //openning openned  next_session playling
            if (!cur_mov_->openned_)
            {//openning
                daemon_.io_svc().post(boost::bind(s->resp_,ec));
            }
            else
            {//openned  next_session playling
                if (cur_mov_->append_session_ != NULL)
                {//next_session
                    if (cur_mov_->cur_session_ == s)
                    {
                        assert(0);
                    }
                    else if (cur_mov_->append_session_ == s)
                    {
                        resonse_player(s,ec);
                        return;
                    }
                }
                else if (cur_mov_->cur_session_ != NULL)
                {//playing
                    if (cur_mov_->cur_session_ == s)
                    { //转next_session
                        resonse_player(s,ec);
                        cancel_play_playlink(ec);

                        Session* sTmp = new Session();
                        cur_mov_->append_session_ = sTmp;
                        return;
                    }
                }
                else
                {//openned
                    
                }
            }
            clear_session(cur_mov_,s);
            if (cur_mov_->sessions_.size() < 1)
            {//转cancel_delay 或 buffering
                if (cur_mov_->openned_)
                {//转buffering
                    Session* sTemp = new Session();
                    cur_mov_->sessions_.push_back(sTemp);
                    async_buffering(s,boost::bind(&Dispatcher::buffering_callback,this,_1));
                }
                async_wait(10000,boost::bind(&Dispatcher::wait_callback,this,time_id_,_1));

            }
        }

        void Dispatcher::close_two(Session* s)
        {
            clear_session(append_mov_,s);
        }

        boost::system::error_code Dispatcher::kill() 
        {
            return boost::system::error_code();
        }

        boost::system::error_code Dispatcher::stop() 
        {
            exit_ = true;
            kill();

            dispatch_thread_->join();
            delete dispatch_thread_;
            dispatch_thread_ = NULL;
            return boost::system::error_code();
        }

        boost::system::error_code Dispatcher::start()
        {
            exit_ = false;
            dispatch_thread_ = new boost::thread(
                boost::bind(&Dispatcher::thread_dispatch, this));
            return boost::system::error_code();
        }

        void Dispatcher::post(PlayInterface *player)
        {
            msgq_->push(player);
        }

        void Dispatcher::thread_dispatch()
        {
            PlayInterface* pMsgType = NULL;
            while(!exit_) 
            {
                if( msgq_->timed_pop(pMsgType,boost::posix_time::milliseconds(10*1000)))
                {
                    thread_command(pMsgType);
                    delete pMsgType;
                    pMsgType = NULL;
                }
            }
        }

        boost::system::error_code Dispatcher::thread_command(PlayInterface* pMsgType)
        {
            boost::system::error_code ec;
            ec = pMsgType->doing();
            return ec;
        }

        void Dispatcher::resonse_player(Session* session,boost::system::error_code const & ec,PlayerFlag pf )
        {
            switch (pf)
            {
            case PF_ALL:
                {
                    for(Session::Iter iter = session->playlist_.begin();
                        iter != session->playlist_.begin();
                        ++iter)
                    {
                        (*iter).resp_(ec);
                    }
                    session->playlist_.clear();
                }
                break;
            case PF_FRONT:
                {
                    Session::Iter iter = session->playlist_.begin();
                    session->playlist_.erase(iter);
                    (*iter).resp_(ec);
                }
                break;
            }

        }

        void Dispatcher::resonse_session(Movie* move,boost::system::error_code const & ec,SessionFlag sf)
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
                        daemon_.io_svc().post(boost::bind(s->resp_,ec));
                    }
                }
                break;
            case SF_FRONT_NO_RESP:
                {
                    Session* front = move->cur_session_;
                    std::remove_if(move->sessions_.begin(),move->sessions_.end(),FindBySession(front->session_id_));

                    for (Movie::Iter iter = move->sessions_.begin(); iter != move->sessions_.end();++iter)
                    {
                        s = (*iter);
                        delete s;
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
                        daemon_.io_svc().post(boost::bind(s->resp_,ec));
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

