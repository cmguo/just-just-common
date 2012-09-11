// ParseUrlTool.h
#ifndef _PPBOX_COMMON_PARSE_URL_TOOL_H_
#define _PPBOX_COMMON_PARSE_URL_TOOL_H_

#include <framework/string/Url.h>

namespace ppbox
{
    namespace common
    {
        class ParseUrlTool
        {
        public:
            ParseUrlTool();
            ParseUrlTool(framework::string::Url const & url);
            ~ParseUrlTool();
            void from_url(framework::string::Url const & url);

            const std::string& playlink();
            const std::string& format();
            const std::string& option();
            const std::string& playid();
            
            framework::string::Url& url();

        private:
            std::string playlink_;
            std::string format_;
            std::string option_;
            std::string playid_;

            framework::string::Url url_;
        };
    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_PARSE_URL_TOOL_H_
