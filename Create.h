// Create.h

#ifndef _PPBOX_COMMON_CREATE_H_
#define _PPBOX_COMMON_CREATE_H_

#include <boost/function.hpp>

namespace ppbox
{
    namespace common
    {

        template <
            typename Class
        >
        struct Creator
        {
            typedef Class * result_type;//����������Ҫ���ⶨ�庯����������

            Class * operator()()
            {
                return new Class;
            }

            template <
                typename Arg1
            >
            Class * operator()(
                Arg1 & arg1)
            {
                return new Class(arg1);
            }

            template <
                typename Arg1,
                typename Arg2
            >
            Class * operator()(
                Arg1 & arg1, Arg2 & arg2)
            {
                return new Class(arg1, arg2);
            }

            template <
                typename Arg1,
                typename Arg2,
                typename Arg3
            >
            Class * operator()(
                Arg1 & arg1, Arg2 & arg2, Arg3 & arg3)
            {
                return new Class(arg1, arg2, arg3);
            }
        };

    } // namespace commom
} // namespace ppbox

#endif // _PPBOX_COMMON_CREATE_H_
