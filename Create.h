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

            template <
                typename Arg1,
                typename Arg2
            >
            Class * operator()(
                Arg1 & arg1, Arg2 & arg2)
            {
                return new Class(arg1, arg2);
            }

            Class * operator()()
            {
                return new Class;
            }
        };

        template <
            typename KeyType, 
            typename CreateProto
        >
        class ClassFactory
        {
        private:
            typedef KeyType key_type;
            typedef boost::function<CreateProto> creator_type;
            typedef typename creator_type::result_type class_type;
            typedef std::map<KeyType, creator_type> creator_map_type;

        public:
            template <
                typename CreatorType
            >
            static void register_class(
                key_type const & key, 
                CreatorType creator)
            {
                creator_map.insert(std::make_pair(key, creator_type(creator)));
            }

            template <
                typename Arg1
            >
            static class_type create(
                key_type const & key, 
                Arg1 & arg1)
            {
                creator_map_type::const_iterator iter = 
                    creator_map.find(key);
                if (iter == creator_map.end())
                    return class_type();
                return iter.second(arg1);
            }

            template <
                typename Arg1, 
                typename Arg2
            >
            static class_type create(
                key_type const & key, 
                Arg1 & arg1, 
                Arg1 & arg2)
            {
                creator_map_type::const_iterator iter = 
                    creator_map.find(key);
                if (iter == creator_map.end())
                    return class_type();
                return iter.second(arg1, arg2);
            }

        private:
            typedef boost::function<CreateProto> creator_type;
            static creator_map_type creator_map;
        };

        template <
            typename KeyType, 
            typename CreateProto
        >
        typename ClassFactory<KeyType, CreateProto>::creator_map_type ClassFactory<KeyType, CreateProto>::creator_map;

    }
}

#endif
