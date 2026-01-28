/**
* @file dev_w35t51nw.h
 * @brief 华邦512Mb(64MB) HyperFlash W35T51NWTBIE 驱动头文件
 * @author HuaCheng Ma
 * @date 2026/1/27
 * @version 0.1
 */

#ifndef DEV_W35T51NW_H
#define DEV_W35T51NW_H
#include <stdint.h>

/** @brief Flash 总大小（字节）: 64MB */
#define W35T31NW_FLASH_SIZE     (64 * 1024 * 1024 * 1)   //64MB
/** @brief 块大小（字节）: 32KB */
#define W35T31NW_BLOCK_SIZE     (32 * 1024 * 1) //32KB
/** @brief 扇区大小（字节）: 4KB */
#define W35T31NW_SECTOR_SIZE    (4096 * 1) //4KB
/** @brief 页大小（字节）: 256B */
#define W35T31NW_PAGE_SIZE      (256 * 1) //256B


/**
 * @brief 从 W35T51NW 读取数据
 * @param[out] pdata 目标缓冲区指针，调用者需保证其至少可容纳 \p _sz 字节
 * @param[in] _addr 从 Flash 中读取的起始地址（字节偏移）
 * @param[in] _size 要读取的字节数
 * @return 0 表示成功，非 0 表示失败（具体错误码由实现定义）
 */
int W35T51NW_Read(uint8_t *pdata, uint32_t _addr, uint32_t _size);

/**
 * @brief 向 W35T51NW 写入数据
 * @param[in] pData 指向包含要写入数据的缓冲区
 * @param[in] WriteAddr 写入起始地址（字节偏移）
 * @param[in] Size 要写入的字节数
 * @return 0 表示成功，非 0 表示失败（具体错误码由实现定义）
 */
int W35T31NW_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);

/**
 * @brief 将设备置于 STR（Single Transfer Rate） 内存映射模式
 * @note 进入此模式后通常可以通过内存访问直接读取 Flash 内容
 * @return 0 表示成功，非 0 表示失败
 */
int W35T51NW_STRMemoryMappedMode(void);

/**
 * @brief 读取设备配置寄存器
 * @param[out] cfgreg 指向用于存放配置寄存器值的缓冲区（字节数由实现约定）
 * @return 0 表示成功，非 0 表示失败
 */
int W35T51NW_ReadCfgReg(uint8_t *cfgreg);

/**
 * @brief 擦除整个芯片
 * @return 0 表示成功，非 0 表示失败
 */
int W35T51NW_EraseChip(void);

/**
 * @brief 擦除 32KB 大小的块
 * @param[in] _addr 要擦除的块所在地址（应为块对齐地址）
 * @return 0 表示成功，非 0 表示失败
 */
int W35T51NW_EraseBlock32K(uint32_t _addr);

/**
 * @brief 复位 W35T51NW 设备
 * @note 通常用于将设备恢复到默认状态
 */
void W35T51NW_Reset(void);

/**
 * @brief 读取设备 ID
 * @param[out] id 指向用于接收设备 ID 的缓冲区（通常为若干字节）
 * @return 无返回值；调用者通过 \p id 获取 ID 内容
 */
void W35T51NW_ReadID(uint8_t *id);

#endif //DEV_W35T51NW_H
