#pragma once

#ifdef _WIN32
#ifdef BUILD_SHARED_M_LOG
#define LOG_API __declspec(dllexport)
#else
#define LOG_API __declspec(dllimport)
#endif
#else
#define LOG_API extern
#endif
