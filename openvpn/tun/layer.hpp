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

// Class that encapsulates the definition of an OSI layer.

#ifndef OPENVPN_TUN_LAYER_H
#define OPENVPN_TUN_LAYER_H

#include <openvpn/common/exception.hpp>

namespace openvpn {
class Layer
{
  public:
    enum Type
    {
        NONE,
        OSI_LAYER_2,
        OSI_LAYER_3,
    };

    Layer()
        : type_(NONE)
    {
    }
    explicit Layer(const Type t)
        : type_(t)
    {
    }
    Type operator()() const
    {
        return type_;
    }

    bool defined() const
    {
        return type_ != NONE;
    }

    const char *dev_type() const
    {
        switch (type_)
        {
        case OSI_LAYER_2:
            return "tap";
        case OSI_LAYER_3:
            return "tun";
        default:
            return "null";
        }
    }

    const char *str() const
    {
        switch (type_)
        {
        case NONE:
            return "UNDEF_LAYER";
        case OSI_LAYER_2:
            return "OSI_LAYER_2";
        case OSI_LAYER_3:
            return "OSI_LAYER_3";
        default:
            throw Exception("Layer: unrecognized layer type");
        }
    }

    int value() const
    {
        switch (type_)
        {
        case NONE:
            return 0;
        case OSI_LAYER_2:
            return 2;
        case OSI_LAYER_3:
            return 3;
        default:
            throw Exception("Layer: unrecognized layer type");
        }
    }

    static Layer from_str(const std::string &str)
    {
        if (str == "OSI_LAYER_3")
            return Layer(OSI_LAYER_3);
        else if (str == "OSI_LAYER_2")
            return Layer(OSI_LAYER_2);
        else if (str == "UNDEF_LAYER")
            return Layer(NONE);
        else
            throw Exception("Layer: unrecognized layer string");
    }

    static Layer from_value(const int value)
    {
        if (value == 3)
            return Layer(OSI_LAYER_3);
        else if (value == 2)
            return Layer(OSI_LAYER_2);
        else if (value == 0)
            return Layer(NONE);
        else
            throw Exception("Layer: unrecognized layer value");
    }

    bool operator==(const Layer &other) const
    {
        return type_ == other.type_;
    }

    bool operator!=(const Layer &other) const
    {
        return type_ != other.type_;
    }

  private:
    Type type_;
};
} // namespace openvpn

#endif // OPENVPN_TUN_LAYER_H
