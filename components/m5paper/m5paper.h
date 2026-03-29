#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/core/automation.h"

namespace esphome::m5paper {
// M5Paper 组件类
class M5PaperComponent : public Component {
    // 初始化设置
	void setup() override;
    // 输出配置信息
	void dump_config() override;
    // 设置优先级：BUS 级别（非常早的初始化，负责为其他组件供电）
    float get_setup_priority() const override { return setup_priority::BUS; };

public:
    // 设置电池电源引脚
	void set_battery_power_pin(GPIOPin *power) { this->battery_power_pin_ = power; }
    // 设置主电源引脚
	void set_main_power_pin(GPIOPin *power) { this->main_power_pin_ = power; }
    // 设置是否允许 ESPHome 深度休眠
	void set_allow_esphome_deep_sleep(bool sleep) { this->allow_esphome_deep_sleep_ = sleep; }
    // 关闭主电源
	void shutdown_main_power();

private:
    GPIOPin *battery_power_pin_{nullptr};   // 电池电源引脚
    GPIOPin *main_power_pin_{nullptr};      // 主电源引脚
    // 深度休眠时保持电源线高电平的 hack
    // 深度休眠模式下电池续航不佳，建议使用 bm8563 休眠
    bool allow_esphome_deep_sleep_{true};

};
// 电源控制动作模板类
template<typename... Ts> class PowerAction : public Action<Ts...>, public Parented<M5PaperComponent> {
public:
    // 执行动作：关闭主电源
	void play(const Ts &... x) override { this->parent_->shutdown_main_power(); }
};

} //namespace esphome::m5paper
