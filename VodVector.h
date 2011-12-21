// Vod.h

#ifndef _PPBOX_COMMON_VECTOR_H_
#define _PPBOX_COMMON_VECTOR_H_

#include <vector>

namespace ppbox
{
    namespace common
    {
        class VodVector
        {
        public:
            VodVector();
            ~VodVector();

            void push_back(void* h);
            void erase(void* h);
            bool find(void* h);
            size_t size();

        private:
            std::vector<void*> vec_;
            boost::mutex mutex_;
            ///boost::mutex::scoped_lock lc(mutex_);

        };
    } // namespace vod
} // namespace ppbox

#endif // _PPBOX_VOD_HTTP_FETCH_MANAGER_H_
