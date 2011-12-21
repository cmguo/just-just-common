// Vod.h

#ifndef _PPBOX_COMMON_HTTP_FETCH_MANAGER_H_
#define _PPBOX_COMMON_HTTP_FETCH_MANAGER_H_

#include "ppbox/common/HttpFetch.h"
#include "ppbox/common/VodVector.h"

#include <boost/asio/streambuf.hpp>
#include <ppbox/common/HttpStatistics.h>

#include <ppbox/common/CommonModuleBase.h>


namespace ppbox
{
    namespace common
    {
        typedef void* FetchHandle;

        class HttpFetchManager
            : public ppbox::common::CommonModuleBase<HttpFetchManager>
        {
        private:
            virtual boost::system::error_code startup();
            virtual void shutdown();

        public:
            HttpFetchManager(util::daemon::Daemon & daemon);
            ~HttpFetchManager();

            FetchHandle async_fetch(
                framework::string::Url const & url 
                ,framework::network::NetName const & server_host
                ,HttpFetch::response_type const & resp);

            void cancel(FetchHandle h);

            void close(FetchHandle h);

            ppbox::common::HttpStatistics const & http_stat(FetchHandle h);

        private:
            VodVector vec_;  //±£´æhandel
        };
    } // namespace vod
} // namespace ppbox

#endif // _PPBOX_VOD_HTTP_FETCH_MANAGER_H_
