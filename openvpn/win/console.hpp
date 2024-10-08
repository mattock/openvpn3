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

// console utilities for Windows

#ifndef OPENVPN_WIN_CONSOLE_H
#define OPENVPN_WIN_CONSOLE_H

#include <windows.h>
#include <string>
#include <openvpn/win/handle.hpp>

namespace openvpn::Win::Console {

class Input
{
    Input(const Input &) = delete;
    Input &operator=(const Input &) = delete;

  public:
    Input(bool password)
        : std_input(Handle::undefined()),
          console_mode_save(0)
    {
        // disable control-C
        ::SetConsoleCtrlHandler(nullptr, TRUE);

        HANDLE in = ::GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        if (Handle::defined(in) && ::GetConsoleMode(in, &mode))
        {
            // running on a console
            DWORD newmode = mode
                            & ~(ENABLE_WINDOW_INPUT
                                | ENABLE_ECHO_INPUT
                                | ENABLE_MOUSE_INPUT);

            if (!password)
            {
                newmode &= ~(ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT);
            }

            if (newmode == mode || ::SetConsoleMode(in, newmode))
            {
                std_input = in;
                console_mode_save = mode;
            }
        }
    }

    ~Input()
    {
        if (Handle::defined(std_input))
            ::SetConsoleMode(std_input, console_mode_save);
    }

    bool available()
    {
        if (Handle::defined(std_input))
        {
            DWORD n;
            if (::GetNumberOfConsoleInputEvents(std_input, &n))
                return n > 0;
        }
        return false;
    }

    std::string get_password(const std::string &prompt)
    {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        std::cout << std::endl;

        return s;
    }

    unsigned int get()
    {
        if (Handle::defined(std_input))
        {
            INPUT_RECORD ir;
            do
            {
                DWORD n;
                if (!available())
                    return 0;
                if (!::ReadConsoleInputA(std_input, &ir, 1, &n))
                    return 0;
            } while (ir.EventType != KEY_EVENT || ir.Event.KeyEvent.bKeyDown != TRUE);
            return keyboard_ir_to_key(&ir);
        }
        else
            return 0;
    }

  private:
    unsigned int keyboard_ir_to_key(INPUT_RECORD *ir)
    {
        if (ir->Event.KeyEvent.uChar.AsciiChar == 0)
            return ir->Event.KeyEvent.wVirtualScanCode;

        if ((ir->Event.KeyEvent.dwControlKeyState
             & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            && (ir->Event.KeyEvent.wVirtualKeyCode != 18))
            return ir->Event.KeyEvent.wVirtualScanCode * 256;

        return ir->Event.KeyEvent.uChar.AsciiChar;
    }

    HANDLE std_input;
    DWORD console_mode_save;
};

class Title
{
    Title(const Title &) = delete;
    Title &operator=(const Title &) = delete;

  public:
    Title(const std::string &new_title)
        : old_title_defined(false)
    {
        char title[256];
        if (::GetConsoleTitleA(title, sizeof(title)))
        {
            old_title = title;
            old_title_defined = true;
        }
        ::SetConsoleTitleA(new_title.c_str());
    }

    ~Title()
    {
        if (old_title_defined)
            ::SetConsoleTitleA(old_title.c_str());
    }

  private:
    bool old_title_defined;
    std::string old_title;
};
} // namespace openvpn::Win::Console

#endif
