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

#include "Stream.hpp"

namespace toolbox {
inline namespace http {
using namespace std;

void StreamBuf::set_content_length(std::streamsize pos, std::streamsize len) noexcept
{
    auto* it = pbase_ + pos;
    do {
        --it;
        *it = '0' + len % 10;
        len /= 10;
    } while (len > 0);
}

StreamBuf::~StreamBuf() = default;

StreamBuf::int_type StreamBuf::overflow(int_type c) noexcept
{
    if (c != traits_type::eof()) {
        auto buf = buf_.prepare(pcount_ + 1);
        pbase_ = static_cast<char*>(buf.data());
        pbase_[pcount_++] = c;
    }
    return c;
}

streamsize StreamBuf::xsputn(const char_type* s, streamsize count) noexcept
{
    auto buf = buf_.prepare(pcount_ + count);
    pbase_ = static_cast<char*>(buf.data());
    memcpy(pbase_ + pcount_, s, count);
    pcount_ += count;
    return count;
}

OStream::~OStream() = default;

void OStream::commit() noexcept
{
    if (cloff_ > 0) {
        buf_.set_content_length(cloff_, buf_.pcount() - hcount_);
    }
    buf_.commit();
}

void OStream::reset(Status status, const char* content_type, NoCache no_cache)
{
    buf_.reset();
    *this << reset_state;

    *this << "HTTP/1.1 " << status << ' ' << enum_string(status);
    if (no_cache == NoCache::Yes) {
        *this << "\r\nCache-Control: no-cache";
    }
    if (content_type) {
        // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF. Use 10 space
        // place-holder for content length. RFC2616 states that field value MAY be preceded by any
        // amount of LWS, though a single SP is preferred.
        *this << "\r\nContent-Type: " << content_type //
              << "\r\nContent-Length:          0";
        cloff_ = buf_.pcount();
    } else {
        cloff_ = 0;
    }
    *this << "\r\n\r\n";
    hcount_ = buf_.pcount();
}

} // namespace http
} // namespace toolbox
