/**
*   @file dev_w35t51nw.c
*   @brief 华邦512Mb(64MB) HyperFlash W35T51NWTBIE 驱动源文件
*   @author HuaCheng Ma
*   @date 2026/1/27
*   @version 0.1
*   @note
*/
#include "dev_w35t51nw.h"
#include "xspi.h"

void W35T51NW_ReadID(uint8_t *id)
{
    XSPI_RegularCmdTypeDef sCommand = {};
    memset(&sCommand, 0, sizeof(sCommand));
    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x9e;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.DataLength         = 6;
    sCommand.DummyCycles        = 0;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return;
    }

    if (HAL_XSPI_Receive(&hxspi2, id, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return;
    }
}

void W35T51NW_Reset(void)
{
    XSPI_RegularCmdTypeDef sCommand = {0};

    memset(&sCommand, 0, sizeof(sCommand));

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x66;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_NONE;
    sCommand.DummyCycles        = 0;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return;
    }

    sCommand.Instruction = 0x99;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return;
    }
}

static int W35T51NW_AutoPollingMemReady(uint32_t timeout)
{
    XSPI_RegularCmdTypeDef sCommand = {0};
    XSPI_AutoPollingTypeDef sCfg = {0};

    memset(&sCommand, 0, sizeof(sCommand));
    memset(&sCfg, 0, sizeof(sCfg));

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x05;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.DummyCycles        = 0;
    sCommand.DataLength         = 1;

    sCfg.MatchValue             = 0x00;
    sCfg.MatchMask              = 0x01;
    sCfg.MatchMode              = HAL_XSPI_MATCH_MODE_AND;
    sCfg.IntervalTime           = timeout;
    sCfg.AutomaticStop          = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    if (HAL_XSPI_AutoPolling(&hxspi2, &sCfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    return 0;
}

static int W35T51NW_WriteEnable(void)
{
    XSPI_RegularCmdTypeDef sCommand = {0};
    XSPI_AutoPollingTypeDef sCfg = {0};

    memset(&sCommand, 0, sizeof(sCommand));
    memset(&sCfg, 0, sizeof(sCfg));

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x06;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    sCommand.Instruction    = 0x05;
    sCommand.DataMode       = HAL_XSPI_DATA_1_LINE;
    sCommand.DataLength     = 1;

    sCfg.MatchValue         = 0x02;
    sCfg.MatchMask          = 0x02;
    sCfg.MatchMode          = HAL_XSPI_MATCH_MODE_AND;
    sCfg.IntervalTime       = 0x20;
    sCfg.AutomaticStop      = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    if (HAL_XSPI_AutoPolling(&hxspi2, &sCfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    return 0;
}

int W35T51NW_EraseBlock32K(uint32_t _addr)
{
    XSPI_RegularCmdTypeDef sCommand = {0};

    memset(&sCommand, 0, sizeof(sCommand));

    if (W35T51NW_WriteEnable())
    {
        return -1;
    }

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x52;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.Address            = _addr;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    if (W35T51NW_AutoPollingMemReady(100))
    {
        return -1;
    }

    return 0;
}

int W35T51NW_EraseChip(void)
{
    XSPI_RegularCmdTypeDef sCommand = {0};

    memset(&sCommand, 0, sizeof(sCommand));

    if (W35T51NW_WriteEnable())
    {
        return -1;
    }

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x60;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    if (W35T51NW_AutoPollingMemReady(100))
    {
        return -1;
    }

    return 0;
}

static int W35T51NW_PageProgram(uint8_t *pdata, uint32_t _addr, uint32_t _sz)
{
    XSPI_RegularCmdTypeDef sCommand = {0};

    memset(&sCommand, 0, sizeof(sCommand));

    if (W35T51NW_WriteEnable())
    {
        return -1;
    }

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x02;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.Address            = _addr;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DummyCycles        = 0U;
    sCommand.DataLength         = _sz;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    if (HAL_XSPI_Transmit(&hxspi2, pdata, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    if (W35T51NW_AutoPollingMemReady(100))
    {
        return -1;
    }

    return 0;
}

int W35T51NW_ReadCfgReg(uint8_t *cfgreg)
{
    XSPI_RegularCmdTypeDef sCommand = {0};

    memset(&sCommand, 0, sizeof(sCommand));

    sCommand.Instruction        = 0x85;    // 1-1-1
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.Address            = 0x0;
    sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.DataLength         = 1;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DummyCycles        = 8;
    // reg = 0xDF;   // standard SPI
    // reg = 0xE7;   // with in DQS IO Enabled
    // reg = 0xC7;   // with in DQS IO disabled
        
    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return -1;
    }
        
    if (HAL_XSPI_Receive(&hxspi2, cfgreg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return -1;
    }

    return 0;
}

int W35T51NW_STRMemoryMappedMode(void)
{
    XSPI_RegularCmdTypeDef sCommand = {0};
    XSPI_MemoryMappedTypeDef sMapCfg = {0};

    memset(&sCommand, 0, sizeof(sCommand));
    memset(&sMapCfg, 0, sizeof(sMapCfg));

    sCommand.OperationType      = HAL_XSPI_OPTYPE_READ_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x8b;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_8_LINES;
    sCommand.DummyCycles        = 8;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    sCommand.OperationType  = HAL_XSPI_OPTYPE_WRITE_CFG;
    sCommand.Instruction    = 0x82;
    sCommand.DummyCycles    = 0U;
    sCommand.DQSMode        = HAL_XSPI_DQS_DISABLE;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    sMapCfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;

    if (HAL_XSPI_MemoryMapped(&hxspi2, &sMapCfg) != HAL_OK)
    {
        return -1;
    }

    return 0;
}

int W35T51NW_Read(uint8_t *pdata, uint32_t _addr, uint32_t _sz)
{
    XSPI_RegularCmdTypeDef sCommand = {0};

    memset(&sCommand, 0, sizeof(sCommand));
    
    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction        = 0x0b;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
    sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.Address            = _addr;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
    sCommand.DummyCycles        = 8;
    sCommand.DataLength         = _sz;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;

    if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    if (HAL_XSPI_Receive(&hxspi2, pdata, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return -1;
    }

    if (W35T51NW_AutoPollingMemReady(100))
    {
        return -1;
    }

    return 0;
}

int W35T51NW_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  int ret = 0;
  uint32_t end_addr, current_size, current_addr;
  uint8_t *write_data;

  /* Calculation of the size between the write address and the end of the page */
  current_size = W35T31NW_PAGE_SIZE - (WriteAddr % W35T31NW_PAGE_SIZE);

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the address variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;
  write_data = pData;

  /* Perform the write page by page */
  do
  {
    /* Issue page program command */
    if (W35T51NW_PageProgram(write_data, current_addr, current_size))
    {
        ret = -1;
    }
    else
    {
      /* Update the address and size variables for next page programming */
      current_addr += current_size;
      write_data += current_size;
      current_size = ((current_addr + W35T31NW_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W35T31NW_PAGE_SIZE;
    }
  } while ((current_addr < end_addr) && (ret == 0));

  /* Return BSP status */
  return ret;
}
