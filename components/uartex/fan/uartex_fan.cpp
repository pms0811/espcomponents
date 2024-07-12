#include "uartex_fan.h"
#include "esphome/core/log.h"

namespace esphome {
namespace uartex {

static const char *TAG = "uartex.fan";

void UARTExFan::dump_config()
{
    ESP_LOGCONFIG(TAG, "UARTEx Fan '%s':", get_name().c_str());
    dump_uartex_device_config(TAG);
}

fan::FanTraits UARTExFan::get_traits()
{
    fan::FanTraits traits{};
    if (this->speed_count_ > 0)
    {
        traits.set_speed(true);
        traits.set_supported_speed_count(this->speed_count_);
    }
    return traits;
}

void UARTExFan::publish(const std::vector<uint8_t>& data)
{
    if (has_state_func("state_speed"))
    {
        optional<float> val = get_state_func("state_speed", &data[0], data.size());
        if (val.has_value() && this->speed != (int)val.value())
        {
            this->speed = (int)val.value();
            publish_state();
        }
    }
}

void UARTExFan::publish(const bool state)
{
    if (state == this->state) return;
    this->state = state; 
    this->publish_state();
}

void UARTExFan::control(const fan::FanCall &call)
{
    bool changed_state = false;
    bool changed_speed = false;
    bool changed_oscillating = false;
    bool changed_direction = false;
    if (call.get_state().has_value() && this->state != *call.get_state())
    {
        this->state = *call.get_state();
        changed_state = true;
    }
    if (call.get_oscillating().has_value() && this->oscillating != *call.get_oscillating())
    {
        this->oscillating = *call.get_oscillating();
        changed_oscillating = true;
    }
    if (call.get_speed().has_value() && this->speed != *call.get_speed())
    {
        this->speed = *call.get_speed();
        changed_speed = true;
    }
    if (call.get_direction().has_value() && this->direction != *call.get_direction())
    {
        this->direction = *call.get_direction();
        changed_direction = true;
    }
    if (get_command_on() && this->state && changed_state) enqueue_tx_cmd(get_command_on());
    if (get_command_speed(this->speed) && changed_speed) enqueue_tx_cmd(get_command_speed(this->speed));
    if (get_command_off() && !this->state && changed_state) enqueue_tx_cmd(get_command_off());
    publish_state();
}

cmd_t *UARTExFan::get_command_speed(const float x)
{
    return get_command("command_speed", x);
}

}  // namespace uartex
}  // namespace esphome