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
                std::string const & playlink);

            static void register_segment(
                std::string const & name,
                register_type func);

        public:
            virtual void set_url(
                std::string const &url);

            virtual void async_open(
                OpenMode mode,
                response_type const & resp) = 0;

            virtual void cancel
                (boost::system::error_code & ec) = 0;

            virtual void close(
                boost::system::error_code & ec)= 0;

            virtual boost::system::error_code reset(
                size_t & time) = 0;

            virtual bool is_open() = 0;

            virtual size_t segment_count() =0;

            virtual boost::system::error_code segment_url(
                size_t segment, 
                framework::string::Url& url,
                boost::system::error_code & ec) = 0;

            virtual void segment_info(
                size_t segment, 
                SegmentInfo & info) = 0;

            virtual bool next_segment(
                size_t segment,
                boost::uint32_t & out_time) = 0;

            virtual void update_segment(
                size_t segment) = 0;

            virtual void update_segment_file_size(
                size_t segment, boost::uint64_t fsize) = 0;

            virtual void update_segment_duration(
                size_t segment,
                boost::uint32_t time) = 0;

            virtual void update_segment_head_size(
                size_t segment, 
                boost::uint64_t hsize) = 0;

            virtual boost::system::error_code get_duration(
                DurationInfo & info,
                boost::system::error_code & ec) = 0;

        protected:
            framework::string::Url url_;

        private:
            static std::map<std::string, register_type> & segment_map();
        };//SegmentBase

    }//common
}//ppbox

#endif//_PPBOX_COMMON_SEGMENT_BASE_H_
