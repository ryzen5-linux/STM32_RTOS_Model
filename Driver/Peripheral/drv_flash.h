/**
  ******************************************************************************
  * @file    drv_flash.h
  * @brief   内部Flash编程驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 内部Flash解锁/上锁
  *   - 页擦除（STM32F103C8T6: 64 pages * 1KB = 64KB）
  *   - 半字（16位）和字（32位）编程
  *   - Flash读操作（直接地址访问）
  *   - 操作状态查询
  *
  * STM32F103C8T6 Flash布局:
  *   起始地址：0x08000000
  *   页大小：  1024字节（1KB）
  *   页数量：  64页（中密度器件）
  *   总容量：  64KB
  *
  * 安全提示:
  *   - 编程/擦除操作期间不可从Flash读取代码（避免将代码与数据放同一页）
  *   - 建议在低频率下操作（HAL层建议关闭优化）
  *   - 每次编程前对应位置必须先擦除（0xFFFF状态）
  *
  * 使用示例:
  *   DRV_FLASH_Unlock();
  *   DRV_FLASH_ErasePage(0x0800F800);  // 擦除第63页（最后几页用于参数存储）
  *   DRV_FLASH_WriteHalfWord(0x0800F800, 0x1234);
  *   DRV_FLASH_Lock();
  *   uint16_t val = DRV_FLASH_ReadHalfWord(0x0800F800);
  ******************************************************************************
  */

#ifndef __DRV_FLASH_H
#define __DRV_FLASH_H

#include "stm32f10x.h"
#include "stm32f10x_flash.h"

/*===========================================================================
 * Flash 布局常量
 *===========================================================================*/
#define DRV_FLASH_BASE_ADDR    (0x08000000U)  /**< Flash起始地址 */
#define DRV_FLASH_PAGE_SIZE    (0x400U)       /**< 页大小：1KB */
#define DRV_FLASH_PAGE_COUNT   (64U)          /**< 总页数（C8T6 64KB） */
#define DRV_FLASH_TOTAL_SIZE   (DRV_FLASH_PAGE_SIZE * DRV_FLASH_PAGE_COUNT)

/** 根据页号计算页起始地址 */
#define DRV_FLASH_PAGE_ADDR(n) (DRV_FLASH_BASE_ADDR + (n) * DRV_FLASH_PAGE_SIZE)

/*===========================================================================
 * 返回状态码
 *===========================================================================*/
#define DRV_FLASH_OK       (0)
#define DRV_FLASH_ERROR    (-1)
#define DRV_FLASH_TIMEOUT  (-2)
#define DRV_FLASH_BUSY     (-3)

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  解锁Flash写保护（编程/擦除前必须调用）
 */
void DRV_FLASH_Unlock(void);

/**
 * @brief  上锁Flash写保护（操作完成后调用）
 */
void DRV_FLASH_Lock(void);

/**
 * @brief  擦除指定地址所在的Flash页（1KB）
 * @param  pageAddr  页内任意地址（将自动对齐至页起始）
 * @retval DRV_FLASH_OK / DRV_FLASH_ERROR / DRV_FLASH_TIMEOUT
 */
int DRV_FLASH_ErasePage(uint32_t pageAddr);

/**
 * @brief  写入16位半字（编程前该地址必须已擦除为0xFFFF）
 * @param  addr   目标地址（必须2字节对齐）
 * @param  value  16位数据
 * @retval DRV_FLASH_OK / DRV_FLASH_ERROR
 */
int DRV_FLASH_WriteHalfWord(uint32_t addr, uint16_t value);

/**
 * @brief  写入32位字（内部分两次半字编程）
 * @param  addr   目标地址（必须4字节对齐）
 * @param  value  32位数据
 * @retval DRV_FLASH_OK / DRV_FLASH_ERROR
 */
int DRV_FLASH_WriteWord(uint32_t addr, uint32_t value);

/**
 * @brief  向指定地址连续写入缓冲数据（半字数组形式）
 * @param  addr   起始地址（必须2字节对齐）
 * @param  buf    16位数据数组指针
 * @param  count  半字数量
 * @retval DRV_FLASH_OK / DRV_FLASH_ERROR
 */
int DRV_FLASH_WriteBuffer(uint32_t addr, const uint16_t *buf, uint32_t count);

/**
 * @brief  读取16位半字
 * @param  addr  源地址（2字节对齐）
 * @retval 读取的16位值
 */
uint16_t DRV_FLASH_ReadHalfWord(uint32_t addr);

/**
 * @brief  读取32位字
 * @param  addr  源地址（4字节对齐）
 * @retval 读取的32位值
 */
uint32_t DRV_FLASH_ReadWord(uint32_t addr);

/**
 * @brief  读取缓冲数据（字节形式）
 * @param  addr  源地址
 * @param  buf   目标缓冲指针
 * @param  len   字节数
 */
void DRV_FLASH_ReadBuffer(uint32_t addr, uint8_t *buf, uint32_t len);

/**
 * @brief  获取Flash操作状态
 * @retval FLASH_COMPLETE / FLASH_ERROR_PG / FLASH_ERROR_WRP / FLASH_TIMEOUT / FLASH_BUSY
 */
FLASH_Status DRV_FLASH_GetStatus(void);

#endif /* __DRV_FLASH_H */
