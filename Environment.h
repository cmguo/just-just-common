// Environment.h

#ifndef _JUST_COMMON_ENVIRONMENT_H_
#define _JUST_COMMON_ENVIRONMENT_H_

#include <framework/system/Version.h>

namespace just
{
    namespace common
    {

        struct Environment
        {
            Environment();

            std::string get_bid() const;

            framework::system::Version version;

            std::string hid;
            std::string aid;
            std::string mid;

        };

    } // namespace common
} // namespace just

#endif // _JUST_COMMON_ENVIRONMENT_H_
