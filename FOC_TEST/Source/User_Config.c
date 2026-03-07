
#include "User_Config.h"


tUsrConfig   UsrConfig;
tCoggingMap *pCoggingMap = NULL;
static uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}
void USR_set_default_config(void){
    // Motor
    UsrConfig.invert_motor_dir       = 0;
    UsrConfig.motor_pole_pairs       = 7;
    UsrConfig.motor_phase_resistance = 0.028f;
    UsrConfig.motor_phase_inductance = 110e-6f;
    UsrConfig.current_limit          = 5;
    UsrConfig.velocity_limit         = 20;

    // Encoder
    UsrConfig.calib_current = 0.5f;
    UsrConfig.calib_voltage = 5.0f;

    // Controller
    UsrConfig.pos_p_gain             = 80.0f;
    UsrConfig.vel_p_gain             = 0.3f;
    UsrConfig.vel_i_gain             = 5.0f;
    UsrConfig.current_ff_gain        = 0.001f;
    UsrConfig.current_ctrl_bw        = 1000;
    UsrConfig.default_op_mode        = CONTROL_MODE_POSITION_PROFILE;
    UsrConfig.anticogging_enable     = 1;
    UsrConfig.sync_target_enable     = 0;
    UsrConfig.target_velcity_window  = 0.5f;
    UsrConfig.target_position_window = 0.01f;
    UsrConfig.current_ramp_rate      = 0.5f;
    UsrConfig.velocity_ramp_rate     = 10;
    UsrConfig.position_filter_bw     = 10;
    UsrConfig.profile_velocity       = 50;
    UsrConfig.profile_accel          = 50;
    UsrConfig.profile_decel          = 50;

    // Encoder
    UsrConfig.calib_valid = 0;
}
void USR_set_default_cogging_map(void)
{
    if (pCoggingMap == NULL) {
        pCoggingMap = HEAP_malloc(sizeof(tCoggingMap));
    }

    for (int i = 0; i < COGGING_MAP_NUM; i++) {
        pCoggingMap->map[i] = 0;
    }
}
   __IO uint32_t crc;
int USR_read_config(void)
{
    int state = 0;

    memcpy(&UsrConfig, (uint8_t *) USR_CONFIG_ADDR, sizeof(tUsrConfig));

    crc = crc32((uint8_t *) &UsrConfig, sizeof(tUsrConfig) - 4);
    if (crc != UsrConfig.crc) {
        state = -1;
    }

    return state;
}
int USR_read_cogging_map(void)
{
    int state = 0;

    if (pCoggingMap == NULL) {
        pCoggingMap = HEAP_malloc(sizeof(tCoggingMap));
    }

    memcpy(pCoggingMap, (uint8_t *) COGGING_MAP_ADDR, sizeof(tCoggingMap));

    uint32_t crc;
    crc = crc32((uint8_t *) pCoggingMap, sizeof(tCoggingMap) - 4);
    if (crc != pCoggingMap->crc) {
        state = -1;
    }

    return state;
}
int USR_erease_config(void)
{
    uint32_t addr;
    HAL_StatusTypeDef hal_status;
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    int32_t FirstPage = 0, NbOfPages = 0;
    FirstPage = GetPage(USR_CONFIG_ADDR);

  /* Get the number of pages to erase from 1st page */
    NbOfPages = GetPage(USR_CONFIG_ADDR+USR_CONFIG_MAX_SIZE-1) - FirstPage + 1;

  /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;
    EraseInitStruct.Banks       = FLASH_BANK_1;

    HAL_FLASH_Unlock();
    hal_status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
    if (hal_status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();

    for (addr = USR_CONFIG_ADDR; addr < (USR_CONFIG_ADDR + USR_CONFIG_MAX_SIZE); addr += 4)
    {
        if (*((uint32_t *)addr) != 0xFFFFFFFF)
        {
            return -2;
        }
    }

    return 0;
}
int USR_erease_cogging_map(void)
{
    uint32_t addr;
    HAL_StatusTypeDef hal_status;
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    HAL_FLASH_Unlock();

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;  
    EraseInitStruct.Page = (COGGING_MAP_ADDR - FLASH_BASE) / PAGE_SIZE; 
    EraseInitStruct.NbPages = (COGGING_MAP_MAX_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;
    EraseInitStruct.Banks = FLASH_BANK_1;

    hal_status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
    if (hal_status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();

    for (addr = COGGING_MAP_ADDR; addr < (COGGING_MAP_ADDR + COGGING_MAP_MAX_SIZE); addr += 4)
    {
        if (*((uint32_t *)addr) != 0xFFFFFFFF)
        {
            return -2;
        }
    }

    return 0;
}
int USR_save_config(void)
{
    if(USR_erease_config()){
        return -1;
    }
    HAL_StatusTypeDef hal_status;
    uint32_t addr;
    uint8_t *pData;
    uint32_t dataLen;

    UsrConfig.crc = crc32((uint8_t *) &UsrConfig, sizeof(tUsrConfig) - 4);

    HAL_FLASH_Unlock();
    addr = USR_CONFIG_ADDR;
    pData = (uint8_t *) &UsrConfig;
    dataLen = sizeof(tUsrConfig);

    for (uint32_t i = 0; i < dataLen; i += 8)
    {
        uint64_t data = *((uint64_t *)(pData + i));
        hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, data);
        if (hal_status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return -1;
        }
    }

    HAL_FLASH_Lock();

    return 0;
}
int USR_save_cogging_map(void)
{
    if(USR_erease_cogging_map()){
        return -1;
    }
    HAL_StatusTypeDef hal_status;
    uint32_t addr;
    uint8_t *pData;
    uint32_t dataLen;

    pCoggingMap->crc = crc32((uint8_t *) pCoggingMap, sizeof(tCoggingMap) - 4);

    HAL_FLASH_Unlock();

    addr = COGGING_MAP_ADDR;
    pData = (uint8_t *) pCoggingMap;
    dataLen = sizeof(tCoggingMap);

    for (uint32_t i = 0; i < dataLen; i += 8)
    {
        uint64_t data = *((uint64_t *)(pData + i));
        hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, data);
        if (hal_status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return -1;
        }
    }

    HAL_FLASH_Lock();

    return 0;
}