// HttpStatistics.h

#ifndef _PPBOX_COMMON_HTTP_STATISTICS_H_
#define _PPBOX_COMMON_HTTP_STATISTICS_H_

#include <util/protocol/http/HttpClient.h>

namespace ppbox
{
    namespace common
    {

        struct HttpStatistics
            : util::protocol::HttpClient::Statistics
        {
            HttpStatistics()
                : try_times(0)
                , total_elapse((boost::uint32_t)-1)
            {
                util::protocol::HttpClient::Statistics::reset();
            }

            void reset()
            {
                util::protocol::HttpClient::Statistics::reset();
                try_times = 0;
                total_elapse = (boost::uint32_t)-1;
                last_last_error.clear();
            }

            void begin_try()
            {
                if (try_times == 0) {
                    reset();
                }
                ++try_times;
            }

            void end_try(
                util::protocol::HttpClient::Statistics const & stat)
            {
                if (try_times == 1) {
                    (util::protocol::HttpClient::Statistics &)(*this) = stat;
                }
                total_elapse = elapse();
                last_last_error = ((util::protocol::HttpClient::Statistics &)(*this)).last_error;
            }

            void end_try(
                util::protocol::HttpClient::Statistics const & stat, 
                boost::system::error_code const & ec)
            {
                if (try_times == 1) {
                    (util::protocol::HttpClient::Statistics &)(*this) = stat;
                    ((util::protocol::HttpClient::Statistics &)(*this)).last_error = ec;
                }
                total_elapse = elapse();
                last_last_error = ec;
            }

            size_t try_times;
            boost::uint32_t total_elapse;
            boost::system::error_code last_last_error;
        };

    } // namespace demux
} // namespace ppbox

#endif // _PPBOX_COMMON_HTTP_STATISTICS_H_
