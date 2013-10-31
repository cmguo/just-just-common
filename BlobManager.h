// BlobManager.h

#ifndef _PPBOX_COMMON_BLOB_MANAGER_H_
#define _PPBOX_COMMON_BLOB_MANAGER_H_

#include <boost/asio/buffer.hpp>

namespace ppbox
{
    namespace common
    {

        class BlobManager
        {
        public:
            BlobManager();

        public:
            boost::system::error_code insert(
                std::string const & key, 
                boost::asio::const_buffer const & data, 
                bool remove_on_get);

            boost::system::error_code get(
                std::string const & key, 
                boost::asio::const_buffer & data);

            boost::system::error_code remove(
                std::string const & key);

        private:
            struct Blob;
            std::map<std::string, Blob *> blobs_;
            std::vector<boost::uint8_t> cache_;
        };

        inline BlobManager & blob_manager()
        {
            static BlobManager manager;
            return manager;
        }

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_BLOB_MANAGER_H_
