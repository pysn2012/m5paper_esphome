import esphome.codegen as cg
from esphome import pins
import esphome.config_validation as cv
from esphome import automation
from esphome.const import __version__ as ESPHOME_VERSION
from esphome.const import (
    CONF_ID,
)
# M5Paper 命名空间
m5paper_ns = cg.esphome_ns.namespace('m5paper')
# M5Paper 组件类定义
M5PaperComponent = m5paper_ns.class_('M5PaperComponent', cg.Component)
# 电源控制动作类
PowerAction = m5paper_ns.class_("PowerAction", automation.Action)
# 配置项常量
CONF_MAIN_POWER_PIN = "main_power_pin"            # 主电源引脚
CONF_BATTERY_POWER_PIN = "battery_power_pin"      # 电池电源引脚
CONF_ALLOW_ESPHOME_DEEP_SLEEP = "allow_esphome_deep_sleep"   # 允许 ESPHome 深度休眠
# 配置验证模式
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(M5PaperComponent),
    cv.Required(CONF_MAIN_POWER_PIN): pins.internal_gpio_output_pin_schema,
    cv.Required(CONF_BATTERY_POWER_PIN): pins.internal_gpio_output_pin_schema,
    cv.Optional(CONF_ALLOW_ESPHOME_DEEP_SLEEP, default=True): cv.boolean
})

# `synchronous` 关键字参数在较新的 ESPHome 版本中添加；仅在支持时传递
_it8951e_action_synchronous = {}
if cv.Version.parse(ESPHOME_VERSION) >= cv.Version.parse("2026.3.0"):
    _it8951e_action_synchronous["synchronous"] = True
# 注册关闭主电源动作
@automation.register_action(
    "m5paper.shutdown_main_power",
    PowerAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(M5PaperComponent),
        }
    ),
    **_it8951e_action_synchronous
)
# 关闭主电源动作代码生成
async def m5paper_shutdown_main_power_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var

# 主代码生成函数
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # 设置主电源引脚
    if CONF_MAIN_POWER_PIN in config:
        power = await cg.gpio_pin_expression(config[CONF_MAIN_POWER_PIN])
        cg.add(var.set_main_power_pin(power))
    # 设置电池电源引脚
    if CONF_BATTERY_POWER_PIN in config:
        power = await cg.gpio_pin_expression(config[CONF_BATTERY_POWER_PIN])
        cg.add(var.set_battery_power_pin(power))
    # 设置是否允许深度休眠
    if CONF_ALLOW_ESPHOME_DEEP_SLEEP in config:
        cg.add(var.set_allow_esphome_deep_sleep(config[CONF_ALLOW_ESPHOME_DEEP_SLEEP]))