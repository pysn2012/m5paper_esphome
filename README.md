# ESPHome 适配 M5Paper 显示组件

适用于搭载 IT8951E 控制器的 M5Stack Paper V1.1 电子纸显示屏的 ESPHome 外部组件。

## 硬件参数

| 规格项 | Value |
|--------------|-------|
| 产品型号 | M5Stack Paper V1.1 |
| SKU | K049-B |
| Display | 4.7" E-Ink (ED047TC1) |
| Resolution | 540 x 960 pixels |
| Controller | IT8951E |
| 灰度等级 | 16 levels (4-bit) |
| SoC | ESP32-D0WDQ6-V3 |
| Flash | 16MB |
| PSRAM | 8MB |

## 引脚映射 (M5Stack Paper V1.1)

| Signal | GPIO | Description |
|--------|------|-------------|
| MOSI | GPIO12 | SPI Data Out |
| MISO | GPIO13 | SPI Data In |
| SCK | GPIO14 | SPI Clock |
| CS | GPIO15 | Chip Select |
| BUSY | GPIO27 | IT8951E Busy |
| RST | GPIO4 | Reset (optional) |

## 功能特性

- **16-level grayscale** (4-bit per pixel)
- **Multiple refresh modes** for quality/speed tradeoff
- **Rotation support** (0, 90, 180, 270 degrees)
- **Deep sleep** power saving
- **Frame buffer** for efficient updates

## 刷新模式

| Mode | Time | Quality | Use Case |
|------|------|---------|----------|
| INIT (0) | 2000ms | Full erase | Initialization |
| DU (1) | 260ms | Fast mono | Touch response |
| GC16 (2) | 450ms | High quality | Images |
| GL16 (3) | 450ms | Text optimized | Text display |
| GLR16 (4) | 450ms | Reduced ghosting | Mixed content |
| GLD16 (5) | 450ms | Light refresh | Frequent updates |
| DU4 (6) | 120ms | 4-level gray | Fast updates |
| A2 (7) | 290ms | Anti-aliased | Animations |

## Installation

### Option 1: Local Path

Copy the component folder to your ESPHome configuration directory:

```yaml
external_components:
  - source:
      path: ./m5paper_display/esphome
    components: [m5paper_display]
```

### Option 2: Git Repository

```yaml
external_components:
  - source:
      url: https://github.com/your-repo/m5paper-esphome
      ref: main
    components: [m5paper_display]
```

## Configuration

```yaml
display:
  - platform: m5paper_display
    id: m5paper_screen

    # SPI Configuration
    cs_pin: GPIO15
    busy_pin: GPIO27
    reset_pin: GPIO4
    mosi_pin: GPIO12
    miso_pin: GPIO13
    sck_pin: GPIO14
    spi_host: vspi
    spi_frequency: 10MHz

    # Display Configuration
    rotation: 0              # 0, 90, 180, 270
    update_mode: GC16          # See refresh modes table
    inverted: false
    clear_on_startup: true
    deep_sleep_enabled: false

    # Update Interval
    update_interval: 10s

    # Display Content (lambda)
    lambda: |-
      it.fill(COLOR_OFF);
      it.print(0, 0, id(my_font), "Hello M5Paper!");
```

## Usage Examples

### Basic Display

```yaml
display:
  - platform: m5paper_display
    id: my_display
    lambda: |-
      it.fill(COLOR_OFF);
      it.print(0, 0, id(my_font), "Hello World!");
```

### Weather Display

```yaml
display:
  - platform: m5paper_display
    id: weather_display
    update_interval: 5min
    lambda: |-
      // Background
      it.fill(COLOR_OFF);

      // City name
      it.print(0, 20, id(title_font), TextAlign::TOP_CENTER,
               "Beijing");

      // Temperature
      it.printf(0, 100, id(temp_font), TextAlign::TOP_CENTER,
                "%.1f°C", id(outdoor_temp).state);

      // Weather icon (custom implementation needed)
      // draw_weather_icon(it, id(weather_icon), 0, 200);

      // Humidity
      it.printf(0, 400, id(body_font), TextAlign::TOP_CENTER,
                "Humidity: %.0f%%", id(humidity).state);
```

### Clock Display

```yaml
display:
  - platform: m5paper_display
    id: clock_display
    update_interval: 60s
    lambda: |-
      it.fill(COLOR_OFF);

      // Time
      it.strftime(0, 100, id(clock_font), TextAlign::TOP_CENTER,
                   "%H:%M", id(sntp_time).now());

      // Date
      it.strftime(0, 300, id(date_font), TextAlign::TOP_CENTER,
                   "%Y-%m-%d", id(sntp_time).now());
```

### Sensor Dashboard

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

      // Sensor readings
      it.printf(x, y, id(sensor_font), "Temperature: %.1f°C",
                 id(temp_sensor).state);
      y += spacing;

      it.printf(x, y, id(sensor_font), "Humidity: %.0f%%",
                 id(humidity_sensor).state);
      y += spacing;

      it.printf(x, y, id(sensor_font), "Pressure: %.0f hPa",
                 id(pressure_sensor).state);
```

## API Reference

### Configuration Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `cs_pin` | int | 15 | Chip Select GPIO |
| `busy_pin` | int | 27 | Busy signal GPIO |
| `reset_pin` | int | -1 | Reset GPIO (-1 = skip) |
| `mosi_pin` | int | 12 | MOSI GPIO |
| `miso_pin` | int | 13 | MISO GPIO |
| `sck_pin` | int | 14 | Clock GPIO |
| `spi_host` | string | "vspi" | SPI host (vspi/hspi) |
| `spi_frequency` | int | 10000000 | SPI frequency in Hz |
| `rotation` | int | 0 | Display rotation (0-3) |
| `update_mode` | int | 2 | Refresh mode (0-8) |
| `inverted` | bool | false | Invert colors |
| `clear_on_startup` | bool | true | Clear display on boot |
| `deep_sleep_enabled` | bool | false | Enable deep sleep |
| `update_interval` | time | 1s | Update interval |

### Display Lambda API

Standard ESPHome display lambda functions are supported:

- `it.fill(color)` - Fill display with color
- `it.print(x, y, font, align, "text")` - Print text
- `it.printf(x, y, font, align, format, ...)` - Print formatted text
- `it.rectangle(x, y, w, h)` - Draw rectangle outline
- `it.filled_rectangle(x, y, w, h)` - Draw filled rectangle
- `it.circle(x, y, radius)` - Draw circle outline
- `it.filled_circle(x, y, radius)` - Draw filled circle
- `it.line(x1, y1, x2, y2)` - Draw line
- `it.get_width()` - Get display width
- `it.get_height()` - Get display height

### Component Methods

```cpp
// Get the underlying IT8951 driver
IT8951Driver& driver = display.get_driver();

// Force a display update
display.update_display();

// Clear the display buffer
display.clear_display();

// Enter deep sleep mode
display.deep_sleep();

// Wake from deep sleep
display.wake_from_sleep();

// Get update count
uint32_t count = display.get_update_count();
```

## Troubleshooting

### Display shows garbage or wrong colors

1. Check SPI pin connections
2. Verify CS, MOSI, MISO, SCK are correct
3. Try lowering SPI frequency: `spi_frequency: 5MHz`

### Ghosting / Image retention

1. Use GC16 mode for full refresh
2. Increase `update_interval`
3. Run full refresh periodically

### Display not updating

1. Check BUSY pin connection
2. Ensure `update_interval` is not too short
3. Check ESPHome logs for errors

### Slow refresh

1. Use DU, A2, or DU4 mode for faster updates
2. These modes have lower quality but faster speed

## License

MIT License

## Credits

- IT8951E driver based on [M5Stack M5EPD](https://github.com/m5stack/M5EPD)
- ESPHome integration inspired by [ESPHome display components](https://esphome.io/components/display/)
