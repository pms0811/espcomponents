#ifndef __AXP192_H__
#define __AXP192_H__

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/button/button.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace axp192 {

#define SLEEP_MSEC(us) (((uint64_t)us) * 1000L)
#define SLEEP_SEC(us)  (((uint64_t)us) * 1000000L)
#define SLEEP_MIN(us)  (((uint64_t)us) * 60L * 1000000L)
#define SLEEP_HR(us)   (((uint64_t)us) * 60L * 60L * 1000000L)

#define CURRENT_100MA  (0b0000)
#define CURRENT_190MA  (0b0001)
#define CURRENT_280MA  (0b0010)
#define CURRENT_360MA  (0b0011)
#define CURRENT_450MA  (0b0100)
#define CURRENT_550MA  (0b0101)
#define CURRENT_630MA  (0b0110)
#define CURRENT_700MA  (0b0111)

class AXP192Component : public PollingComponent, public i2c::I2CDevice {
public:
    void set_batterylevel_sensor(sensor::Sensor *batterylevel_sensor) { batterylevel_sensor_ = batterylevel_sensor; }
    void set_battery_state(binary_sensor::BinarySensor *battery_state) { battery_state_ = battery_state; }
    void set_battery_charging(binary_sensor::BinarySensor *battery_charging) { battery_charging_ = battery_charging; }
    void set_brightness(number::Number *brightness) { brightness_ = brightness; }
    void set_poweroff(button::Button *poweroff) { poweroff_ = poweroff; }
    // ========== INTERNAL METHODS ==========
    // (In most use cases you won't need these)
    void setup() override;
    void dump_config() override;
    float get_setup_priority() const override;
    void update() override;
    void brightness_callback(float value);
    void poweroff_callback();

protected:
    sensor::Sensor *batterylevel_sensor_{nullptr};
    binary_sensor::BinarySensor *battery_state_{nullptr};
    binary_sensor::BinarySensor *battery_charging_{nullptr};
    number::Number *brightness_{nullptr};
    button::Button *poweroff_{nullptr};

    /**
     * LDO2: Display backlight
     * LDO3: Display Control
     * RTC: Don't set GPIO1 as LDO
     * DCDC1: Main rail. When not set the controller shuts down.
     * DCDC3: Use unknown
     */
    void  begin(bool disableLDO2 = false, bool disableLDO3 = false, bool disableRTC = false, bool disableDCDC1 = false, bool disableDCDC3 = false);
    bool  GetBatState();
    bool GetBatCharging();
    uint8_t  GetBatData();
  
    void  EnableCoulombcounter(void);
    void  DisableCoulombcounter(void);
    void  StopCoulombcounter(void);
    void  ClearCoulombcounter(void);
    uint32_t GetCoulombchargeData(void);
    uint32_t GetCoulombdischargeData(void);
    float GetCoulombData(void); 
    
    uint16_t GetVbatData(void) __attribute__((deprecated));
    uint16_t GetIchargeData(void) __attribute__((deprecated));
    uint16_t GetIdischargeData(void) __attribute__((deprecated));
    uint16_t GetTempData(void) __attribute__((deprecated));
    uint32_t GetPowerbatData(void) __attribute__((deprecated));
    uint16_t GetVinData(void) __attribute__((deprecated));
    uint16_t GetIinData(void) __attribute__((deprecated));
    uint16_t GetVusbinData(void) __attribute__((deprecated));
    uint16_t GetIusbinData(void) __attribute__((deprecated));
    uint16_t GetVapsData(void) __attribute__((deprecated));
    uint8_t GetBtnPress(void);

      // -- sleep
    void SetSleep(void);
    void DeepSleep(uint64_t time_in_us = 0);
    void LightSleep(uint64_t time_in_us = 0);

    // void SetChargeVoltage( uint8_t );
    void  SetChargeCurrent( uint8_t );
    float GetBatVoltage();
    float GetBatCurrent();
    float GetVinVoltage();
    float GetVinCurrent();
    float GetVBusVoltage();
    float GetVBusCurrent();
    float GetTempInAXP192();
    float GetBatPower();
    float GetBatChargeCurrent();
    float GetAPSVoltage();
    float GetBatCoulombInput();
    float GetBatCoulombOut();
    uint8_t GetWarningLevel(void);	
    void SetCoulombClear();
    void SetLDO2( bool State );
    void SetLDO3( bool State );
    void SetAdcState(bool State);
    
    void PowerOff();

    
    void Write1Byte( uint8_t Addr ,  uint8_t Data );
    uint8_t Read8bit( uint8_t Addr );
    uint16_t Read12Bit( uint8_t Addr);
    uint16_t Read13Bit( uint8_t Addr);
    uint16_t Read16bit( uint8_t Addr );
    uint32_t Read24bit( uint8_t Addr );
    uint32_t Read32bit( uint8_t Addr );
    void ReadBuff( uint8_t Addr , uint8_t Size , uint8_t *Buff );
}; 

class Brightness : public number::Number
{
public:
    void control(float value)
    {
        this->state = value;
        this->publish_state(value);
    }
};

class Poweroff : public button::Button
{
public:
    void press_action() override
    {

    }
}

}
}
#endif
