// Vod.h

#ifndef _PPBOX_COMMON_HTTPFETCH_H_
#define _PPBOX_COMMON_HTTPFETCH_H_

#include "ppbox/common/HttpStatistics.h"

#include <util/protocol/http/HttpClient.h>
#include <framework/network/NetName.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/streambuf.hpp>

namespace ppbox
{
    namespace common
    {
        class HttpFetch
        {
        public:

            typedef boost::function<void (
                boost::system::error_code const &, 
                boost::asio::streambuf &)
            > response_type;

            HttpFetch(boost::asio::io_service & io_svc);
            ~HttpFetch();

            void async_fetch(
                framework::string::Url const & url 
                ,framework::network::NetName const & server_host
                ,response_type const & resp);

            HttpStatistics const & http_stat();

            void cancel();

            void close();

        private:
            void handle_fetch(
                boost::system::error_code const & ec);
            void response(
                boost::system::error_code const & ec);


        private:
            util::protocol::HttpClient http_;

            HttpStatistics http_stat_;

            framework::network::NetName server_host_;

            bool canceled_;
            response_type resp_;
            size_t try_times_;

        };
    } // namespace vod
} // namespace ppbox

#endif // _PPBOX_VOD_HTTPFETCH_H_
