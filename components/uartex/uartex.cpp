#include "uartex.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/application.h"

namespace esphome {
namespace uartex {
static const char *TAG = "uartex";
void UARTExComponent::dump_config()
{
    ESP_LOGCONFIG(TAG, "  RX Receive Timeout: %d", conf_rx_timeout_);
    ESP_LOGCONFIG(TAG, "  TX Transmission Timeout: %d", conf_tx_timeout_);
    ESP_LOGCONFIG(TAG, "  TX Retry Count: %d", conf_tx_retry_cnt_);
    if (tx_ctrl_pin_)   LOG_PIN("  TX Ctrl Pin: ", tx_ctrl_pin_);
    if (rx_header_.has_value()) ESP_LOGCONFIG(TAG, "  Data rx_header: %s", to_hex_string(rx_header_.value()).c_str());
    if (rx_footer_.has_value()) ESP_LOGCONFIG(TAG, "  Data rx_footer: %s", to_hex_string(rx_footer_.value()).c_str());
    if (tx_header_.has_value()) ESP_LOGCONFIG(TAG, "  Data tx_header: %s", to_hex_string(tx_header_.value()).c_str());
    if (tx_footer_.has_value()) ESP_LOGCONFIG(TAG, "  Data tx_footer: %s", to_hex_string(tx_footer_.value()).c_str());
    ESP_LOGCONFIG(TAG, "  Data rx_checksum: %d", rx_checksum_);
    ESP_LOGCONFIG(TAG, "  Data tx_checksum: %d", tx_checksum_);
    ESP_LOGCONFIG(TAG, "  Device count: %d", devices_.size());
}

void UARTExComponent::setup()
{
    if (this->tx_ctrl_pin_)
    {
        this->tx_ctrl_pin_->setup();
        this->tx_ctrl_pin_->digital_write(false);
    }
    if (rx_checksum_ != CHECKSUM_NONE) rx_parser_.set_checksum_len(1);
    if (rx_checksum_2_ != CHECKSUM_NONE) rx_parser_.set_checksum_len(2);
    rx_time_ = get_time();
    tx_time_ = get_time();
    if (rx_header_.has_value()) rx_parser_.add_headers(rx_header_.value());
    if (rx_footer_.has_value()) rx_parser_.add_footers(rx_footer_.value());
    if (this->error_) this->error_->publish_state("None");
    if (this->version_) this->version_->publish_state(UARTEX_VERSION);
    ESP_LOGI(TAG, "Initaialize.");
}

void UARTExComponent::loop()
{
    read_from_uart();
    publish_to_devices();
    write_to_uart();
}

void UARTExComponent::read_from_uart()
{
    rx_parser_.clear();
    unsigned long timer = get_time();
    while (elapsed_time(timer) < conf_rx_timeout_)
    {
        while (this->available())
        {
            uint8_t byte;
            if (this->read_byte(&byte))
            {
                if (rx_parser_.parse_byte(byte)) return;
                if (validate_data() == ERROR_NONE) return;
                timer = get_time();
            }
        }
        delay(1);
    }
}

void UARTExComponent::publish_to_devices()
{
    if (rx_parser_.buffer().size() == 0) return;
    ERROR error = validate_data();
    if (error != ERROR_NONE)
    {
        publish_error(error);
        return;
    }
    publish_error(ERROR_NONE);
    verify_ack();
    publish_data();
    rx_time_ = get_time();
}

bool UARTExComponent::verify_ack()
{
    if (!is_have_tx_data()) return false;
    if (tx_device() == nullptr) return false;
    if (!tx_device()->equal(rx_parser_.data(), tx_cmd()->ack)) return false;
    tx_data_response(true);
    ESP_LOGD(TAG, "Ack: %s, Gap Time: %lums", to_hex_string(rx_parser_.buffer()).c_str(), elapsed_time(tx_time_));
    return true;
}

void UARTExComponent::publish_data()
{
    bool found = false;
    for (UARTExDevice* device : this->devices_)
    {
        if (device->parse_data(rx_parser_.data()))
        {
            found = true;
        }
    }
#ifdef ESPHOME_LOG_HAS_VERY_VERBOSE
    ESP_LOGVV(TAG, "Receive data-> %s, Gap Time: %lums", to_hex_string(rx_parser_.buffer()).c_str(), elapsed_time(rx_time_));
#endif
#ifdef ESPHOME_LOG_HAS_VERBOSE
    if (!found) ESP_LOGV(TAG, "Notfound data-> %s", to_hex_string(rx_parser_.buffer()).c_str());
#endif
}

void UARTExComponent::dequeue_tx_data_from_devices()
{
    for (UARTExDevice* device : this->devices_)
    {
        const cmd_t *cmd = device->dequeue_tx_cmd();
        if (cmd == nullptr) continue;
        enqueue_tx_data({device, cmd}, false);

        const cmd_t *cmd_low_priority = device->dequeue_tx_cmd_low_priority();
        if (cmd_low_priority == nullptr) continue;
        enqueue_tx_data({device, cmd_low_priority}, true);                    
    }
}

void UARTExComponent::write_to_uart()
{
    if (elapsed_time(rx_time_) < conf_tx_delay_) return;
    if (elapsed_time(tx_time_) < conf_tx_delay_) return;
    if (elapsed_time(tx_time_) < conf_tx_timeout_) return;
    if (retry_tx_data()) return;
    write_tx_data();
}

bool UARTExComponent::retry_tx_data()
{
    if (!is_have_tx_data()) return false;
    if (conf_tx_retry_cnt_ <= tx_retry_cnt_)
    {
        tx_data_response(false);
        ESP_LOGD(TAG, "Retry fail.");
        publish_error(ERROR_ACK);
        return false;
    }
    ESP_LOGD(TAG, "Retry count: %d", tx_retry_cnt_);
    write_tx_cmd();
    return true;
}

void UARTExComponent::write_tx_data()
{
    dequeue_tx_data_from_devices();
    if (!tx_queue_.empty())
    {
        tx_data_ = tx_queue_.front();
        tx_queue_.pop();
        write_tx_cmd();
    }
    else if (!tx_queue_low_priority_.empty())
    {
        tx_data_ = tx_queue_low_priority_.front();
        tx_queue_low_priority_.pop();
        write_tx_cmd();
    }
}

void UARTExComponent::write_tx_cmd()
{
    unsigned long timer = get_time();
    if (tx_ctrl_pin_) tx_ctrl_pin_->digital_write(true);
    if (tx_header_.has_value()) write_data(tx_header_.value());
    write_data(tx_cmd()->data);
    if (tx_checksum_ != CHECKSUM_NONE) write_data(get_tx_checksum(tx_cmd()->data));
    if (tx_footer_.has_value()) write_data(tx_footer_.value());
    write_flush();
    if (tx_ctrl_pin_) tx_ctrl_pin_->digital_write(false);
    tx_retry_cnt_++;
    tx_time_ = get_time();
    if (tx_cmd()->ack.size() == 0) tx_data_response(true);
}

void UARTExComponent::write_data(const uint8_t data)
{
    this->write_byte(data);
    ESP_LOGD(TAG, "Write byte-> 0x%02X", data);
}

void UARTExComponent::write_data(const std::vector<uint8_t> &data)
{
    this->write_array(data);
    ESP_LOGD(TAG, "Write array-> %s", to_hex_string(data).c_str());
}

void UARTExComponent::enqueue_tx_data(const tx_data_t data, bool low_priority)
{
    if (low_priority) tx_queue_low_priority_.push(data);
    else tx_queue_.push(data);
}

void UARTExComponent::write_flush()
{
    this->flush();
    ESP_LOGD(TAG, "Flush.");
}

void UARTExComponent::register_device(UARTExDevice *device)
{
    devices_.push_back(device);
}

void UARTExComponent::set_tx_delay(uint16_t tx_delay)
{
    conf_tx_delay_ = tx_delay;
}

void UARTExComponent::set_tx_timeout(uint16_t timeout)
{
    conf_tx_timeout_ = timeout;
}

void UARTExComponent::set_tx_retry_cnt(uint16_t tx_retry_cnt)
{
    conf_tx_retry_cnt_ = tx_retry_cnt;
}

void UARTExComponent::set_rx_timeout(uint16_t timeout)
{
    conf_rx_timeout_ = timeout;
}

void UARTExComponent::set_tx_ctrl_pin(InternalGPIOPin *pin)
{
    tx_ctrl_pin_ = pin;
}

bool UARTExComponent::is_have_tx_data()
{
    if (tx_data_.cmd) return true;
    return false;
}

void UARTExComponent::tx_data_response(bool ok)
{
    if (tx_data_.device)
    {
        if (ok) tx_data_.device->ack_ok();
        else    tx_data_.device->ack_ng();
    }
    clear_tx_data();
}

void UARTExComponent::clear_tx_data()
{
    tx_data_.device = nullptr;
    tx_data_.cmd = nullptr;
    tx_retry_cnt_ = 0;
}

const cmd_t* UARTExComponent::tx_cmd()
{
    return tx_data_.cmd;
}

UARTExDevice* UARTExComponent::tx_device()
{
    return tx_data_.device;
}

unsigned long UARTExComponent::elapsed_time(const unsigned long timer)
{
    return millis() - timer;
}

unsigned long UARTExComponent::get_time()
{
    return millis();
}

ERROR UARTExComponent::validate_data()
{
    if (rx_parser_.data().size() == 0)
    {
        return ERROR_SIZE;
    }
    if (rx_header_.has_value() && rx_parser_.parse_header() == false)
    {
        return ERROR_HEADER;
    }
    if (rx_footer_.has_value() && rx_parser_.parse_footer() == false)
    {
        return ERROR_FOOTER;
    }
    if (rx_checksum_ != CHECKSUM_NONE && !rx_parser_.validate(get_rx_checksum(rx_parser_.data())))
    {
        return ERROR_CHECKSUM;
    }
    return ERROR_NONE;
}

bool UARTExComponent::publish_error(ERROR error_code)
{
    bool error = true;
    switch(error_code)
    {
    case ERROR_SIZE:
        ESP_LOGW(TAG, "[Read] Size error: %s", to_hex_string(rx_parser_.buffer()).c_str());
        if (this->error_ && error_code_ != ERROR_SIZE) this->error_->publish_state("Size Error");
        break;
    case ERROR_HEADER:
        ESP_LOGW(TAG, "[Read] Header error: %s", to_hex_string(rx_parser_.buffer()).c_str());
        if (this->error_ && error_code_ != ERROR_HEADER) this->error_->publish_state("Header Error");
        break;
    case ERROR_FOOTER:
        ESP_LOGW(TAG, "[Read] Footer error: %s", to_hex_string(rx_parser_.buffer()).c_str());
        if (this->error_ && error_code_ != ERROR_FOOTER) this->error_->publish_state("Footer Error");
        break;
    case ERROR_CHECKSUM:
        ESP_LOGW(TAG, "[Read] Checksum error: %s", to_hex_string(rx_parser_.buffer()).c_str());
        if (this->error_ && error_code_ != ERROR_CHECKSUM) this->error_->publish_state("Checksum Error");
        break;
    case ERROR_CHECKSUM_2:
        ESP_LOGW(TAG, "[Read] Checksum error: %s", to_hex_string(rx_parser_.buffer()).c_str());
        if (this->error_ && error_code_ != ERROR_CHECKSUM_2) this->error_->publish_state("Checksum2 Error");
        break;
    case ERROR_ACK:
        ESP_LOGW(TAG, "[Read] Ack error: %s", to_hex_string(rx_parser_.buffer()).c_str());
        if (this->error_ && error_code_ != ERROR_ACK) this->error_->publish_state("Ack Error");
        break;
    case ERROR_NONE:
        if (this->error_ && error_code_ != ERROR_NONE) this->error_->publish_state("None");
        error = false;
        break;
    }
    error_code_ = error_code;
    return error;
}

void UARTExComponent::set_rx_header(std::vector<uint8_t> header)
{
    rx_header_ = header;
}

void UARTExComponent::set_rx_footer(std::vector<uint8_t> footer)
{
    rx_footer_ = footer;
}

void UARTExComponent::set_tx_header(std::vector<uint8_t> header)
{
    tx_header_ = header;
}

void UARTExComponent::set_tx_footer(std::vector<uint8_t> footer)
{
    tx_footer_ = footer;
}

void UARTExComponent::set_rx_checksum(CHECKSUM checksum)
{
    rx_checksum_ = checksum;
}

void UARTExComponent::set_rx_checksum_lambda(std::function<uint8_t(const uint8_t *data, const uint16_t len)> &&f)
{
    rx_checksum_f_ = f;
    rx_checksum_ = CHECKSUM_CUSTOM;
}

void UARTExComponent::set_tx_checksum(CHECKSUM checksum)
{
    tx_checksum_ = checksum;
}

void UARTExComponent::set_tx_checksum_lambda(std::function<uint8_t(const uint8_t *data, const uint16_t len)> &&f)
{
    tx_checksum_f_ = f;
    tx_checksum_ = CHECKSUM_CUSTOM;
}

const std::vector<uint8_t> UARTExComponent::get_rx_checksum(const std::vector<uint8_t> &data) const
{
    if (this->rx_checksum_f_.has_value())
    {
        return (*rx_checksum_f_)(&data[0], data.size());
    }
    else
    {
        std::vector<uint8_t> checksum;
        uint8_t crc = 0;
        switch(rx_checksum_)
        {
        case CHECKSUM_ADD:
            if (this->rx_header_.has_value())
            {
                for (uint8_t byte : this->rx_header_.value()) { crc += byte; }
            }
            for (uint8_t byte : data) { crc += byte; }
            checksum.push_back(crc);
            break;
        case CHECKSUM_XOR:
            if (this->rx_header_.has_value())
            {
                for (uint8_t byte : this->rx_header_.value()) { crc ^= byte; }
            }
            for (uint8_t byte : data) { crc ^= byte; }
            checksum.push_back(crc);
            break;
        case CHECKSUM_NONE:
        case CHECKSUM_CUSTOM:
            break;
        }
        return checksum;
    }
}


const std::vector<uint8_t> UARTExComponent::get_tx_checksum(const std::vector<uint8_t> &data) const
{
    if (this->tx_checksum_f_.has_value())
    {
        return (*tx_checksum_f_)(&data[0], data.size());
    }
    else
    {
        std::vector<uint8_t> checksum;
        uint8_t crc = 0;
        switch(tx_checksum_)
        {
        case CHECKSUM_ADD:
            if (this->tx_header_.has_value())
            {
                for (uint8_t byte : this->tx_header_.value()) { crc += byte; }
            }
            for (uint8_t byte : data) { crc += byte; }
            checksum.push_back(crc);
            break;
        case CHECKSUM_XOR:
            if (this->tx_header_.has_value())
            {
                for (uint8_t byte : this->tx_header_.value()) { crc ^= byte; }
            }
            for (uint8_t byte : data) { crc ^= byte; }
            checksum.push_back(crc);
            break;
        case CHECKSUM_NONE:
        case CHECKSUM_CUSTOM:
            break;
        }
        return checksum;
    }
}

}  // namespace uartex
}  // namespace esphome