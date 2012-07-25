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
            typedef Class * result_type;//����������Ҫ���ⶨ�庯����������

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