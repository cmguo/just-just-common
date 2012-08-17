//SegmentBase.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/SegmentBase.h"

#include <framework/logger/LoggerStreamRecord.h>
using namespace framework::logger;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("SegmentBase", 0);

namespace ppbox
{
    namespace common
    {

        std::map<std::string, SegmentBase::register_type> & SegmentBase::segment_map()
        {
            static std::map<std::string, SegmentBase::register_type> g_map;
            return g_map;
        }

        SegmentBase::SegmentBase(
            boost::asio::io_service & io_svc)
        {
        }

        SegmentBase::~SegmentBase()
        {
        }

        void SegmentBase::register_segment(
            std::string const & name,
            register_type func)
        {
            segment_map().insert(std::make_pair(name, func));
            return;
        }

        SegmentBase * SegmentBase::create(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
        {
            std::map<std::string, register_type>::iterator iter = 
                segment_map().find(url.protocol());
            if (segment_map().end() == iter) {
                return NULL;
            } else {
                register_type func = iter->second;
                SegmentBase * segment = func(io_svc);
                segment->set_url(url);
                return segment;
            }
        }

        void SegmentBase::set_url(
            framework::string::Url const &url)
        {
            url_ = url;
        }

    }//common
}//ppbox


//SegmentBase.cpp