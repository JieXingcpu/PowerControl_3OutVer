#ifndef INA3221_REGISTER_H
#define INA3221_REGISTER_H

// INA2261寄存器地址定义
// clang-format off
#define I2C_ADDRESS                0x80   // 设备I2C地址
#define CONFIG_REGISTER_ADDR       0x00   // 配置寄存器
#define SHUNT_VOLTAGE_CHANNEL_1    0x01   // 通道1分流电压寄存器
#define BUS_VOLTAGE_CHANNEL_1      0x02   // 通道1总线电压寄存器
#define SHUNT_VOLTAGE_CHANNEL_2    0x03   // 通道2分流电压寄存器
#define BUS_VOLTAGE_CHANNEL_2      0x04   // 通道2总线电压寄存器
#define SHUNT_VOLTAGE_CHANNEL_3    0x05   // 通道3分流电压寄存器
#define BUS_VOLTAGE_CHANNEL_3      0x06   // 通道3总线电压寄存器
#define CRITICAL_ALERT_CHANNEL_1   0x07   // 通道1临界警报限值寄存器
#define WARNING_ALERT_CHANNEL_1    0x08   // 通道1警告警报限值寄存器
#define CRITICAL_ALERT_CHANNEL_2   0x09   // 通道2临界警报限值寄存器
#define WARNING_ALERT_CHANNEL_2    0x0A   // 通道2警告警报限值寄存器
#define CRITICAL_ALERT_CHANNEL_3   0x0B   // 通道3临界警报限值寄存器
#define WARNING_ALERT_CHANNEL_3    0x0C   // 通道3警告警报限值寄存器
#define SHUNT_VOLTAGE_SUM          0x0D   // 分流电压总和寄存器
#define SHUNT_VOLTAGE_SUM_LIMIT    0x0E   // 分流电压总和限值寄存器
#define MASK_ENABLE_REGISTER       0x0F   // 屏蔽/使能寄存器
#define POWER_VALID_UPPER_LIMIT    0x10   // 电源有效上限寄存器
#define POWER_VALID_LOWER_LIMIT    0x11   // 电源有效下限寄存器
#define MANUFACTURER_ID_REGISTER   0xFE   // 制造商ID寄存器
#define CHIP_ID_REGISTER           0xFF   // 芯片ID寄存器


#endif