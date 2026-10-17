#pragma once

#include <cstdio>
#include <cwchar>

// RHM01：沿参考编译开关输出到当前 CRT stdout；禁用时仅检查类型，不求值实参。
#ifndef m_log
#if defined(_DEBUG) || defined(MY_DEBUG) || (defined(MATCHER) ? defined(MATCHER_DEBUG) : defined(ENABLE_LOG))
#define m_log(...) std::printf(__VA_ARGS__)
#define m_logw(...) std::wprintf(__VA_ARGS__)
#else
#define m_log(...) static_cast<void>(sizeof(std::printf(__VA_ARGS__)))
#define m_logw(...) static_cast<void>(sizeof(std::wprintf(__VA_ARGS__)))
#endif
#endif

// 普通业务日志；ENABLE_LOG 独立于 MATCHER_DEBUG。
#ifndef m_log_i
#if defined(_DEBUG) || defined(MY_DEBUG) || defined(ENABLE_LOG)
#define m_log_i(...) std::printf(__VA_ARGS__)
#define m_log_iw(...) std::wprintf(__VA_ARGS__)
#else
#define m_log_i(...) static_cast<void>(sizeof(std::printf(__VA_ARGS__)))
#define m_log_iw(...) static_cast<void>(sizeof(std::wprintf(__VA_ARGS__)))
#endif
#endif

// 匹配诊断日志；迁入宏接口，不引入参考项目的扫描业务。
#ifndef m_log_m
#if defined(_DEBUG) || defined(MY_DEBUG) || defined(MATCHER_DEBUG)
#define m_log_m(...) std::printf(__VA_ARGS__)
#define m_log_mw(...) std::wprintf(__VA_ARGS__)
#else
#define m_log_m(...) static_cast<void>(sizeof(std::printf(__VA_ARGS__)))
#define m_log_mw(...) static_cast<void>(sizeof(std::wprintf(__VA_ARGS__)))
#endif
#endif
