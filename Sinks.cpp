// Sinks.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/Sinks.h"

#include <framework/logger/StreamRecord.h>
using namespace framework::logger;


namespace ppbox
{
    namespace common
    {
        Sinks::Sinks(boost::asio::io_service & ios)
            : tmpSink(ios)
        {
            default_sink_ = &tmpSink;
        }

        Sinks::~Sinks()
        {

        }

        void Sinks::clean()
        {
            default_sink_ = &tmpSink;
            sinks_.clear();
        }


        void Sinks::add(size_t control, util::stream::Sink* sink)
        {
            if((size_t)-1 == control)
            {
                default_sink_ = sink;
            }
            else
            {
                if (control < sinks_.size())
                {
                    sinks_[control] = sink;
                }
                else if(control == sinks_.size())
                {
                    sinks_.push_back(sink);
                }
                else
                {
                    while(sinks_.size() < control)
                    {
                        sinks_.push_back(&tmpSink);
                    }
                    sinks_.push_back(sink);
                }
            }
        }

        util::stream::Sink* Sinks::operator [] (size_t index)
        {
            if(index< sinks_.size())
            {
                return sinks_[index];
            }
            else
            {
                return default_sink_;
            }
        }

    } // namespace common
} // namespace ppbox
