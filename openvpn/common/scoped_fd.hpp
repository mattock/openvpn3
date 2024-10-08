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

// A scoped file descriptor that is automatically closed by its destructor.

#ifndef OPENVPN_COMMON_SCOPED_FD_H
#define OPENVPN_COMMON_SCOPED_FD_H

#include <unistd.h> // for close()
#include <errno.h>

namespace openvpn {

class ScopedFD
{
    ScopedFD(const ScopedFD &) = delete;
    ScopedFD &operator=(const ScopedFD &) = delete;

  public:
    typedef int base_type;

    ScopedFD()
        : fd(undefined())
    {
    }

    explicit ScopedFD(const int fd_arg)
        : fd(fd_arg)
    {
    }

    static int undefined()
    {
        return -1;
    }

    int release()
    {
        const int ret = fd;
        fd = -1;
        // OPENVPN_LOG("**** SFD RELEASE=" << ret);
        return ret;
    }

    static bool defined_static(int fd)
    {
        return fd >= 0;
    }

    bool defined() const
    {
        return defined_static(fd);
    }

    int operator()() const
    {
        return fd;
    }

    void reset(const int fd_arg)
    {
        close();
        fd = fd_arg;
        // OPENVPN_LOG("**** SFD RESET=" << fd);
    }

    void reset()
    {
        close();
    }

    // unusual semantics: replace fd without closing it first
    void replace(const int fd_arg)
    {
        // OPENVPN_LOG("**** SFD REPLACE " << fd << " -> " << fd_arg);
        fd = fd_arg;
    }

    // return false if close error
    bool close()
    {
        return close_with_errno() == 0;
    }

    // return errno value if close error, otherwise return 0
    int close_with_errno()
    {
        int eno = 0;
        if (defined())
        {
            if (::close(fd) == -1)
                eno = errno;
            // OPENVPN_LOG("**** SFD CLOSE fd=" << fd << " errno=" << eno);
            fd = -1;
        }
        return eno;
    }

    virtual ~ScopedFD()
    {
        // OPENVPN_LOG("**** SFD DESTRUCTOR");
        close();
    }

    ScopedFD(ScopedFD &&other) noexcept
    {
        fd = other.fd;
        other.fd = -1;
    }

    ScopedFD &operator=(ScopedFD &&other) noexcept
    {
        close();
        fd = other.fd;
        other.fd = -1;
        return *this;
    }

  private:
    int fd;
};

} // namespace openvpn

#endif // OPENVPN_COMMON_SCOPED_FD_H
