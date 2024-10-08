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

#ifndef OPENVPN_COMMON_STOP_H
#define OPENVPN_COMMON_STOP_H

#include <vector>
#include <functional>
#include <utility>
#include <mutex>

#include <openvpn/common/exception.hpp>

OPENVPN_EXCEPTION_INHERIT(std::range_error, openvpn_stop_limit);

namespace openvpn {

class Stop
{
  public:
    class Scope
    {
        friend Stop;

      public:
        Scope(Stop *stop_arg, std::function<void()> &&method_arg)
            : stop(stop_arg),
              method(std::move(method_arg)),
              index(-1)
        {
            constexpr int stop_index_limit = 1000;
            if (stop)
            {
                std::lock_guard<std::recursive_mutex> lock(stop->mutex);
                if (stop->stop_called)
                {
                    // stop already called, call method immediately
                    method();
                }
                else
                {
                    if (index > stop_index_limit)
                        throw openvpn_stop_limit("Stop count limit exceeded");

                    index = static_cast<decltype(index)>(stop->scopes.size());
                    stop->scopes.push_back(this);
                }
            }
        }

        ~Scope()
        {
            if (stop)
            {
                std::lock_guard<std::recursive_mutex> lock(stop->mutex);
                if (index >= 0 && index < static_cast<int>(stop->scopes.size()) && stop->scopes[index] == this)
                {
                    stop->scopes[index] = nullptr;
                    stop->prune();
                }
            }
        }

      private:
        Scope(const Scope &) = delete;
        Scope &operator=(const Scope &) = delete;

        Stop *const stop;
        const std::function<void()> method;
        int index;
    };

    Stop()
    {
    }

    void stop()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        stop_called = true;
        while (scopes.size())
        {
            Scope *scope = scopes.back();
            scopes.pop_back();
            if (scope)
            {
                scope->index = -1;
                scope->method();
            }
        }
    }

    static bool is_triggered(const Stop *stop)
    {
        return stop && stop->stop_called;
    }

    Stop(const Stop &) = delete;
    Stop &operator=(const Stop &) = delete;

  private:
    void prune()
    {
        while (scopes.size() && !scopes.back())
            scopes.pop_back();
    }

    std::recursive_mutex mutex;
    std::vector<Scope *> scopes;
    volatile bool stop_called = false;
};

} // namespace openvpn

#endif
