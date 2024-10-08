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

// Wrap a mbed TLS x509_crl object

#ifndef OPENVPN_MBEDTLS_PKI_X509CRL_H
#define OPENVPN_MBEDTLS_PKI_X509CRL_H

#include <string>
#include <sstream>
#include <cstring>

#include <mbedtls/x509_crl.h>

#include <openvpn/common/size.hpp>
#include <openvpn/common/exception.hpp>
#include <openvpn/common/rc.hpp>
#include <openvpn/mbedtls/util/error.hpp>

namespace openvpn::MbedTLSPKI {

class X509CRL : public RC<thread_unsafe_refcount>
{
  public:
    typedef RCPtr<X509CRL> Ptr;

    X509CRL()
        : chain(nullptr)
    {
    }

    X509CRL(const std::string &crl_txt)
        : chain(nullptr)
    {
        try
        {
            parse(crl_txt);
        }
        catch (...)
        {
            dealloc();
            throw;
        }
    }

    void parse(const std::string &crl_txt)
    {
        alloc();

        // crl_txt.length() is increased by 1 as it does not include the NULL-terminator
        // which mbedtls_x509_crl_parse() expects to see.
        const int status = mbedtls_x509_crl_parse(chain,
                                                  (const unsigned char *)crl_txt.c_str(),
                                                  crl_txt.length() + 1);
        if (status < 0)
        {
            throw MbedTLSException("error parsing CRL", status);
        }

        pem_chain = crl_txt;
    }

    std::string extract() const
    {
        return std::string(pem_chain);
    }

    mbedtls_x509_crl *get() const
    {
        return chain;
    }

    ~X509CRL()
    {
        dealloc();
    }

  private:
    void alloc()
    {
        if (!chain)
        {
            chain = new mbedtls_x509_crl;
            std::memset(chain, 0, sizeof(mbedtls_x509_crl));
        }
    }

    void dealloc()
    {
        if (chain)
        {
            mbedtls_x509_crl_free(chain);
            delete chain;
            chain = nullptr;
        }
    }

    mbedtls_x509_crl *chain;
    std::string pem_chain;
};
} // namespace openvpn::MbedTLSPKI

#endif
