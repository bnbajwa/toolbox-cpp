// The Reactive C++ Toolbox.
// Copyright (C) 2013-2019 Swirly Cloud Limited
// Copyright (C) 2021 Reactive Markets Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TOOLBOX_UTIL_VERSION_HPP
#define TOOLBOX_UTIL_VERSION_HPP

#include <toolbox/util/String.hpp>

#include <boost/functional/hash.hpp>

namespace toolbox {
inline namespace util {

struct Version {
    constexpr explicit Version(int major = 0, int minor = 0) noexcept
    : major{major}
    , minor{minor}
    {
    }
    ~Version() = default;

    // Copy.
    constexpr Version(const Version&) noexcept = default;
    constexpr Version& operator=(const Version&) noexcept = default;

    // Move.
    constexpr Version(Version&&) noexcept = default;
    constexpr Version& operator=(Version&&) noexcept = default;

    constexpr bool empty() const noexcept { return major == 0 && minor == 0; }
    /// Returns the length of the equivalent string representation.
    constexpr std::size_t size() const noexcept
    {
        return dec_digits(major) + dec_digits(minor) + 1;
    }
    constexpr explicit operator bool() const noexcept { return !empty(); }

    void clear() noexcept { major = minor = 0; }
    void swap(Version& rhs) noexcept
    {
        std::swap(major, rhs.major);
        std::swap(minor, rhs.minor);
    }

    constexpr auto operator<=>(const Version&) const noexcept = default;
    constexpr bool operator==(const Version&) const noexcept = default;

    int major{0}, minor{0};
};

static_assert(Version{1, 2} == Version{1, 2});
static_assert(Version{1, 2} < Version{1, 3});
static_assert(Version{1, 2} < Version{2, 2});

template <typename StreamT>
    requires Streamable<StreamT>
StreamT& operator<<(StreamT& os, Version ver)
{
    os << ver.major << '.' << ver.minor;
    return os;
}

template <>
struct TypeTraits<Version> {
    static auto from_string(std::string_view sv) noexcept
    {
        const auto [major, minor] = split_pair(sv, '.');
        return Version{TypeTraits<int>::from_string(major), TypeTraits<int>::from_string(minor)};
    }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s});
    }
};

inline std::size_t hash_value(toolbox::Version ver)
{
    std::size_t h{0};
    boost::hash_combine(h, ver.major);
    boost::hash_combine(h, ver.minor);
    return h;
}

} // namespace util
} // namespace toolbox

namespace std {
template <>
struct hash<toolbox::Version> {
    inline std::size_t operator()(toolbox::Version ver) const { return hash_value(ver); }
};
} // namespace std

#endif // TOOLBOX_UTIL_VERSION_HPP
