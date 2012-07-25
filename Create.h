//Create.h
#ifndef PPBOX_COMMON_CREATE_H_
#define PPBOX_COMMON_CREATE_H_

namespace ppbox
{
    namespace common
    {
        template <
            typename Class
        >
        struct Creator
        {
            typedef Class * result_type;//函数对象，需要对外定义函数返回类型

            template <
                typename Arg1
            >
            Class * operator()(
                Arg1 & arg1)
            {
                return new Class(arg1);
            }
        };

    }
}

#endif
