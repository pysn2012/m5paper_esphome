#pragma once

#include "esphome/core/component.h"
#include "esphome/core/version.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome::it8951e {

enum class EPaperState : uint8_t {
  IDLE,       // 空闲状态（无任何操作）
  UPDATE,     // 更新缓冲区
  RESET,      // 拉低复位引脚（复位生效）
  RESET_END,  // 拉高复位引脚（复位结束）

  SHOULD_WAIT,     // 此状态及以上需要等待显示屏退出忙状态
  INITIALISE,      // 发送初始化序列
  TRANSFER_DATA,   // 向显示屏传输数据
  POWER_ON,        // 开启显示屏电源
  REFRESH_SCREEN,  // 发送刷新命令
  POWER_OFF,       // 关闭显示屏电源
  DEEP_SLEEP,      // 显示屏进入深度睡眠
};
// 分10ms块传输数据，保证主循环能正常执行
static constexpr uint32_t MAX_TRANSFER_TIME = 10;

enum it8951eModel
{
  M5EPD = 0,
  it8951eModelsEND // 必须放在最后
};

#if ESPHOME_VERSION_CODE >= VERSION_CODE(2023, 12, 0)
class IT8951ESensor : public display::DisplayBuffer,
#else
class IT8951ESensor : public PollingComponent, public display::DisplayBuffer,
#endif  // VERSION_CODE(2023, 12, 0)
                      public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING,
                                            spi::DATA_RATE_20MHZ> {
 public:
  float get_setup_priority() const override { return setup_priority::PROCESSOR; };

/*
---------------------------------------- 刷新模式说明
---------------------------------------- INIT 初始化（INIT）模式
用于完全擦除显示屏并将其置为白色状态。适用于以下场景：当显示屏掉电后重新上电，
内存中的显示信息无法准确反映显示屏的光学状态时。该波形会多次切换显示屏状态，
最终将其置为白色。

DU
直接更新（DU）模式是一种极快、无闪烁的更新方式。此模式仅支持从任意灰度级切换到纯黑或纯白，
无法更新到除黑白外的其他灰度级。其快速更新特性使其适用于触摸传感器/手写笔输入响应、
菜单选择指示等场景。

GC16
灰度清除（GC16）模式用于全屏幕更新，可提供高画质显示效果。当配合全屏更新命令使用时，
整个屏幕会随新图像写入完成更新；若使用局部更新命令，则仅更新灰度值发生变化的像素。
GC16模式支持16级独立灰度。

GL16
GL16波形主要用于更新白色背景上的稀疏内容（如整页抗锯齿文本），可减少闪烁。
GL16模式支持16级独立灰度。

GLR16
GLR16模式需配合图像预处理算法使用，用于更新白色背景上的稀疏内容，可减少闪烁和图像伪影。
该模式支持16级灰度：若仅使用偶数像素状态（0,2,4,…30），其表现与传统GL16波形完全一致；
若配合专用图像预处理算法，像素状态29和31触发的切换可提升显示质量。对于AF波形，
GLR16波形数据与GL16指向相同的电压列表，无需单独存储。

GLD16
GLD16模式需配合图像预处理算法使用，用于更新白色背景上的稀疏内容，可减少闪烁和图像伪影。
建议仅配合全屏更新命令使用。该模式支持16级灰度：若仅使用偶数像素状态（0,2,4,…30），
其表现与传统GL16波形完全一致；若配合专用图像预处理算法，像素状态29和31触发的切换可
按照波形文件中编码的预设像素映射表刷新背景（闪烁程度低于GC16模式），相比GLR16模式能
进一步减少图像伪影。对于AF波形，GLD16波形数据与GL16指向相同的电压列表，无需单独存储。

DU4
DU4模式更新速度快（与DU相当）、无闪烁。该模式支持从任意灰度级切换到灰度级1、6、11、16
（对应像素状态[0 10 20 30]）。快速更新+四级灰度的特性使其适用于菜单中的抗锯齿文本显示，
相比GC16模式会有一定程度的残影增加。

A2
A2模式是一种快速、无闪烁的更新模式，专为快速翻页或简单黑白动画设计。此模式仅支持在黑白之间切换，
无法更新到其他灰度级。建议的A2重复更新过渡序列见图1：从4位图像切换到1位图像时，
先显示白色图像可减少残影并提升A2更新的图像质量。
*/

  struct IT8951DevInfo_s
  {
      int usPanelW;          // 面板宽度
      int usPanelH;          // 面板高度
      uint16_t usImgBufAddrL;// 图像缓冲区地址低位
      uint16_t usImgBufAddrH;// 图像缓冲区地址高位
      char usFWVersion[16];  // 固件版本
      char usLUTVersion[16]; // LUT版本
  };

  struct IT8951Dev_s
  {
      struct IT8951DevInfo_s devInfo;
      display::DisplayType displayType;
  };

  enum update_mode_e         //             典型特性
  {                          //   残影  更新时间    适用场景
      UPDATE_MODE_INIT = 0,  // * 无    2000ms      显示屏初始化
      UPDATE_MODE_DU   = 1,  //   低    260ms       单色菜单、文本输入、触摸屏输入
      UPDATE_MODE_GC16 = 2,  // * 极低  450ms       高质量图像显示
      UPDATE_MODE_GL16 = 3,  // * 中等  450ms       白色背景文本显示
      UPDATE_MODE_GLR16 = 4, //   低    450ms       白色背景文本显示
      UPDATE_MODE_GLD16 = 5, //   低    450ms       白色背景文本和图形显示
      UPDATE_MODE_DU4 = 6,   // * 中等  120ms       快速翻页（对比度降低）
      UPDATE_MODE_A2 = 7,    //   中等  290ms       菜单抗锯齿文本/触摸屏幕输入
      UPDATE_MODE_NONE = 8   // 无更新
  };  // 标*的为更常用模式

  void set_reset_pin(GPIOPin *reset) { this->reset_pin_ = reset; }
  void set_busy_pin(GPIOPin *busy) { this->busy_pin_ = busy; }

  void set_reversed(bool reversed) { this->reversed_ = reversed; }
  void set_reset_duration(uint32_t reset_duration) { this->reset_duration_ = reset_duration; }
  void set_model(it8951eModel model);
  void set_sleep_when_done(bool sleep_when_done) { this->sleep_when_done_ = sleep_when_done; }
  void set_full_update_every(uint32_t full_update_every) { this->full_update_every_ = full_update_every; }

  void setup() override;    // 初始化设置
  void loop() override;     // 主循环
  void update() override;   // 快速更新
  void update_slow();       // 慢速更新
  void dump_config() override; // 打印配置信息
  // 获取显示类型
  display::DisplayType get_display_type() override { return IT8951DevAll[this->model_].displayType; }

  void clear(bool init);// 清屏（init=true时执行初始化清屏）

  void fill(Color color) override;// 填充指定颜色
  void draw_pixel_at(int x, int y, Color color) override;// 在指定坐标绘制像素
  void write_display(update_mode_e mode);// 按指定模式写入显示内容

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override; // 内部绝对坐标像素绘制实现

  int get_width_internal() override { return usPanelW_; };

  int get_height_internal() override { return usPanelH_; };

  uint32_t get_buffer_length_();// 获取缓冲区长度


 private:
  // 设备型号配置表
  struct IT8951Dev_s IT8951DevAll[it8951eModel::it8951eModelsEND]
  { // it8951eModel::M5EPD 型号配置
    960,    // .devInfo.usPanelW - 面板宽度
    540,    // .devInfo.usPanelH - 面板高度
    0x36E0, // .devInfo.usImgBufAddrL - 图像缓冲区地址低位
    0x0012, // .devInfo.usImgBufAddrH - 图像缓冲区地址高位
    "",     // .devInfo.usFWVersion - 固件版本（预留）
    "",     // .devInfo.usLUTVersion - LUT版本（预留）
    display::DisplayType::DISPLAY_TYPE_GRAYSCALE // .displayType (M5EPD支持16级灰度)
  };

  int max_x = 0;            // 最大X坐标（更新区域）
  int max_y = 0;            // 最大Y坐标（更新区域）
  int min_x = 960;          // 最小X坐标（更新区域）
  int min_y = 540;          // 最小Y坐标（更新区域）
  uint16_t m_endian_type = 0; // 字节序类型
  uint16_t m_pix_bpp = 0;   // 像素位深度
  uint8_t _it8951_rotation = 0; // 旋转角度

  GPIOPin *reset_pin_{nullptr}; // 复位引脚
  GPIOPin *busy_pin_{nullptr};  // 忙状态引脚

  int usPanelW_{0};         // 面板宽度
  int usPanelH_{0};         // 面板高度
  bool reversed_{false};    // 是否反转显示
  uint32_t reset_duration_{100}; // 复位持续时间（默认100ms）
  bool sleep_when_done_{true}; // 更新完成后是否休眠（默认开启）
  uint32_t full_update_every_{60}; // 每60次更新执行一次全屏更新
  uint32_t partial_update_{0}; // 局部更新计数器
  enum it8951eModel model_{it8951eModel::M5EPD}; // 默认设备型号

  void reset(void); // 执行复位操作

  /* 1000ms超时设置：实测最长耗时约750ms（平均约310ms）
   * 主要用于屏幕休眠和命令执行等待
   */
  void wait_busy(uint32_t timeout = 1000);  // 等待忙状态结束
  void check_busy(uint32_t timeout = 1000); // 检查忙状态

  uint16_t get_vcom();      // 获取VCOM电压值
  void set_vcom(uint16_t vcom); // 设置VCOM电压值

  // 来自Waveshare官方参考驱动代码
  uint16_t read_word();     // 读取16位数据

  void write_two_byte16(uint16_t type, uint16_t cmd); // 写入两个16位数据
  void write_command(uint16_t cmd); // 写入命令
  void write_word(uint16_t cmd);    // 写入16位数据
  void write_reg(uint16_t addr, uint16_t data); // 写入寄存器
  // 设置目标内存地址
  void set_target_memory_addr(uint16_t tar_addrL, uint16_t tar_addrH);
  void write_args(uint16_t cmd, uint16_t *args, uint16_t length);
  // 设置更新区域
  void set_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void update_area(uint16_t x, uint16_t y, uint16_t w,
                    uint16_t h, update_mode_e mode);



  void process_state_();
  void set_state_(EPaperState state, uint16_t delay = 0);
  bool is_idle_() const;
  bool prepare_transfer_(update_mode_e &mode);
  bool transfer_row_data_();
  bool is_display_busy_();
  uint8_t color_to_nibble_(const Color &color) const;

  EPaperState state_{EPaperState::IDLE}; // 当前状态（默认空闲）
  uint32_t delay_until_{0};             // 延迟执行时间戳
  bool waiting_for_idle_{false};        // 是否等待空闲状态
  bool initialized_{false};             // 是否已完成初始化

  update_mode_e pending_mode_{update_mode_e::UPDATE_MODE_NONE}; // 待执行的更新模式
  uint16_t pending_x_{0};               // 待更新区域X坐标
  uint16_t pending_y_{0};               // 待更新区域Y坐标
  uint16_t pending_w_{0};               // 待更新区域宽度
  uint16_t pending_h_{0};               // 待更新区域高度
  uint16_t transfer_row_{0};            // 当前传输行
  uint32_t draw_calls_since_yield_{0};  // 自上次让步后的绘制调用次数
  uint32_t update_started_at_{0};       // 更新开始时间戳
  bool update_timing_active_{false};    // 更新计时是否激活
  bool did_init_clear_{false};          // 是否执行过初始化清屏
  uint32_t clear_count_{0};             // 清屏计数器
  static constexpr uint32_t INIT_CLEAR_EVERY = 12; // 每12次执行一次初始化清屏
  bool update_pending_{false};          // 是否有待执行的更新
  update_mode_e queued_update_mode_{update_mode_e::UPDATE_MODE_NONE}; // 排队等待的更新模式
};
// 清屏动作类
template<typename... Ts> class ClearAction : public Action<Ts...>, public Parented<IT8951ESensor> {
 public:
  void play(const Ts &... x) override { this->parent_->clear(true); }
};
// 慢速更新动作类
template<typename... Ts> class UpdateSlowAction : public Action<Ts...>, public Parented<IT8951ESensor> {
 public:
  void play(const Ts &... x) override { this->parent_->update_slow(); }
};
// 绘制动作类（DU模式）
template<typename... Ts> class DrawAction : public Action<Ts...>, public Parented<IT8951ESensor> {
 public:
  void play(const Ts &... x) override { this->parent_->write_display(IT8951ESensor::UPDATE_MODE_DU); }
};

}  // namespace esphome::it8951e
