// ClassRegister.h

#ifndef PPBOX_DELAY_REGISTER_CLASS

#  ifndef PPBOX_REGISTER_CLASS_NAME

#    ifdef PPBOX_REGISTER_CLASS
#      undef PPBOX_REGISTER_CLASS
#      undef PPBOX_REGISTER_CLASS_FUNC
#      undef PPBOX_REGISTER_CLASS_FACTORY
#      undef PPBOX_REGISTER_CLASS_FACTORY_FUNC
#    endif

#    include "ppbox/common/Create.h"
#    include "ppbox/common/Call.h"

#    define PPBOX_REGISTER_CLASS_NAME(cls) BOOST_PP_CAT(reg_class_, BOOST_PP_CAT(cls, __LINE__))
#    define PPBOX_REGISTER_CLASS(key, cls) \
         static ppbox::common::Call PPBOX_REGISTER_CLASS_NAME(cls)(cls::register_creator, key, ppbox::common::Creator<cls>())
#    define PPBOX_REGISTER_CLASS_FUNC(key, cls, func) \
         static ppbox::common::Call PPBOX_REGISTER_CLASS_NAME(cls)(cls::register_creator, key, func)
#    define PPBOX_REGISTER_CLASS_FACTORY(key, fact, cls) \
         static ppbox::common::Call PPBOX_REGISTER_CLASS_NAME(cls)(fact::register_creator, key, ppbox::common::Creator<cls>())
#    define PPBOX_REGISTER_CLASS_FACTORY_FUNC(key, fact, func) \
         static ppbox::common::Call PPBOX_REGISTER_CLASS_NAME(cls)(fact::register_creator, key, func)

#  endif // PPBOX_REGISTER_CLASS_NAME

#else // PPBOX_DELAY_REGISTER_CLASS

#  ifndef PPBOX_REGISTER_CLASS_NAME
#    define PPBOX_REGISTER_CLASS(key, cls)
#    define PPBOX_REGISTER_CLASS_FUNC(key, cls, func)
#    define PPBOX_REGISTER_CLASS_FACTORY(key, fact, cls)
#    define PPBOX_REGISTER_CLASS_FACTORY_FUNC(key, fact, func)
#  endif

#endif // PPBOX_DELAY_REGISTER_CLASS
