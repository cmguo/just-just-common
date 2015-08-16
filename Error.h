// Error.h

#ifndef _JUST_COMMON_ERROR_H_
#define _JUST_COMMON_ERROR_H_

namespace just
{
    namespace common
    {

        namespace error
        {
            enum errors
            {
                invalid_url = 1, 
            };

            namespace detail {

                class common_category
                    : public boost::system::error_category
                {
                public:
                    common_category()
                    {
                        register_category(*this);
                    }

                    const char* name() const BOOST_SYSTEM_NOEXCEPT
                    {
                        return "common";
                    }

                    std::string message(int value) const
                    {
                        switch (value) {
                            case invalid_url:
                                return "common: invalid url";
                            default:
                                return "commoner: unknown error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::common_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace error

    } // namespace common
} // namespace just

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<just::common::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using just::common::error::make_error_code;
#endif

    }
}

#endif // _JUST_COMMON_ERROR_H_
