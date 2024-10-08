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

// Parse the tls-version-min option.

#pragma once

#include <string>

#include <openvpn/common/size.hpp>
#include <openvpn/common/exception.hpp>
#include <openvpn/common/options.hpp>

namespace openvpn::TLSVersion {

enum class Type
{
    UNDEF,
    V1_0,
    V1_1,
    V1_2,
    V1_3
};

inline bool operator<(const Type &A, const Type &B)
{
    return static_cast<int>(A) < static_cast<int>(B);
}

inline const std::string to_string(const Type version)
{
    switch (version)
    {
    case Type::UNDEF:
        return "UNDEF";
    case Type::V1_0:
        return "V1_0";
    case Type::V1_1:
        return "V1_1";
    case Type::V1_2:
        return "V1_2";
    case Type::V1_3:
        return "V1_3";
    default:
        return "???";
    }
}

inline Type parse_tls_version_min(const std::string &ver,
                                  const bool or_highest,
                                  const Type max_version)
{
    if (ver == "1.0" && Type::V1_0 <= max_version)
        return Type::V1_0;
    else if (ver == "1.1" && Type::V1_1 <= max_version)
        return Type::V1_1;
    else if (ver == "1.2" && Type::V1_2 <= max_version)
        return Type::V1_2;
    else if (ver == "1.3" && Type::V1_3 <= max_version)
        return Type::V1_3;
    else if (or_highest)
        return max_version;
    else
        throw option_error(ERR_INVALID_OPTION_CRYPTO, "tls-version-min: unrecognized TLS version");
}

inline Type parse_tls_version_min(const OptionList &opt,
                                  const std::string &relay_prefix,
                                  const Type max_version)
{
    const Option *o = opt.get_ptr(relay_prefix + "tls-version-min");
    if (o)
    {
        const std::string ver = o->get_optional(1, 16);
        const bool or_highest = (o->get_optional(2, 16) == "or-highest");
        return parse_tls_version_min(ver, or_highest, max_version);
    }
    return Type::UNDEF;
}

inline void apply_override(Type &tvm, const std::string &override)
{
    const Type orig = tvm;
    Type newtvm = Type::UNDEF;

    if (override.empty() || override == "default")
        newtvm = tvm;
    else if (override == "disabled")
        tvm = Type::UNDEF;
    else if (override == "tls_1_0")
        newtvm = Type::V1_0;
    else if (override == "tls_1_1")
        newtvm = Type::V1_1;
    else if (override == "tls_1_2")
        newtvm = Type::V1_2;
    else if (override == "tls_1_3")
        newtvm = Type::V1_3;
    else
        throw option_error(ERR_INVALID_OPTION_CRYPTO, "tls-version-min: unrecognized override string");

    if (newtvm > orig || newtvm == Type::UNDEF)
        tvm = newtvm;

    // OPENVPN_LOG("*** TLS-version-min before=" << to_string(orig) << " override=" << override << " after=" << to_string(tvm)); // fixme
}
} // namespace openvpn::TLSVersion
