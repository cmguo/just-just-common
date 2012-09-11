#ifndef _PPBOX_COMMON_TEMP_SINK_H_
#define _PPBOX_COMMON_TEMP_SINK_H_

#include "ppbox/common/CommonError.h"

#include <util/stream/Sink.h>

namespace ppbox
{
    namespace common
    {
        class TempSink : public util::stream::Sink
        {
        public:
            TempSink(
                boost::asio::io_service & io_svc)
                : util::stream::Sink(io_svc)
            {}

            virtual ~TempSink()
            {}

        private:
            virtual std::size_t private_write_some(
                util::stream::StreamConstBuffers const & buffers,
                boost::system::error_code & ec)
            {
                ec = error::not_support;
                return 0;
            }
        };

    } // namespace mux
} // namespace ppbox

#endif 
