//    OpenVPN -- An application to securely tunnel IP networks
//               over a single port, with support for SSL/TLS-based
//               session authentication and key exchange,
//               packet encryption, packet authentication, and
//               packet compression.
//
//    Copyright (C) 2012- OpenVPN Inc.
//
//    SPDX-License-Identifier: MPL-2.0 OR AGPL-3.0-only WITH openvpn3-openssl-exception
//

#ifndef OPENVPN_WIN_UNICODE_H
#define OPENVPN_WIN_UNICODE_H

#include <windows.h>

#include <string>
#include <memory>

#include <openvpn/common/size.hpp>
#include <openvpn/common/exception.hpp>

namespace openvpn::Win {
typedef std::unique_ptr<wchar_t[]> UTF16;
typedef std::unique_ptr<char[]> UTF8;

OPENVPN_SIMPLE_EXCEPTION(win_utf16);

inline wchar_t *utf16(const std::string &str, int cp = CP_UTF8)
{
    // first get output length (return value includes space for trailing nul)
    const int len = ::MultiByteToWideChar(cp,
                                          0,
                                          str.c_str(),
                                          -1,
                                          nullptr,
                                          0);
    if (len <= 0)
        throw win_utf16();
    UTF16 ret(new wchar_t[len]);
    const int len2 = ::MultiByteToWideChar(cp,
                                           0,
                                           str.c_str(),
                                           -1,
                                           ret.get(),
                                           len);
    if (len != len2)
        throw win_utf16();
    return ret.release();
}

inline size_t utf16_strlen(const wchar_t *str)
{
    return ::wcslen(str);
}

inline char *utf8(wchar_t *str)
{
    // first get output length (return value includes space for trailing nul)
    const int len = ::WideCharToMultiByte(CP_UTF8,
                                          0,
                                          str,
                                          -1,
                                          NULL,
                                          0,
                                          NULL,
                                          NULL);
    if (len <= 0)
        throw win_utf16();
    UTF8 ret(new char[len]);
    const int len2 = ::WideCharToMultiByte(CP_UTF8,
                                           0,
                                           str,
                                           -1,
                                           ret.get(),
                                           len,
                                           NULL,
                                           NULL);
    if (len != len2)
        throw win_utf16();
    return ret.release();
}
} // namespace openvpn::Win
#endif
