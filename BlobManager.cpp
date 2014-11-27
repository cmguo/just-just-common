// BlobManager.cpp

#include "just/common/Common.h"
#include "just/common/BlobManager.h"

namespace just
{
    namespace common
    {

        struct BlobManager::Blob
        {
            Blob(
                boost::asio::const_buffer const & data, 
                bool remove_on_get)
            {
                boost::uint8_t const * buf = 
                    boost::asio::buffer_cast<boost::uint8_t const *>(data);
                size_t size = boost::asio::buffer_size(data);
                this->data.assign(buf, buf + size);
                this->remove_on_get = remove_on_get;
            }
            std::vector<boost::uint8_t> data;
            bool remove_on_get;
        };

        BlobManager::BlobManager()
        {
        }

        boost::system::error_code BlobManager::insert(
            std::string const & key, 
            boost::asio::const_buffer const & data, 
            bool remove_on_get)
        {
            typedef std::map<std::string, Blob *>::iterator iterator;
            std::pair<iterator, bool> result =  blobs_.insert(
                std::make_pair(key, new Blob(data, remove_on_get)));
            return result.second 
                ? boost::system::error_code() 
                : framework::system::logic_error::item_already_exist;
        }

        boost::system::error_code BlobManager::get(
            std::string const & key, 
            boost::asio::const_buffer & data)
        {
            std::map<std::string, Blob *>::iterator iter = blobs_.find(key);
            if (iter == blobs_.end()) {
                return framework::system::logic_error::item_not_exist;
            } else {
                if (iter->second->remove_on_get) {
                    cache_.swap(iter->second->data);
                    data = boost::asio::buffer(cache_);
                    delete iter->second;
                    blobs_.erase(iter);
                } else {
                    data = boost::asio::buffer(iter->second->data);
                }
                return boost::system::error_code();
            }
        }

        boost::system::error_code BlobManager::remove(
            std::string const & key)
        {
            std::map<std::string, Blob *>::iterator iter = blobs_.find(key);
            if (iter == blobs_.end()) {
                return framework::system::logic_error::item_not_exist;
            } else {
                delete iter->second;
                blobs_.erase(iter);
                return boost::system::error_code();
            }
        }

    } // namespace common
} // namespace just
