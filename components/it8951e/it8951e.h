#pragma once

#include "esphome/core/component.h"
#include "esphome/core/version.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace it8951e {

enum it8951eModel {
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
----------------------------------------
INIT (初始化模式)
用于完全擦除显示屏并将其置于白色状态。适用于设备断电后重新上电等场景，
此时内存中的显示信息与屏幕实际光学状态不一致。该波形会多次切换显示屏并最终停留在白色状态。

DU (直接更新模式)
一种非常快速且无闪烁的更新模式。仅支持从任意灰度级直接转换为纯黑或纯白。
不能用于更新到除黑白外的其他灰度级。因其极快的更新速度，适用于触摸输入、手写笔响应或菜单指示器。

GC16 (灰度清除模式)
用于全屏更新并提供高质量图像。使用全屏更新命令时，整个屏幕会随新图像刷新；
若使用局部更新命令，则仅更新灰度值发生变化的像素。GC16 模式支持 16 级灰度。

GL16 (灰度文本模式)
主要用于在白色背景上更新稀疏内容（如抗锯齿文本），可减少闪烁。
GL16 波形支持 16 级灰度。

GLR16 (优化文本模式)
结合图像预处理算法使用，可在白色背景上更新稀疏内容，同时减少闪烁和图像伪影。
支持 16 级灰度。若仅使用偶数像素状态（0,2,4,...,30），其行为与传统 GL16 完全相同；
若配合专用预处理算法，像素状态 29 和 31 可进一步提升显示质量。

GLD16 (深度优化模式)
结合图像预处理算法使用，专为白色背景上的稀疏内容设计，可显著减少闪烁和伪影。
建议仅用于全屏更新。支持 16 级灰度。其特性与 GLR16 类似，但背景刷新效果更优。

DU4 (四灰度快速模式)
一种快速无闪烁波形（类似 DU），支持从任意灰度级转换到灰度级 1/6/11/16（对应像素状态 [0,10,20,30]）。
四灰度特性使其适用于菜单中的抗锯齿文本，但残影略高于 GC16。

A2 (黑白动画模式)
专为快速翻页或简单黑白动画设计的快速无闪烁模式。仅支持黑白之间的转换。
推荐在连续 A2 更新前插入白色图像过渡，以减少残影并提升画质。
*/

  struct IT8951DevInfo_s {
      uint16_t usPanelW;        // 面板宽度
      uint16_t usPanelH;        // 面板高度
      uint16_t usImgBufAddrL;   // 图像缓冲区地址低16位
      uint16_t usImgBufAddrH;   // 图像缓冲区地址高16位
      char usFWVersion[16];     // 固件版本
      char usLUTVersion[16];    // LUT 版本
  };

  struct IT8951Dev_s {
      struct IT8951DevInfo_s devInfo;
      display::DisplayType displayType;
  };

  enum update_mode_e {          //             典型特性
      UPDATE_MODE_INIT = 0,     // * 无        2000ms       显示屏初始化
      UPDATE_MODE_DU   = 1,     //   低        260ms        单色菜单、文本输入、触摸屏响应
      UPDATE_MODE_GC16 = 2,     // * 极低      450ms        高质量图像
      UPDATE_MODE_GL16 = 3,     // * 中        450ms        白底文本
      UPDATE_MODE_GLR16 = 4,    //   低        450ms        白底文本（优化）
      UPDATE_MODE_GLD16 = 5,    //   低        450ms        白底图文（深度优化）
      UPDATE_MODE_DU4 = 6,      // * 中        120ms        快速翻页（降低对比度）
      UPDATE_MODE_A2 = 7,       //   中        290ms        菜单抗锯齿文本/触摸响应
      UPDATE_MODE_NONE = 8      // 带 * 的模式更常用
  };

  void set_reset_pin(GPIOPin *reset) { this->reset_pin_ = reset; }
  void set_busy_pin(GPIOPin *busy) { this->busy_pin_ = busy; }

  void set_reversed(bool reversed) { this->reversed_ = reversed; }
  void set_reset_duration(uint32_t reset_duration) { this->reset_duration_ = reset_duration; }
  void set_model(it8951eModel model) { this->model_ = model; }

  void setup() override;
  void update() override;
  void update_slow();
  void dump_config() override;

  display::DisplayType get_display_type() override { return IT8951DevAll[this->model_].displayType; }

  void clear(bool init);

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  uint32_t get_buffer_length_();

 private:
  // 设备信息表（按型号索引）
  struct IT8951Dev_s IT8951DevAll[it8951eModel::it8951eModelsEND] = {
    { // it8951eModel::M5EPD
      .devInfo = {
        .usPanelW = 960,
        .usPanelH = 540,
        .usImgBufAddrL = 0x36E0,
        .usImgBufAddrH = 0x0012,
        .usFWVersion = "",
        .usLUTVersion = ""
      },
      .displayType = display::DisplayType::DISPLAY_TYPE_GRAYSCALE // M5EPD 支持16级灰度
    }
  };

  uint8_t *should_write_buffer_{nullptr};
  void get_device_info(struct IT8951DevInfo_s *info);

  // 脏区域跟踪
  uint32_t max_x = 0;
  uint32_t max_y = 0;
  uint32_t min_x = 960;
  uint32_t min_y = 540;
  uint16_t m_endian_type, m_pix_bpp;

  // 硬件引脚
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *busy_pin_{nullptr};

  // 配置参数
  bool reversed_ = false;
  uint32_t reset_duration_{100};
  enum it8951eModel model_{it8951eModel::M5EPD};

  // 核心操作函数
  void reset(void);
  void wait_busy(uint32_t timeout = 30);
  void check_busy(uint32_t timeout = 30);
  uint16_t get_vcom();
  void set_vcom(uint16_t vcom);

  // SPI 通信
  uint16_t read_word();
  void read_words(void *buf, uint32_t length);
  void write_two_byte16(uint16_t type, uint16_t cmd);
  void write_command(uint16_t cmd);
  void write_word(uint16_t cmd);
  void write_reg(uint16_t addr, uint16_t data);
  
  // 🔥 关键修复：添加 uint32_t 版本的地址设置函数
  void set_target_memory_addr(uint32_t tar_addr);          // 新增声明
  void set_target_memory_addr(uint16_t tar_addrL, uint16_t tar_addrH); // 原有声明

  void write_args(uint16_t cmd, uint16_t *args, uint16_t length);
  void set_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void update_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h, update_mode_e mode);

  // 显示控制
  void write_buffer_to_display(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *gram);
  void write_display();
  void write_display_slow();
};

// IT8951 寄存器地址常量（来自官方驱动）
static const uint16_t IT8951_LISAR = 0x0208; // 目标内存地址寄存器

// 自动化操作类
template<typename... Ts> class ClearAction : public Action<Ts...>, public Parented<IT8951ESensor> {
 public:
  void play(Ts... x) override { this->parent_->clear(true); }
};

template<typename... Ts> class UpdateSlowAction : public Action<Ts...>, public Parented<IT8951ESensor> {
 public:
  void play(Ts... x) override { this->parent_->update_slow(); }
};

}  // namespace it8951e
}  // namespace esphome