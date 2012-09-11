// CommonUrl.cpp
#include "ppbox/common/Common.h"
#include "ppbox/common/CommonUrl.h"
#include "ppbox/common/CommonError.h"

#include <framework/string/Base64.h>
#include <framework/string/Slice.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::logger;

#include <security/Des.h>

#include <vector>

namespace ppbox
{
    namespace common
    {
        const std::string url_profix = "base64";

        const static char *gsKey[11] =
        {
            "\x70\x70\x6C\x69\x76\x65\x6F\x6B",
            "\x15\xB9\xFD\xAE\xDA\x40\xF8\x6B\xF7\x1C\x73\x29\x25\x16\x92\x4A\x29\x4F\xC8\xBA\x31\xB6\xE9\xEA",
            "\x29\x02\x8A\x76\x98\xEF\x4C\x6D\x3D\x25\x2F\x02\xF4\xF7\x9D\x58\x15\x38\x9D\xF1\x85\x25\xD3\x26",
            "\xD0\x46\xE6\xB6\xA4\xA8\x5E\xB6\xC4\x4C\x73\x37\x2A\x0D\x5D\xF1\xAE\x76\x40\x51\x73\xB3\xD5\xEC",
            "\x43\x52\x29\xC8\xF7\x98\x31\x13\x19\x23\xF1\x8C\x5D\xE3\x2F\x25\x3E\x2A\xF2\xAD\x34\x8C\x46\x15",
            "\x9B\x29\x15\xA7\x2F\x83\x29\xA2\xFE\x6B\x68\x1C\x8A\xAE\x1F\x97\xAB\xA8\xD9\xD5\x85\x76\xAB\x20",
            "\xB3\xB0\xCD\x83\x0D\x92\xCB\x37\x20\xA1\x3E\xF4\xD9\x3B\x1A\x13\x3D\xA4\x49\x76\x67\xF7\x51\x91",
            "\xAD\x32\x7A\xFB\x5E\x19\xD0\x23\x15\x0E\x38\x2F\x6D\x3B\x3E\xB5\xB6\x31\x91\x20\x64\x9D\x31\xF8",
            "\xC4\x2F\x31\xB0\x08\xBF\x25\x70\x67\xAB\xF1\x15\xE0\x34\x6E\x29\x23\x13\xC7\x46\xB3\x58\x1F\xB0",
            "\x52\x9B\x75\xBA\xE0\xCE\x20\x38\x46\x67\x04\xA8\x6D\x98\x5E\x1C\x25\x57\x23\x0D\xDF\x31\x1A\xBC",
            "\x8A\x52\x9D\x5D\xCE\x91\xFE\xE3\x9E\x9E\xE9\x54\x5D\xF4\x2C\x3D\x9D\xEC\x2F\x76\x7C\x89\xCE\xAB"
        };

        CommonUrl::CommonUrl(framework::string::Url const & url)
        {
            std::string url_path = url.path_all();
            if (url_path.compare(1, url_profix.size(), url_profix) == 0) 
            {
                 std::string tmphost = "http://host";
                url_path = url_path.substr(url_profix.size()+1, url_path.size() - url_profix.size()+1);
                std::vector<std::string> parm;
                framework::string::slice<std::string>(url_path, std::inserter(parm, parm.end()), ".");
                url_path = parm[0];
                url_path = framework::string::Base64::decode(url_path);
                url_path = std::string("/") + url_path;
                tmphost += url_path;
                this->from_string(tmphost);
            }
            else
            {
                this->from_string(url.to_string());
            }
        }
        //3DES解密    url判断是不是 pptv://code 或 pptv:///3des
        //如果没有:// 加上默认的  ppvod:///
        CommonUrl::CommonUrl(std::string const & url)
        {
            std::string tempUrl = url;
            
            if (std::string::npos == tempUrl.find("://") )
            {
                tempUrl = "ppvod:///" + url;
            }
            else if(std::string::npos != tempUrl.find("pptv://") )
            {
               tempUrl = parse_playlink(url);
            }
            from_string(tempUrl);
        }


        CommonUrl::~CommonUrl()
        {

        }

        boost::system::error_code CommonUrl::parse_params(
            std::string  const & play_link,
            std::string & code,
            boost::int32_t& key_index)
        {
            boost::system::error_code ec;
            const std::string http_head = "http://host/play?";
            const std::string url_profix = "pptv://code=";
            const std::string url_3des = "pptv:///3des";

            if (play_link.compare(0, url_profix.size(), url_profix) == 0)
            {
                std::string play_link_t = play_link.substr(std::string("pptv://").size(), play_link.size() - std::string("pptv://").size());
                framework::string::map_find(play_link_t, "code", code, "&");
                framework::string::map_find(play_link_t, "key", key_index, "&");
            }
            else if(play_link.compare(0, url_3des.size(), url_3des) == 0)
            {
                framework::string::Url request_url(play_link);
                code = request_url.path();
                code=code.substr(5,code.size()-5);
                std::string  strKey_index = request_url.param("key");
                framework::string::parse2(strKey_index.c_str(), key_index);
            }
            else
            {
                ec = error::not_support;
            }
            return ec;
        }

        const std::string CommonUrl::parse_playlink(std::string  const & play_link)
        {
            std::string playlink;
            std::string str_code;
            boost::int32_t key_index = 0;
            boost::system::error_code ec = parse_params(play_link,str_code,key_index);
            if(ec || key_index < 1 || key_index > 10) return playlink;

             char out_str[256] = {0};
             if(security::Des::pptv_3_des_d(str_code.c_str(), str_code.size(), gsKey[key_index], 24, out_str, 256)
                 || security::Des::pptv_3_des_d(str_code.c_str(), str_code.size(), gsKey[key_index], 24, out_str, 256))
             {
                out_str[255] = '\0';
                std::string s(out_str);

                std::string vt;
                framework::string::map_find(s, "vt", vt, "&");

                std::string id;
                framework::string::map_find(s, "id", id, "&");

                std::string proto;
                if(vt == "4")
                {
                    proto = "pplive3";
                }
                else if(vt == "3")
                {
                    proto = "ppvod2"; 
                }
                else
                {
                    //LOG_INFO("[create] VT Error vt:"<<vt);
                    return playlink;
                }

                playlink = proto + ":///";
                playlink += id;
                playlink += "?";
                playlink += s;
             }
             return playlink;
        }
    } // namespace common
} // namespace ppbox
