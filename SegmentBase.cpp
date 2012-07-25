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
        }

        SegmentBase * SegmentBase::create(
            boost::asio::io_service & io_svc,
            std::string const & playlink)
        {
            std::string::size_type pos_colon = playlink.find("://");
            std::string name = "ppvod";
            if (pos_colon == std::string::npos) {
                pos_colon = 0;
            } else {
                name = playlink.substr(0, pos_colon);
                pos_colon += 3;
            }
            std::map<std::string, register_type>::iterator iter = segment_map().find(name);
            if (segment_map().end() == iter) {
                return NULL;
            } else {
                register_type func = iter->second;
                return func(io_svc);
            }
        }

        void SegmentBase::set_url(
            std::string const &url)
        {
            url_.from_string(url);
        }

    }//common
}//ppbox


//SegmentBase.cpp