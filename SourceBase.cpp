//SourceBase.cpp
#include "ppbox/common/Common.h"
#include "ppbox/common/SegmentBase.h"
#include "ppbox/common/SourceBase.h"


namespace ppbox
{
    namespace common
    {

        std::map< std::string, std::pair<SourceBase::register_type , boost::uint8_t> > & SourceBase::source_map()
        {
            static std::map< std::string, std::pair<SourceBase::register_type , boost::uint8_t> > get_map;
            return get_map;
        }

        SourceBase::SourceBase(
            boost::asio::io_service & io_svc)
            : ios_(io_svc)
        {}

        SourceBase::~SourceBase(){}

        SourceBase * SourceBase::create(
            boost::asio::io_service & io_svc,
            std::string const & playlink)
        {
            SourceBase* SourceBase = NULL;
            std::string::size_type pos_colon = playlink.find("://");
            std::string proto = "ppvod";
            if (pos_colon == std::string::npos) {
                pos_colon = 0;
            } else {
                proto = playlink.substr(0, pos_colon);
                pos_colon += 3;
            }

            register_type func = NULL;
            std::map< std::string, std::pair<register_type , boost::uint8_t> >::iterator iter = source_map().find(proto);
            if (source_map().end() == iter) {
                return NULL;
            }
            func = iter->second.first;
            SourceBase = func(io_svc);

            return SourceBase;
        }

        void SourceBase::register_source(
            std::string const & name, 
            boost::uint8_t priority, 
            register_type func)
        {
            std::map< std::string, std::pair<register_type , boost::uint8_t> >::iterator iter = source_map().find(name);
            std::pair<register_type , boost::uint8_t> p;
            p = std::make_pair(func, priority);
            std::pair<std::string, std::pair<register_type , boost::uint8_t>> source_base;
            source_base = std::make_pair(name, p);
            if (source_map().end() == iter) {
                source_map().insert(source_base);
            } else {
                if (source_map()[name].second < priority) {
                    source_map().erase(iter);
                    source_map().insert(source_base);
                }
            }
            return;
        }

    }//common
}//ppbox