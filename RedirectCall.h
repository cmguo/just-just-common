// RedirectCall.h

#ifndef JUST_COMMON_REDIRECT_CALL_H_
#define JUST_COMMON_REDIRECT_CALL_H_

#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/comma_if.hpp>

#include <stdarg.h>

namespace just
{
    namespace common
    {

        class RedirectCall
        {
        public:
            typedef void const * context_t;

            typedef void const * caller_t;

            typedef void (*call_t)(
                context_t, // context
                caller_t, // real call
                void *, // result
                va_list); // call args

            typedef void (*free_t)(
                context_t); // context

        public:
            RedirectCall(
                call_t call = NULL, 
                free_t free = NULL, 
                context_t context = NULL);

        public:
            void set(
                call_t call, 
                free_t free, 
                context_t context);

        public:
#define VA_OP_0(z, n, d) typename BOOST_PP_CAT(A, n)
#define VA_OP_1(z, n, d) BOOST_PP_CAT(A, n)
#define VA_OP_2(z, n, d) BOOST_PP_CAT(A, n) BOOST_PP_CAT(a, n)
#define VA_OP_3(z, n, d) BOOST_PP_CAT(a, n)

#define BOOST_PP_LOCAL_MACRO(n) \
            template <typename R BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM(n, VA_OP_0, _)> \
            R call( \
                R (*f)(BOOST_PP_ENUM(n, VA_OP_1, _)) BOOST_PP_COMMA_IF(n) \
                BOOST_PP_ENUM(n, VA_OP_2, _))\
            { \
                if (call_) { \
                    R r; \
                    redirect_call((caller_t)f, &r BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM(n, VA_OP_3, _)); \
                    return r; \
                } else { \
                    return f(BOOST_PP_ENUM(n, VA_OP_3, _)); \
                } \
            }

#define BOOST_PP_LOCAL_LIMITS (0, 5)
#include BOOST_PP_LOCAL_ITERATE()

            void call( 
                void (*f)())
            { 
                if (call_) { 
                    redirect_call((caller_t)f, NULL); 
                    return; 
                } else { 
                    return f(); 
                } 
            }

#define BOOST_PP_LOCAL_MACRO(n) \
            template <BOOST_PP_ENUM(n, VA_OP_0, _)> \
            void call( \
                void (*f)(BOOST_PP_ENUM(n, VA_OP_1, _)), \
                BOOST_PP_ENUM(n, VA_OP_2, _)) \
            { \
                if (call_) { \
                    redirect_call((caller_t)f, NULL, BOOST_PP_ENUM(n, VA_OP_3, _)); \
                    return; \
                } else { \
                    return f(BOOST_PP_ENUM(n, VA_OP_3, _)); \
                } \
            }

#define BOOST_PP_LOCAL_LIMITS (1, 5)
#include BOOST_PP_LOCAL_ITERATE()

        public:
            void free(
                context_t context)
            {
                if (free_)
                    free_(context);
            }

        private:
            void redirect_call(
                caller_t caller, 
                void * result, 
                ...)
            {
                va_list args;
                va_start(args, result); \
                call_(context_, caller, result, args); \
                va_end(args);
            }

        private:
            call_t call_;
            free_t free_;
            context_t context_;
        };

    } // common
} // just

#endif // JUST_COMMON_REDIRECT_CALL_H_
