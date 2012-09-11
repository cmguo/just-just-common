// Sinks.h

#ifndef _PPBOX_COMMON_SINKS_H_
#define _PPBOX_COMMON_SINKS_H_

#include "ppbox/common/TempSink.h"

#include <util/stream/Sink.h>

#include <vector>

namespace ppbox
{
    namespace common
    {
        class Sinks
        {
        public:
            Sinks();
            ~Sinks();

            void add(size_t control, util::stream::Sink* sink);

            void clean();

            util::stream::Sink* operator [] (size_t index);
        private:
            std::vector<util::stream::Sink*> sinks_;
            util::stream::Sink* default_sink_;
            TempSink tmpSink;
        };
    } // namespace mux
} // namespace ppbox

#endif // _PPBOX_COMMON_URL_H_
