#pragma once

#include "LegacyException.hpp"

namespace cg::platform::native_cpp {

// NCE01：接收已完成构造的POD payload，不再复制成员/增加共享引用。
struct AdoptPayload final {};

// 对应原全局exception层级；SDK持有本机对象，业务字段仅在payload内。
class Exception {
public:
    Exception(const Exception& source); // 基类按值捕获只复制原exception部分。
    Exception& operator=(const Exception&) = delete; // 禁止默认浅赋值拥有块。
    virtual ~Exception() noexcept(false); // 仅一次原非删除析构，不释放本机对象storage。
    virtual const char* what() const noexcept; // 原基础/五派生文字与空回退。

protected:
    enum class Kind : std::uint8_t { Base, Logic, Length, Range, Runtime, StreamFailure };
    Exception(Kind kind, const LegacyLogicErrorObject& source); // 依本族copy callback构造。
    Exception(AdoptPayload, Kind kind, const LegacyLogicErrorObject& prepared) noexcept;
    const LegacyLogicErrorObject& payload() const noexcept { return payload_; }

private:
    Kind kind_; // 本机复制/清理职责；不是原业务标量或SDK类型编号。
    LegacyLogicErrorObject payload_; // 唯一owner；未解释prefix/padding不预清。
};

class LogicError : public Exception {
public:
    explicit LogicError(const LegacyLogicErrorObject& source);
    LogicError(const LogicError& source); // 派生切片依原logic copy/析构。
protected:
    LogicError(Kind kind, const LegacyLogicErrorObject& source);
    LogicError(AdoptPayload, Kind kind, const LegacyLogicErrorObject& prepared) noexcept;
};

class LengthError final : public LogicError {
public:
    explicit LengthError(const LegacyStringLengthErrorObject& source);
    LengthError(const LengthError& source);
    LengthError(AdoptPayload, const LegacyStringLengthErrorObject& prepared) noexcept;
};

class RangeError final : public LogicError {
public:
    explicit RangeError(const LegacyStringRangeErrorObject& source);
    RangeError(const RangeError& source);
    RangeError(AdoptPayload, const LegacyStringRangeErrorObject& prepared) noexcept;
};

class RuntimeError : public Exception {
public:
    explicit RuntimeError(const LegacyRuntimeErrorObject& source);
    RuntimeError(const RuntimeError& source); // 派生切片依原runtime copy/析构。
protected:
    RuntimeError(Kind kind, const LegacyRuntimeErrorObject& source);
    RuntimeError(AdoptPayload, Kind kind, const LegacyRuntimeErrorObject& prepared) noexcept;
};

class StreamFailure final : public RuntimeError {
public:
    explicit StreamFailure(const LegacyStreamFailureObject& source);
    StreamFailure(const StreamFailure& source);
    StreamFailure(AdoptPayload, const LegacyStreamFailureObject& prepared) noexcept;
};

} // namespace cg::platform::native_cpp
