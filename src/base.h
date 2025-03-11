#pragma once

#ifdef _WIN32
    #ifdef BUILD_SHARED_LOG_CC
        #define LOG_CC_API __declspec(dllexport)
    #else
        #define LOG_CC_API __declspec(dllimport)
    #endif
#else
    #define LOG_CC_API extern
#endif



#ifndef __top_level_namespace
    #define __top_level_namespace logcc
#endif