#pragma once

/*-----------------------------------------------------------------------
IT8951 命令定义
------------------------------------------------------------------------*/

// 数据包类型
#define IT8951_PACKET_TYPE_CMD      0x6000
#define IT8951_PACKET_TYPE_WRITE    0x0000
#define IT8951_PACKET_TYPE_READ     0x1000

// 内置I80接口命令码
#define IT8951_TCON_SYS_RUN         0x0001
#define IT8951_TCON_STANDBY         0x0002
#define IT8951_TCON_SLEEP           0x0003
#define IT8951_TCON_REG_RD          0x0010
#define IT8951_TCON_REG_WR          0x0011

#define IT8951_TCON_MEM_BST_RD_T    0x0012
#define IT8951_TCON_MEM_BST_RD_S    0x0013
#define IT8951_TCON_MEM_BST_WR      0x0014
#define IT8951_TCON_MEM_BST_END     0x0015

#define IT8951_TCON_LD_IMG          0x0020
#define IT8951_TCON_LD_IMG_AREA     0x0021
#define IT8951_TCON_LD_IMG_END      0x0022

// I80接口用户自定义命令码
#define IT8951_I80_CMD_DPY_AREA     0x0034
#define IT8951_I80_CMD_GET_DEV_INFO 0x0302
#define IT8951_I80_CMD_DPY_BUF_AREA 0x0037
#define IT8951_I80_CMD_VCOM         0x0039
#define IT8951_I80_CMD_VCOM_READ    0x0000
#define IT8951_I80_CMD_VCOM_WRITE   0x0001

/*-----------------------------------------------------------------------
 IT8951 模式定义
------------------------------------------------------------------------*/
// 像素模式（每像素比特数）
#define IT8951_2BPP             0
#define IT8951_3BPP             1
#define IT8951_4BPP             2
#define IT8951_8BPP             3

// 字节序类型
#define IT8951_LDIMG_L_ENDIAN   0
#define IT8951_LDIMG_B_ENDIAN   1

// 默认VCOM电压值（单位：毫伏）
#define IT8951_DEFAULT_VCOM     2300

/*-----------------------------------------------------------------------
IT8951 寄存器定义
------------------------------------------------------------------------*/
// 寄存器基地址
#define IT8951_DISPLAY_REG_BASE     0x1000 // 显示控制器寄存器读写基地址

// 基础LUT寄存器基地址
#define IT8951_LUT0EWHR    (IT8951_DISPLAY_REG_BASE + 0x00) //LUT0 引擎宽高寄存器
#define IT8951_LUT0XYR     (IT8951_DISPLAY_REG_BASE + 0x40) //LUT0 坐标寄存器
#define IT8951_LUT0BADDR   (IT8951_DISPLAY_REG_BASE + 0x80) //LUT0 基地址寄存器
#define IT8951_LUT0MFN     (IT8951_DISPLAY_REG_BASE + 0xC0) //LUT0 模式与帧数寄存器
#define IT8951_LUT01AF     (IT8951_DISPLAY_REG_BASE + 0x114) //LUT0和LUT1激活标志寄存器

// 更新参数设置寄存器
#define IT8951_UP0SR       (IT8951_DISPLAY_REG_BASE + 0x134) // 更新参数0寄存器
#define IT8951_UP1SR       (IT8951_DISPLAY_REG_BASE + 0x138) // 更新参数1寄存器
#define IT8951_LUT0ABFRV   (IT8951_DISPLAY_REG_BASE + 0x13C) // LUT0混合与填充值寄存器
#define IT8951_UPBBADDR    (IT8951_DISPLAY_REG_BASE + 0x17C) // 更新缓冲区基地址寄存器
#define IT8951_LUT0IMXY    (IT8951_DISPLAY_REG_BASE + 0x180) //LUT0图像缓冲区偏移寄存器
#define IT8951_LUTAFSR     (IT8951_DISPLAY_REG_BASE + 0x224) //LUT引擎状态寄存器
#define IT8951_BGVR        (IT8951_DISPLAY_REG_BASE + 0x250) //1比特位图颜色表寄存器

// 系统寄存器
#define IT8951_SYS_REG_BASE         0x0000

// 系统寄存器地址
#define IT8951_I80CPCR              (IT8951_SYS_REG_BASE + 0x04) // I80接口控制寄存器

// 内存转换控制器寄存器
#define IT8951_MCSR_BASE_ADDR       0x0200
#define IT8951_MCSR                 (IT8951_MCSR_BASE_ADDR + 0x0000) // 内存转换控制状态寄存器
#define IT8951_LISAR                (IT8951_MCSR_BASE_ADDR + 0x0008) // 加载图像起始地址寄存器
