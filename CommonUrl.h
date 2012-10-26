// CommonUrl.h

#ifndef _PPBOX_COMMON_URL_H_
#define _PPBOX_COMMON_URL_H_

#include <framework/string/Url.h>

namespace ppbox
{
    namespace common
    {

            bool decode_url(
                framework::string::Url & url, 
                boost::system::error_code & ec);

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_URL_H_
