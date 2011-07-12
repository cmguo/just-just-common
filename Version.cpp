// Version.cpp

#include "ppbox/common/Common.h"
#define VERSION_SOURCE
#include "ppbox/common/Version.h"

#include <framework/logger/LoggerStreamRecord.h>

#include <iostream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("Ppbox", 0)

namespace ppbox
{
    namespace common
    {

        void log_versions()
        {
            for (std::map<char const *, char const *>::const_iterator iter = 
                framework::system::version_collection().begin(); iter != 
                framework::system::version_collection().end(); ++iter) {
                    std::cout << iter->first << " " << iter->second << std::endl;
                    LOG_S(framework::logger::Logger::kLevelError, iter->first << " " << iter->second);
            }
        }

    } // namespace common
} // namespace ppbox
