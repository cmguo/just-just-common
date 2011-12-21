// BigMp4.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/VodVector.h"

namespace ppbox
{
    namespace common
    {

        VodVector::VodVector()
        {
        }

        VodVector::~VodVector()
        {
        }

        void VodVector::push_back(void* h)
        {
            boost::mutex::scoped_lock lc(mutex_);
            vec_.push_back(h);
        }

        void VodVector::erase(void* h)
        {
            boost::mutex::scoped_lock lc(mutex_);
            std::vector<void*>::iterator iter = vec_.begin();
            for(; iter != vec_.end(); ++iter)
            {
                if(*iter == h)
                {
                    vec_.erase(iter);
                    return;
                }
            }
            
        }

        bool VodVector::find(void* h)
        {
            boost::mutex::scoped_lock lc(mutex_);
            // std::vector<void*> vec_;
            std::vector<void*>::iterator iter = vec_.begin();
            for(; iter != vec_.end(); ++iter)
            {
                if(*iter == h)
                {
                    return true;
                }
            }

            return false;
        }

        size_t VodVector::size()
        {
            boost::mutex::scoped_lock lc(mutex_);
            return vec_.size();
        }
    } // namespace HttpFetchManager
} // namespace ppbox
