// Common.h
#if defined( PPBOX_SINGLE_PROCESS )
#define FRAMEWORK_MESSAGEQUEUE_SINGLE_PROCESS
#endif

#if (!defined(PPBOX_DISABLE_PEER) && !defined(PPBOX_DISABLE_VOD))
#define PPBOX_DISABLE_PEER
#endif

#include <util/Util.h>

#include "ppbox/common/CommonModuleBase.h"
