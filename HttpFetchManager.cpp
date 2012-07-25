// BigMp4.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/HttpFetchManager.h"


#include <util/archive/XmlIArchive.h>
#include <util/buffers/BufferCopy.h>
#include <util/archive/ArchiveBuffer.h>

#include <framework/logger/LoggerStreamRecord.h>
using namespace framework::logger;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("HttpFetchManager", 0);

using namespace ppbox::vod;
namespace ppbox
{
    namespace common
    {
        HttpFetchManager::HttpFetchManager(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<HttpFetchManager>(daemon, "HttpFetchManager")
        {
        }

        HttpFetchManager::~HttpFetchManager()
        {
        }

        boost::system::error_code HttpFetchManager::startup()
        {
            return boost::system::error_code();
        }
        void HttpFetchManager::shutdown()
        {

        }

        FetchHandle HttpFetchManager::async_fetch(
            framework::string::Url const & url 
            ,framework::network::NetName const & server_host
            ,HttpFetch::response_type const & resp)
        {
            HttpFetch *fetch = new HttpFetch(io_svc());
            vec_.push_back(fetch);

            fetch->async_fetch(
                url
                ,server_host
                ,resp
                );

            return fetch;
        }

        ppbox::common::HttpStatistics const & HttpFetchManager::http_stat(FetchHandle h)
        {
            if(vec_.find(h))
            {
                return ((HttpFetch *)h)->http_stat();
            }
            else
            {
                assert(0);
                //Ê§°ÜµÄ
                static ppbox::common::HttpStatistics g_http_stat;
                return g_http_stat;
            }
        }

        void HttpFetchManager::cancel(FetchHandle h)
        {
            if(vec_.find(h))
            {
                ((HttpFetch *)h)->cancel();
            }
        }

        void HttpFetchManager::close(FetchHandle h)
        {
            if(vec_.find(h))
            {
                ((HttpFetch *)h)->close();
                delete (HttpFetch *)h;
                vec_.erase(h);
            }
        }

    } // namespace HttpFetchManager
} // namespace ppbox
