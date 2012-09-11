// CommonUrl.h

#ifndef _PPBOX_COMMON_URL_H_
#define _PPBOX_COMMON_URL_H_

#include <framework/string/Url.h>

namespace ppbox
{
    namespace common
    {
        class CommonUrl : public framework::string::Url
        {
        public:
            CommonUrl(framework::string::Url const & url);
            CommonUrl(std::string const & url);
            ~CommonUrl();

        private:
            //主要是base64解密，及去掉多余信息
            boost::system::error_code parse_params(
                std::string  const & play_link,
                std::string & code,
                boost::int32_t& key_index);

            const std::string parse_playlink(std::string  const & play_link);
        };
    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_URL_H_
