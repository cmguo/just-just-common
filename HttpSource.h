// HttpSource.h
#ifndef _PPBOX_COMMON_HTTPSOURCE_H_
#define _PPBOX_COMMON_HTTPSOURCE_H_

#include "ppbox/common/SourceBase.h"
#include "ppbox/common/SegmentBase.h"

#include <util/protocol/http/HttpClient.h>

namespace ppbox
{
    namespace common
    {

        class HttpSource
            : public SourceBase
        {
        public:
            HttpSource(
                boost::asio::io_service & io_svc);
            ~HttpSource();

            boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);
           
            bool is_open(
                boost::system::error_code & ec);

            boost::system::error_code cancel(
                size_t segment, 
                boost::system::error_code & ec);

            boost::system::error_code close(
                size_t segment, 
                boost::system::error_code & ec);

            std::size_t read(
                const write_buffer_t & buffers,
                boost::system::error_code & ec);

            void async_read(
                const write_buffer_t & buffers,
                read_handle_type handler);

            boost::uint64_t total(
                boost::system::error_code & ec);

            boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            void set_http_connection(
                util::protocol::http_field::Connection::TypeEnum connection);

            bool continuable(
                boost::system::error_code const & ec);

            bool recoverable(
                boost::system::error_code const & ec);

            void on_seg_beg(
                size_t segment);

            void on_seg_end(
                size_t segment);
            util::protocol::HttpClient::Statistics const & http_stat();

            void set_demux_statu() {}


        protected:
            bool flag_;
            framework::network::NetName addr_;
            util::protocol::HttpRequest request_;
            util::protocol::HttpClient http_;
        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_HTTPSOURCE_H_
