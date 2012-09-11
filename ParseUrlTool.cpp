// ParseUrlTool.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/ParseUrlTool.h"

#include <framework/string/Slice.h>

#include <framework/logger/StreamRecord.h>
using namespace framework::logger;

#include <vector>

namespace ppbox
{
    namespace common
    {
        ParseUrlTool::ParseUrlTool()
        {

        }

        ParseUrlTool::ParseUrlTool(framework::string::Url const & url)
        {
            from_url(url);
        }

        void ParseUrlTool::from_url(framework::string::Url const & url)
        {   
            url_ = url;

            //解playlink
            std::string type = url.param("type");
            playlink_ = url.param("playlink");
            if (!type.empty())
            {
                type = type +  ":///";
                playlink_ = type + playlink_;
            }
            playlink_ = framework::string::Url::decode(playlink_);

            //解format
            format_ = url.param("format");
            if(format_.empty())
            {//如果为空就取play的后缀
                if (url.path().size() > 1)
                {
                    std::string option = url.path().substr(1);
                    std::vector<std::string> dirs;
                    framework::string::slice<std::string>(option, std::inserter(dirs, dirs.end()), "/");
                    boost::uint32_t d_size = dirs.size();
                    if(d_size > 0)
                    {
                        if(d_size > 1)
                        {
                            playid_ = dirs[d_size-2];
                        }
                        option = dirs[d_size-1];
                        std::vector<std::string> parm;
                        framework::string::slice<std::string>(option, std::inserter(parm, parm.end()), ".");
                        if(parm.size() > 0)
                        {
                            option_ = parm[0];
                        }

                        if (parm.size() == 2) 
                        {
                            if (format_.empty())
                                format_ = parm[1];
                        }
                    }
                }
            }
        }

        ParseUrlTool::~ParseUrlTool()
        {

        }

        const std::string& ParseUrlTool::playlink()
        {
            return playlink_;
        }

        const std::string& ParseUrlTool::format()
        {   
            return format_;
        }

        const std::string& ParseUrlTool::option()
        {
            return option_;
        }

        const std::string& ParseUrlTool::playid()
        {
            return playid_;
        }

        framework::string::Url& ParseUrlTool::url()
        {
            return url_;
        }
    } // namespace common
} // namespace ppbox
