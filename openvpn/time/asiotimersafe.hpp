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

#include <openvpn/common/rc.hpp>
#include <openvpn/time/asiotimer.hpp>

// AsioTimerSafe is like AsioTimer but with strict cancellation
// semantics that guarantees that a handler will never be called
// with a non-error status after the timer is cancelled.

namespace openvpn {
class AsioTimerSafe
{
  public:
    typedef std::unique_ptr<AsioTimerSafe> UPtr;

    AsioTimerSafe(openvpn_io::io_context &io_context)
        : timer_(io_context),
          epoch_(new Epoch)
    {
    }

    std::size_t expires_at(const Time &t)
    {
        inc_epoch();
        return timer_.expires_at(t);
    }

    std::size_t expires_after(const Time::Duration &d)
    {
        inc_epoch();
        return timer_.expires_after(d);
    }

    std::size_t cancel()
    {
        inc_epoch();
        return timer_.cancel();
    }

    template <typename F>
    void async_wait(F &&func)
    {
        inc_epoch();
        timer_.async_wait([func = std::move(func), epoch = epoch(), eptr = epoch_](const openvpn_io::error_code &error)
                          { func(epoch == eptr->epoch ? error : openvpn_io::error::operation_aborted); });
    }

  private:
    typedef std::size_t epoch_t;

    struct Epoch : public RC<thread_unsafe_refcount>
    {
        typedef RCPtr<Epoch> Ptr;
        epoch_t epoch = 0;
    };

    epoch_t epoch() const
    {
        return epoch_->epoch;
    }

    void inc_epoch()
    {
        ++epoch_->epoch;
    }

    AsioTimer timer_;
    Epoch::Ptr epoch_;
};
} // namespace openvpn
