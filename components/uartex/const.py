# coding=utf-8
"""Constants used by esphome."""

CONF_UARTEX_ID = 'uartex_id'
CONF_RX_TIMEOUT = 'rx_timeout'
CONF_RX_LENGTH = 'rx_length'
CONF_TX_DELAY = 'tx_delay'
CONF_TX_TIMEOUT = 'tx_timeout'
CONF_TX_RETRY_CNT = 'tx_retry_cnt'
CONF_TX_CTRL_PIN = 'tx_ctrl_pin'
CONF_RX_HEADER = 'rx_header'
CONF_RX_FOOTER = 'rx_footer'
CONF_TX_HEADER = 'tx_header'
CONF_TX_FOOTER = 'tx_footer'
CONF_RX_CHECKSUM = 'rx_checksum'
CONF_TX_CHECKSUM = 'tx_checksum'
CONF_RX_CHECKSUM_2 = 'rx_checksum2'
CONF_TX_CHECKSUM_2 = 'tx_checksum2'
CONF_MASK = 'mask'
CONF_ERROR = 'error'
CONF_LOG = 'log'
CONF_DISABLED = "disabled"
CONF_LOCK_TIMEOUT = 'lock_timeout'
CONF_UNLOCK_TIMEOUT = 'unlock_timeout'

CONF_STATE = 'state'
CONF_STATE_ON = 'state_on'
CONF_STATE_OFF = 'state_off'
CONF_STATE_NUMBER = 'state_number'
CONF_STATE_RESPONSE = 'state_response'
CONF_STATE_TEMPERATURE_CURRENT = 'state_temperature_current'
CONF_STATE_TEMPERATURE_TARGET = 'state_temperature_target'
CONF_STATE_HUMIDITY_CURRENT = 'state_humidity_current'
CONF_STATE_HUMIDITY_TARGET = 'state_humidity_target'
CONF_STATE_COOL = 'state_cool'
CONF_STATE_HEAT = 'state_heat'
CONF_STATE_FAN_ONLY = 'state_fan_only'
CONF_STATE_DRY = 'state_dry'
CONF_STATE_AUTO = 'state_auto'
CONF_STATE_SWING_OFF = 'state_swing_off'
CONF_STATE_SWING_BOTH = 'state_swing_both'
CONF_STATE_SWING_VERTICAL = 'state_swing_vertical'
CONF_STATE_SWING_HORIZONTAL = 'state_swing_horizontal'
CONF_STATE_FAN_ON = 'state_fan_on'
CONF_STATE_FAN_OFF = 'state_fan_off'
CONF_STATE_FAN_AUTO = 'state_fan_auto'
CONF_STATE_FAN_LOW = 'state_fan_low'
CONF_STATE_FAN_MEDIUM = 'state_fan_medium'
CONF_STATE_FAN_HIGH = 'state_fan_high'
CONF_STATE_FAN_MIDDLE = 'state_fan_middle'
CONF_STATE_FAN_FOCUS = 'state_fan_focus'
CONF_STATE_FAN_DIFFUSE = 'state_fan_diffuse'
CONF_STATE_FAN_QUIET = 'state_fan_quiet'
CONF_STATE_PRESET_NONE = 'state_preset_none'
CONF_STATE_PRESET_HOME = 'state_preset_home'
CONF_STATE_PRESET_AWAY = 'state_preset_away'
CONF_STATE_PRESET_BOOST = 'state_preset_boost'
CONF_STATE_PRESET_COMFORT = 'state_preset_comfort'
CONF_STATE_PRESET_ECO = 'state_preset_eco'
CONF_STATE_PRESET_SLEEP = 'state_preset_sleep'
CONF_STATE_PRESET_ACTIVITY = 'state_preset_activity'
CONF_STATE_CUSTOM_FAN = 'state_custom_fan'
CONF_STATE_CUSTOM_PRESET = 'state_custom_preset'
CONF_STATE_LOCKED = 'state_locked'
CONF_STATE_UNLOCKED = 'state_unlocked'
CONF_STATE_JAMMED = 'state_jammed'
CONF_STATE_LOCKING = 'state_locking'
CONF_STATE_UNLOCKING = 'state_unlocking'
CONF_STATE_SPEED = 'state_speed'
CONF_STATE_PRESET = 'state_preset'
CONF_STATE_BRIGHTNESS = 'state_brightness'
CONF_STATE_TEXT = 'state_text'
CONF_STATE_OPEN = 'state_open'
CONF_STATE_CLOSED = 'state_closed'
CONF_STATE_POSITION = 'state_position'

CONF_COMMAND_UPDATE = 'command_update'
CONF_COMMAND_ON = 'command_on'
CONF_COMMAND_OFF = 'command_off'
CONF_COMMAND_TEMPERATURE = 'command_temperature'
CONF_COMMAND_HUMIDITY = 'command_humidity'
CONF_COMMAND_COOL = 'command_cool'
CONF_COMMAND_HEAT = 'command_heat'
CONF_COMMAND_FAN_ONLY = 'command_fan_only'
CONF_COMMAND_DRY = 'command_dry'
CONF_COMMAND_AUTO = 'command_auto'
CONF_COMMAND_SWING_OFF = 'command_swing_off'
CONF_COMMAND_SWING_BOTH = 'command_swing_both'
CONF_COMMAND_SWING_VERTICAL = 'command_swing_vertical'
CONF_COMMAND_SWING_HORIZONTAL = 'command_swing_horizontal'
CONF_COMMAND_FAN_ON = 'command_fan_on'
CONF_COMMAND_FAN_OFF = 'command_fan_off'
CONF_COMMAND_FAN_AUTO = 'command_fan_auto'
CONF_COMMAND_FAN_LOW = 'command_fan_low'
CONF_COMMAND_FAN_MEDIUM = 'command_fan_medium'
CONF_COMMAND_FAN_HIGH = 'command_fan_high'
CONF_COMMAND_FAN_MIDDLE = 'command_fan_middle'
CONF_COMMAND_FAN_FOCUS = 'command_fan_focus'
CONF_COMMAND_FAN_DIFFUSE = 'command_fan_diffuse'
CONF_COMMAND_FAN_QUIET = 'command_fan_quiet'
CONF_COMMAND_PRESET_NONE = 'command_preset_none'
CONF_COMMAND_PRESET_HOME = 'command_preset_home'
CONF_COMMAND_PRESET_AWAY = 'command_preset_away'
CONF_COMMAND_PRESET_BOOST = 'command_preset_boost'
CONF_COMMAND_PRESET_COMFORT = 'command_preset_comfort'
CONF_COMMAND_PRESET_ECO = 'command_preset_eco'
CONF_COMMAND_PRESET_SLEEP = 'command_preset_sleep'
CONF_COMMAND_PRESET_ACTIVITY = 'command_preset_activity'
CONF_COMMAND_CUSTOM_FAN = 'command_custom_fan'
CONF_COMMAND_CUSTOM_PRESET = 'command_custom_preset'
CONF_COMMAND_LOCK = 'command_lock'
CONF_COMMAND_UNLOCK = 'command_unlock'
CONF_COMMAND_NUMBER = 'command_number'
CONF_COMMAND_SPEED = 'command_speed'
CONF_COMMAND_PRESET = 'command_preset'
CONF_COMMAND_BRIGHTNESS = 'command_brightness'
CONF_COMMAND_TEXT = 'command_text'
CONF_COMMAND_OPEN = 'command_open'
CONF_COMMAND_CLOSE = 'command_close'
CONF_COMMAND_STOP = 'command_stop'
CONF_ACK = 'ack'
CONF_LENGTH = 'length'
CONF_PRECISION = 'precision'
CONF_SIGNED = 'signed'
CONF_ENDIAN = 'endian'
CONF_SPEED_CNT = 'speed_cnt'
CONF_ON_WRITE = 'on_write'
CONF_ON_READ = 'on_read'

