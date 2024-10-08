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

#pragma once

#include <vector>

#include <openvpn/common/rc.hpp>

namespace openvpn {

class LogSetup : public RC<thread_safe_refcount>
{
  public:
    typedef RCPtr<LogSetup> Ptr;

    virtual void reopen() = 0;
};

class MultiLogSetup : public std::vector<LogSetup::Ptr>,
                      public LogSetup
{
  public:
    typedef RCPtr<MultiLogSetup> Ptr;

    virtual void reopen() override
    {
        for (const auto &e : *this)
            e->reopen();
    }
};

} // namespace openvpn
