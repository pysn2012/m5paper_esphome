# ESPHome 适配 M5Paper 显示组件

适用于搭载 IT8951E 控制器的 M5Stack Paper V1.1 电子纸显示屏的 ESPHome 外部组件。

## 硬件参数

|            规格项             |              数值              |
| :---------------------------: | :----------------------------: |
|           产品型号            |       M5Stack Paper V1.1       |
|              SKU              |             K049-B             |
|            显示屏             | 4.7 英寸 电子墨水屏 (ED047TC1) |
|            分辨率             |         540 x 960 像素         |
|            控制器             |            IT8951E             |
|           灰度等级            |          16 级 (4 位)          |
|        系统芯片（SoC）        |        ESP32-D0WDQ6-V3         |
|             闪存              |              16MB              |
| 伪静态随机存取存储器（PSRAM） |              8MB               |

## 引脚映射 (M5Stack Paper V1.1)

| 信号 | 通用输入输出引脚（GPIO） |            描述             |
| :--: | :----------------------: | :-------------------------: |
| MOSI |          GPIO12          | 串行外设接口（SPI）数据输出 |
| MISO |          GPIO13          | 串行外设接口（SPI）数据输入 |
| SCK  |          GPIO14          |   串行外设接口（SPI）时钟   |
|  CS  |          GPIO15          |            片选             |
| BUSY |          GPIO27          |       IT8951E 忙状态        |
| RST  |          GPIO4           |        复位（可选）         |

## 功能特性

- **16 级灰度**（每像素 4 位）
- **多种刷新模式**，可在显示质量和刷新速度间权衡
- **支持旋转**（0°、90°、180°、270°）
- **深度睡眠** 省电模式
- **帧缓冲区** 实现高效更新

## 刷新模式

|   模式    |   耗时    | 显示质量 |   使用场景   |
| :-------: | :-------: | :------: | :----------: |
| INIT (0)  | 2000 毫秒 | 全屏擦除 |    初始化    |
|  DU (1)   | 260 毫秒  | 快速单色 |   触摸响应   |
| GC16 (2)  | 450 毫秒  |  高质量  |   图片显示   |
| GL16 (3)  | 450 毫秒  | 文字优化 |   文本显示   |
| GLR16 (4) | 450 毫秒  | 减少残影 | 混合内容显示 |
| GLD16 (5) | 450 毫秒  | 轻量刷新 |  高频次更新  |
|  DU4 (6)  | 120 毫秒  | 4 级灰度 |   快速更新   |
|  A2 (7)   | 290 毫秒  |  抗锯齿  |   动画效果   |

## 安装方法

推荐使用 GitHub Actions workflows 进行云端编译，高效且稳定。

① 启用工作流：进入仓库页面，点击上方「Actions」选项卡，启用工作流功能；

② 编写workflows配置文件；

③ 配置 WIFI 加密参数：进入仓库「Settings > Actions」，添加两个仓库密钥（WIFI_SSID、WIFI_PASSWORD），编译时将自动注入 WIFI 信息；

④ 触发编译：选择「Build」工作流，点击「Run workflow」手动触发编译；

⑤ 下载固件：编译完成后，在工作流运行结果的「Artifacts」板块，下载生成的固件压缩包。

## 烧录固件

测试阶段推荐使用ESPConnect在线工具烧录，支持查看串口日志，便于调试：

① 用 USB 数据线将 M5Paper 开发板连接至电脑；

② 访问ESPConnect在线工具：https://thelastoutpostworkshop.github.io/ESPConnect/

③ 点击「连接」，在设备列表中选择对应的设备；

④ 左侧选择「闪存工具」，上传编译好的固件，勾选「写入前擦除整个闪存」；

⑤ 点击「烧录固件」，等待烧录完成。

## 配置说明

```yaml
display:
  - platform: m5paper_display
    id: m5paper_screen

    # SPI 配置
    cs_pin: GPIO15
    busy_pin: GPIO27
    reset_pin: GPIO4
    mosi_pin: GPIO12
    miso_pin: GPIO13
    sck_pin: GPIO14
    spi_host: vspi
    spi_frequency: 10MHz

    # 显示屏配置
    rotation: 0              # 可选值：0、90、180、270
    update_mode: GC16          # 参考刷新模式表格
    inverted: false
    clear_on_startup: true
    deep_sleep_enabled: false

    # 更新间隔
    update_interval: 10s

    # 显示内容（Lambda 表达式）
    lambda: |-
      it.fill(COLOR_OFF);
      it.print(0, 0, id(my_font), "你好 M5Paper!");
```

## 使用示例

### 基础显示

```yaml
display:
  - platform: m5paper_display
    id: my_display
    lambda: |-
      it.fill(COLOR_OFF);
      it.print(0, 0, id(my_font), "Hello World!");
```

### 天气显示

```yaml
display:
  - platform: m5paper_display
    id: weather_display
    update_interval: 5min
    lambda: |-
      // 背景填充
      it.fill(COLOR_OFF);

      // 城市名称
      it.print(0, 20, id(title_font), TextAlign::TOP_CENTER,
               "北京");

      // 温度
      it.printf(0, 100, id(temp_font), TextAlign::TOP_CENTER,
                "%.1f°C", id(outdoor_temp).state);

      // 天气图标（需自定义实现）
      // draw_weather_icon(it, id(weather_icon), 0, 200);

      // 湿度
      it.printf(0, 400, id(body_font), TextAlign::TOP_CENTER,
                "湿度: %.0f%%", id(humidity).state);
```

### 时钟显示

```yaml
display:
  - platform: m5paper_display
    id: clock_display
    update_interval: 60s
    lambda: |-
      it.fill(COLOR_OFF);

      // 时间
      it.strftime(0, 100, id(clock_font), TextAlign::TOP_CENTER,
                   "%H:%M", id(sntp_time).now());

      // 日期
      it.strftime(0, 300, id(date_font), TextAlign::TOP_CENTER,
                   "%Y-%m-%d", id(sntp_time).now());
```

### 传感器仪表盘

```yaml
display:
  - platform: m5paper_display
    id: sensor_dashboard
    update_interval: 30s
    rotation: 90
    lambda: |-
      int y = 20;
      int x = 20;
      int spacing = 100;

      it.fill(COLOR_OFF);

      // 传感器读数
      it.printf(x, y, id(sensor_font), "温度: %.1f°C",
                 id(temp_sensor).state);
      y += spacing;

      it.printf(x, y, id(sensor_font), "湿度: %.0f%%",
                 id(humidity_sensor).state);
      y += spacing;

      it.printf(x, y, id(sensor_font), "气压: %.0f 百帕",
                 id(pressure_sensor).state);
```

## API 参考

### 配置项

|        配置项        |  类型  |  默认值  |             描述              |
| :------------------: | :----: | :------: | :---------------------------: |
|       `cs_pin`       |  整数  |    15    |        片选 GPIO 引脚         |
|      `busy_pin`      |  整数  |    27    |     忙状态信号 GPIO 引脚      |
|     `reset_pin`      |  整数  |    -1    | 复位 GPIO 引脚（-1 表示跳过） |
|      `mosi_pin`      |  整数  |    12    |        MOSI GPIO 引脚         |
|      `miso_pin`      |  整数  |    13    |        MISO GPIO 引脚         |
|      `sck_pin`       |  整数  |    14    |        时钟 GPIO 引脚         |
|      `spi_host`      | 字符串 |  "vspi"  |     SPI 主机（vspi/hspi）     |
|   `spi_frequency`    |  整数  | 10000000 |    SPI 频率（单位：赫兹）     |
|      `rotation`      |  整数  |    0     |     显示屏旋转角度（0-3）     |
|    `update_mode`     |  整数  |    2     |        刷新模式（0-8）        |
|      `inverted`      | 布尔值 |  false   |           颜色反转            |
|  `clear_on_startup`  | 布尔值 |   true   |       启动时清空显示屏        |
| `deep_sleep_enabled` | 布尔值 |  false   |         启用深度睡眠          |
|  `update_interval`   |  时间  |   1 秒   |           更新间隔            |

### 显示屏 Lambda 接口

标准 ESPHome 显示屏 Lambda 函数均支持：

- `it.fill(color)` - 用指定颜色填充整个屏幕
- `it.print(x, y, font, align, "text")` - 打印文字
- `it.printf(x, y, font, align, format, ...)` - 打印格式化文本（支持变量）
- `it.rectangle(x, y, w, h)` - 绘制空心矩形
- `it.filled_rectangle(x, y, w, h)` - 绘制实心矩形
- `it.circle(x, y, radius)` - 绘制空心圆形
- `it.filled_circle(x, y, radius)` - 绘制实心圆形
- `it.line(x1, y1, x2, y2)` - 绘制直线
- `it.get_width()` - 获取屏幕宽度
- `it.get_height()` - 获取屏幕高度

### 组件方法

```c++
// 获取底层 IT8951 驱动
IT8951Driver& driver = display.get_driver();

// 强制更新显示屏
display.update_display();

// 清空显示缓冲区
display.clear_display();

// 进入深度睡眠模式
display.deep_sleep();

// 从睡眠中唤醒
display.wake_from_sleep();

// 获取更新次数
uint32_t count = display.get_update_count();
```

## 故障排除

### 显示屏显示乱码或颜色错误

1. 检查 SPI 引脚连接
2. 确认 CS、MOSI、MISO、SCK 引脚配置正确
3. 尝试降低 SPI 频率：`spi_frequency: 5MHz`

### 残影 / 图像残留

1. 使用 GC16 模式进行全屏刷新
2. 增大 `update_interval` 数值
3. 定期执行全屏刷新

### 显示屏不更新

1. 检查 BUSY 引脚连接
2. 确保 `update_interval` 数值不设置过短
3. 查看 ESPHome 日志排查错误

### 刷新速度慢

1. 使用 DU、A2 或 DU4 模式实现更快更新
2. 这些模式显示质量较低，但刷新速度更快
