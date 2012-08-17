//SegmentBase.h

#ifndef _PPBOX_COMMON_SEGMENT_BASE_H_
#define _PPBOX_COMMON_SEGMENT_BASE_H_


#include "ppbox/common/Call.h"
#include "ppbox/common/Create.h"

#include <framework/string/Url.h>
#include <framework/network/NetName.h>

#include <map>

#define PPBOX_REGISTER_SEGMENT(n, c) \
    static ppbox::common::Call reg ## n(ppbox::common::SegmentBase::register_segment, BOOST_PP_STRINGIZE(n), ppbox::common::Creator<c>())

namespace ppbox 
{
    namespace common 
    {

        struct SegmentInfo
        {
            SegmentInfo()
                : head_size(0)
                , size(0)
                , time(0)
            {
            }

            boost::uint64_t head_size;
            boost::uint64_t size;
            boost::uint64_t time;
        };

        struct DurationInfo
        {   
            DurationInfo()
                : redundancy(0)
                , begin(0)
                , end(0)
                , total(0)
            {
            }

            boost::uint32_t redundancy;
            boost::uint32_t begin;
            boost::uint32_t end;
            boost::uint32_t total;
            boost::uint32_t interval;
        }; 


        class SegmentBase
        {

        public:

            enum OpenMode
            {
                fast = 1,
                slow
            };

            typedef boost::function<
                void(boost::system::error_code const &) > 
                response_type;

            typedef boost::function<SegmentBase * (
                boost::asio::io_service & )
            > register_type;


        public:
            SegmentBase(
                boost::asio::io_service & io_svc);

            ~SegmentBase();

        public:
            static SegmentBase * create(
                boost::asio::io_service & io_svc,
                framework::string::Url const & playlink);

            static void register_segment(
                std::string const & name,
                register_type func);

        public:
            virtual void set_url(
                framework::string::Url const &url);

            virtual void async_open(
                OpenMode mode,
                response_type const & resp) = 0;

            virtual void cancel() = 0;

            virtual void close()= 0;

            virtual boost::system::error_code get_duration(
                DurationInfo & info,
                boost::system::error_code & ec) = 0;

        public:
            virtual size_t segment_count() const = 0;

            virtual std::string const segment_protocol() const
            {
                return get_protocol();
            }

            virtual boost::system::error_code segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) = 0;

            virtual void segment_info(
                size_t segment, 
                SegmentInfo & info) const = 0;

        public:
            std::string const & get_protocol() const
            {
                return url_.protocol();
            }

        protected:
            framework::string::Url url_;

        private:
            static std::map<std::string, register_type> & segment_map();
        };//SegmentBase

    }//common
}//ppbox

#endif//_PPBOX_COMMON_SEGMENT_BASE_H_
