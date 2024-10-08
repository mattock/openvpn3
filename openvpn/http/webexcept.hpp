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

#ifndef OPENVPN_HTTP_EXCEPT_H
#define OPENVPN_HTTP_EXCEPT_H

#include <string>
#include <sstream>
#include <exception>

#include <openvpn/http/status.hpp>

#define OPENVPN_THROW_WEB(exc, status, stuff) \
    do                                        \
    {                                         \
        std::ostringstream _ovpn_exc;         \
        _ovpn_exc << stuff;                   \
        throw exc(status, _ovpn_exc.str());   \
    } while (0)

namespace openvpn::HTTP {
class WebException : public std::exception
{
  public:
    WebException(const int status, const std::string &error)
        : status_(status),
          error_(error),
          formatted(std::string(Status::to_string(status_)) + " : " + error_)
    {
    }

    WebException(const int status)
        : status_(status),
          error_(Status::to_string(status_)),
          formatted(error_)
    {
    }

    int status() const
    {
        return status_;
    }
    const std::string &error() const
    {
        return error_;
    }

    const char *what() const noexcept override
    {
        return formatted.c_str();
    }

  private:
    const int status_;
    const std::string error_;
    const std::string formatted;
};
} // namespace openvpn::HTTP

#endif
