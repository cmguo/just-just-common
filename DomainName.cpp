// DomainName.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/DomainName.h"

#include <framework/system/FileTag.h>

namespace ppbox
{
    namespace common
    {

        boost::system::error_code domain_name_get(
            std::string const & file, 
            std::string const & domain, 
            std::string & value)
        {
            framework::system::FileTag ft(file, "domain_name");
            return ft.get(domain, value);
        }

        boost::system::error_code domain_name_set(
            std::string const & file, 
            std::string const & domain, 
            std::string const & value)
        {
            framework::system::FileTag ft(file, "domain_name");
            return ft.set(domain, value);
        }

        boost::system::error_code domain_name_get_all(
            std::string const & file, 
            std::vector<std::pair<std::string, std::string> > & values)
        {
            framework::system::FileTag ft(file, "domain_name");
            return ft.get_all(values);
        }

    } // namespace common
} // namespace ppbox
