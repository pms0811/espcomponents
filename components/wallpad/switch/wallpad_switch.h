#pragma once

#include "esphome/components/wallpad/wallpad_device.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace wallpad {

class WallPadSwitch : public switch_::Switch, public WallPadDevice 
{
  public:
        WallPadSwitch() { device_name_ = &this->name_; }
        void dump_config() override;
        void publish(const uint8_t *data, const num_t len) override;
        bool publish(bool state) override { publish_state(state); return true; }

        void write_state(bool state) override 
        {
            if(state == this->state) return;
            send_command(state ? this->get_command_on() : this->get_command_off());
            publish_state(state);
        }

  protected:

};

}  // namespace wallpad
}  // namespace esphome