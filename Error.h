// Error.h

#ifndef _PPBOX_COMMON_ERROR_H_
#define _PPBOX_COMMON_ERROR_H_

namespace ppbox
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

                    const char* name() const
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
} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::common::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::common::error::make_error_code;
#endif

    }
}

#endif // _PPBOX_COMMON_ERROR_H_
