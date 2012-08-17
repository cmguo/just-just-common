// SourceBase.h
#ifndef _PPBOX_COMMON_SOURCE_BASE_H_
#define _PPBOX_COMMON_SOURCE_BASE_H_

#include <ppbox/common/Call.h>
#include <ppbox/common/Create.h>

#include <util/buffers/Buffers.h>

#include <framework/string/Url.h>

#include <boost/asio/read.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <map>

#define PPBOX_REGISTER_SOURCE(n, c) \
    static ppbox::common::Call reg_ ## n(ppbox::common::SourceBase::register_source, BOOST_PP_STRINGIZE(n), ppbox::common::Creator<c>())

namespace ppbox
{
    namespace common
    {

        class SourceBase
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &)
            > response_type;

            typedef boost::function<SourceBase * (
                boost::asio::io_service & )
            > register_type;

            typedef boost::function<void(
                boost::system::error_code const &,
                size_t)
            > read_handle_type;

            typedef util::buffers::Buffers<
                boost::asio::mutable_buffer, 2
            > write_buffer_t;

        public:
            static void register_source(
                std::string const & name,
                register_type func);
            
            static SourceBase * create(
                boost::asio::io_service & io_svc,
                std::string const & proto);

            static void destory(
                SourceBase * SourceBase);

        public:
            SourceBase(
                boost::asio::io_service & io_svc);

            virtual ~SourceBase();

            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec)= 0;

            virtual void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp)= 0;

            virtual bool is_open(
                boost::system::error_code & ec)= 0;

            virtual boost::system::error_code cancel(
                size_t segment, 
                boost::system::error_code & ec)= 0;

            virtual boost::system::error_code close(
                size_t segment, 
                boost::system::error_code & ec)= 0;

            virtual std::size_t read(
                const write_buffer_t & buffers,
                boost::system::error_code & ec)= 0;

            virtual void async_read(
                const write_buffer_t & buffers,
                read_handle_type handler)= 0;

            virtual boost::uint64_t total(
                boost::system::error_code & ec)= 0;

            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec)= 0;

            virtual boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec)= 0;

            virtual bool continuable(
                boost::system::error_code const & ec)= 0;

            virtual bool recoverable(
                boost::system::error_code const & ec) = 0;

            virtual void on_seg_beg(
                size_t segment) = 0;

            virtual void on_seg_end(
                size_t segment) = 0;

            virtual void set_demux_statu() = 0;

        protected:
            boost::asio::io_service &ios_service() 
            {
                return ios_;
            }
        private:
            boost::asio::io_service &ios_;
            static std::map< std::string, SourceBase::register_type > & source_map();
        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_SOURCE_BASE_H_
