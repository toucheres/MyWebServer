#pragma once
#include "corutine.hpp"
#include "signal_slots.hpp"
#include "file.h"
class stdiolistener : public co_async, public enable_auto_remove_slots
{
  public:
    signal<std::string_view> on_user_input;
    virtual EventStatus eventGo()
    {
        auto& stdaio = async_in_out::getInstance();
        auto in = stdaio.read_line();

        if (in == std::string{""})
        {
            return EventStatus::Continue;
        }
        else if (in == std::string{"quit\n"})
        {
            on_exit.emit();
            return EventStatus::Stop;
        }
        else
        {
            on_user_input.emit(in);
            return EventStatus::Continue;
        }
    }
};