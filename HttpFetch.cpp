// BigMp4.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/HttpFetch.h"

#include <util/protocol/http/HttpRequest.h>
#include <framework/logger/LoggerStreamRecord.h>
using namespace framework::logger;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("HttpFetch", 0);


namespace ppbox
{
    namespace common
    {

        HttpFetch::HttpFetch(boost::asio::io_service & io_svc)
            : http_(io_svc)
            ,canceled_(false)
            , try_times_(0)
        {
        }

        HttpFetch::~HttpFetch()
        {
        }

        void HttpFetch::async_fetch(
            framework::string::Url const & url
            ,framework::network::NetName const & server_host
            ,response_type const & resp)
        {
            resp_ = resp;
            server_host_ = server_host;

            util::protocol::HttpRequestHead request_head;
            request_head.method = util::protocol::HttpRequestHead::get;
            request_head.path = url.path_all();
            request_head.host.reset(url.host_svc());
            request_head["Accept"] = "{*/*}";

            std::ostringstream oss;
            request_head.get_content(oss);
            LOG_STR(framework::logger::Logger::kLevelDebug1, oss.str().c_str());

            http_stat_.begin_try();
            http_.async_fetch(request_head,
                boost::bind(&HttpFetch::handle_fetch, this, _1));
        }


        void HttpFetch::cancel()
        {
            boost::system::error_code ec1;
            http_.cancel(ec1);
            canceled_ = true;
        }

        void HttpFetch::close()
        {
            boost::system::error_code  ec;
            http_.close(ec);
        }

        HttpStatistics const & HttpFetch::http_stat()
        {
            return http_stat_;
        }
        
        void HttpFetch::handle_fetch(
            boost::system::error_code const & ec)
        {
            http_stat_.end_try(http_.stat(), ec);
            if (!ec) {
            } else {
                if (!canceled_ && (++try_times_ == 1 || util::protocol::HttpClient::recoverable(ec))) {
                    LOG_S(Logger::kLevelDebug, "[handle_fetch] ec: " << ec.message());
                    http_stat_.begin_try();
                    http_.request_head().host.reset(server_host_.host_svc());
                    http_.async_fetch(http_.request_head(),
                        boost::bind(&HttpFetch::handle_fetch, this, _1));
                    return;
                }
                LOG_S(Logger::kLevelAlarm, "[handle_fetch] ec: " << ec.message());
            }
            //returned_ = 1;
            response(ec);
        }


        void HttpFetch::response(
            boost::system::error_code const & ec)
        {
            response_type resp;
            resp.swap(resp_);
            resp(ec, http_.response().data());
        }
        
    } // namespace HttpFetch::
} // namespace ppbox
