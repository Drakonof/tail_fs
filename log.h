/*
Created by Sergey Trubin at 2005-03-01 17:27:26

Update  by Sergey Trubin    2019-03-08  0:38
Update  by Sergey Trubin    2019-07-04 15:14
Update  by Sergey Trubin    2020-02-06 13:46
Update  by Sergey Trubin    2021-11-15 17:27
*/

#ifndef LOG_H
#define LOG_H 1

# ifndef NOT_LOG

#  ifdef __cplusplus

#   include <iostream>

#   ifndef __
#     define __  << " :\t"  <<
#   endif // __

#    define __LOG_H_LINE_ARGS__ << __FILE__ << '[' << __LINE__ << ']' << ' ' << __func__ << "\t-> "

#    define LOG(t)  std::cout __LOG_H_LINE_ARGS__ <<       t << std::endl
#    define ERR(t)  std::cerr __LOG_H_LINE_ARGS__ <<       t << std::endl
#    define LOG1(t) std::cout __LOG_H_LINE_ARGS__ << #t __ t << std::endl

#    define SHW(t)  #t << '=' << t  << '\t'
#    define ___(t)  SHW(t)
#    define   W(t)  SHW(t)

# else // __cplusplus

#    include <stdio.h>

#    define __LOG_H_LINE_ARGS2__(t) __FILE__,__LINE__,__func__   ,t

#    define LOG(S,t)   printf("%s[%u] %s\t%s=%" #S "\n", __FILE__,__LINE__,__func__,#t,t)

#    define LOGI( t)   LOG(i,t)
#    define LOGU( t)   LOG(u,t)
#    define LOGD( t)   LOG(d,t)

#    define LOGS(t)    printf("%s[%i] %s\t%s=%s\n",      __FILE__,__LINE__,__func__    ,t)
#    define LOGSS(s,t) printf("%s[%i] %s\t%s=%s " #s " \n",      __FILE__,__LINE__,__func__    ,t)


#  endif  // __cplusplus

# else

# ifdef __cplusplus
#   define LOG (t)
# else
#   define LOG (s,t)
#endif // __cplusplus

#   define LOG1(t)
#   define LOGS(t)
#   define LOGI(t)
#   define LOGU(t)
#   define LOGD(t)

# endif // NOT_LOG

#endif  // LOG_H

