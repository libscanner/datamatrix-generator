// dmgen/Result.h — operation result: a value or an error.
//
// C++20 has no std::expected, so this is a minimal equivalent. The public
// dmgen API does not throw: everything that can fail returns Result<T>.
#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

namespace dmgen {

enum class ErrorCode {
    InvalidArgument = 1,  ///< invalid option: size not in the table, empty data…
    DataTooLong,          ///< data does not fit even into 144×144
    SizeTooSmall,         ///< data does not fit into the explicitly requested size
    UnencodableChar,      ///< character cannot be encoded with the selected scheme
    Gs1Invalid,           ///< string failed GS1 validation (details: gs1::validate)
    IoError,              ///< failed to write the file or image
    InternalVerifyFailed, ///< self-check could not read the assembled symbol — a dmgen bug
    Unlicensed,           ///< trial period over and no license (dmgen/License.h)
};

/// Error code and a human-readable description (UTF-8, in Russian).
struct Error {
    ErrorCode   code;
    std::string message;
};

const char* toString(ErrorCode code) noexcept;

template <typename T>
class [[nodiscard]] Result {
public:
    Result(T value) : v_(std::move(value)) {}
    Result(Error error) : v_(std::move(error)) {}

    bool ok() const noexcept { return v_.index() == 0; }
    explicit operator bool() const noexcept { return ok(); }

    /// The value; on error throws std::runtime_error with the error message
    /// (for those who prefer exceptions to checking ok()).
    T&       value() &       { check(); return std::get<0>(v_); }
    const T& value() const & { check(); return std::get<0>(v_); }
    T&&      value() &&      { check(); return std::get<0>(std::move(v_)); }
    T&       operator*() &       { return value(); }
    const T& operator*() const & { return value(); }
    T*       operator->()       { return &value(); }
    const T* operator->() const { return &value(); }

    const Error& error() const { return std::get<1>(v_); }

private:
    void check() const {
        if (!ok()) throw std::runtime_error(std::get<1>(v_).message);
    }
    std::variant<T, Error> v_;
};

template <>
class [[nodiscard]] Result<void> {
public:
    Result() = default;
    Result(Error error) : err_(std::move(error)), failed_(true) {}

    bool ok() const noexcept { return !failed_; }
    explicit operator bool() const noexcept { return ok(); }
    const Error& error() const { return err_; }

private:
    Error err_{};
    bool  failed_ = false;
};

} // namespace dmgen
