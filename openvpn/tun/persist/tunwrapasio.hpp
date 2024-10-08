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

#ifndef OPENVPN_TUN_PERSIST_TUNWRAPASIO_H
#define OPENVPN_TUN_PERSIST_TUNWRAPASIO_H

#include <utility>

namespace openvpn {

// This object supports that subset of the Asio stream
// interface required by TunIO, and is intended to wrap
// a ScopedAsioStream embedded in a TunWrap object.
// It is used primarily on Windows to wrap the TAP
// interface HANDLE in way that plays well with Windows
// I/O completion ports (once a HANDLE is bound to an
// I/O completion port it cannot be unbound).
template <typename TunWrap>
class TunWrapAsioStream
{
  public:
    TunWrapAsioStream(const typename TunWrap::Ptr &tun_wrap_arg)
        : tun_wrap(tun_wrap_arg)
    {
    }


    void release()
    {
        tun_wrap.reset();
    }

    // Delegate STREAM methods (only need to support the
    // subset of methods used by TunIO).
    // Prototypes from asio/windows/basic_stream_handle.hpp

    template <typename MUTABLE_BUFFER, typename HANDLER>
    void async_read_some(const MUTABLE_BUFFER &buffers, HANDLER &&handler)
    {
        return tun_wrap->obj()->async_read_some(buffers, std::move(handler));
    }

    template <typename CONST_BUFFER>
    std::size_t write_some(const CONST_BUFFER &buffers)
    {
        return tun_wrap->obj()->write_some(buffers);
    }

    void cancel()
    {
        tun_wrap->obj()->cancel();
    }

    void close()
    {
        tun_wrap->obj()->close();
    }

  private:
    typename TunWrap::Ptr tun_wrap;
};

} // namespace openvpn
#endif
