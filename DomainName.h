// DomainName.h

#ifndef _PPBOX_COMMON_DOMAIN_NAME_H_
#define _PPBOX_COMMON_DOMAIN_NAME_H_

#include <framework/network/NetName.h>

#include <boost/preprocessor/stringize.hpp>

#define DEFINE_DOMAIN_NAME(name, value) \
static char const * name ## _string() \
{ \
    char const * tag_version = "!" \
        BOOST_PP_STRINGIZE(name) "_domain_name_tag" \
        value "\000                      "; \
    char const * str = tag_version + sizeof(BOOST_PP_STRINGIZE(name)) + 16; \
    return str; \
} \
static framework::network::NetName const name(name ## _string())

namespace ppbox
{
    namespace common
    {

        boost::system::error_code domain_name_get(
            std::string const & file, 
            std::string const & domain, 
            std::string & value);

        boost::system::error_code domain_name_set(
            std::string const & file, 
            std::string const & domain, 
            std::string const & value);

        boost::system::error_code domain_name_get_all(
            std::string const & file, 
            std::vector<std::pair<std::string, std::string> > & values);

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_DOMAIN_NAME_H_
