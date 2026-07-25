/*
 * User_Config.c
 *
 *  Created on: 2026��3��13��
 *      Author: XYY
 */


#include "User_Config.h"
#include "controller.h"

#include "IfxFlash.h"
#include "IfxScuWdt.h"
#include "IfxCpu.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h> // ���� HEAP_malloc ��Ӧ�ı�׼���������ԭ��������

tUsrConfig   UsrConfig;
tCoggingMap *pCoggingMap = NULL;

/* =========================================================================
 * 1. TC364 DFlash Ӳ����׼���� (��� Data Flash 0)
 * ========================================================================= */
#define USR_CONFIG_ADDR       ((uint32_t)0xAF000000)
// ���ڶ������ݿ���� 8KB �����������߽�
#define COGGING_MAP_ADDR      ((uint32_t)0xAF002000) 

#define FLASH_PAGE_SIZE       (32U)       // TC3xx DFlash Ӳ��ǿ��Ҫ�� 32 �ֽڶ���
#define FLASH_SECTOR_SIZE     (0x2000U)   // TC3xx DFlash ��������Ϊ 8KB

#define USR_CONFIG_MAX_SIZE   FLASH_SECTOR_SIZE
#define COGGING_MAP_MAX_SIZE  FLASH_SECTOR_SIZE

/* =========================================================================
 * 2. Ĭ��ֵ�����߼� (����ԭ STM32 �����߼�)
 * ========================================================================= */
void USR_set_default_config(void)
{
    // Motor
    UsrConfig.invert_motor_dir       = 0;
    UsrConfig.motor_pole_pairs       = 7;
    UsrConfig.motor_phase_resistance = 0.028f;
    UsrConfig.motor_phase_inductance = 110e-6f;
    UsrConfig.current_limit          = 1;
    UsrConfig.velocity_limit         = 20;

    // Encoder
    UsrConfig.calib_current = 0.5f;
    UsrConfig.calib_voltage = 5.0f;

    // Controller
    UsrConfig.pos_p_gain             = 80.0f;
    UsrConfig.vel_p_gain             = 0.1f;
    UsrConfig.vel_i_gain             = 3.0f;
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

/* =========================================================================
 * 3. ���ݶ�ȡ���� (ֱ��ͨ���ڴ�ӳ���ȡ)
 * ========================================================================= */
volatile uint32_t config_crc;

int USR_read_config(void)
{
    int state = 0;

    // TC364 �� DFlash ֧��ֱ���ֽ�Ѱַ��ȡ������ԭ memcpy �߼�
    memcpy(&UsrConfig, (uint8_t *)USR_CONFIG_ADDR, sizeof(tUsrConfig));

    config_crc = crc32((uint8_t *)&UsrConfig, sizeof(tUsrConfig) - 4);
    if (config_crc != UsrConfig.crc) {
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

    memcpy(pCoggingMap, (uint8_t *)COGGING_MAP_ADDR, sizeof(tCoggingMap));

    uint32_t local_crc;
    local_crc = crc32((uint8_t *)pCoggingMap, sizeof(tCoggingMap) - 4);
    if (local_crc != pCoggingMap->crc) {
        state = -1;
    }

    return state;
}

/* =========================================================================
 * 4. ���������� 0xFFFFFFFF �հ�У�� (��Ӧԭ STM32 HAL_FLASHEx_Erase)
 * ========================================================================= */
int USR_erease_config(void)
{
    uint16_t password;
    uint32_t addr;

    password = IfxScuWdt_getSafetyWatchdogPassword();
    IfxScuWdt_clearSafetyEndinit(password); // ������ȫ����

    // �����������ڵ���������
    IfxFlash_eraseSector(USR_CONFIG_ADDR);
    IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

    IfxScuWdt_setSafetyEndinit(password);   // ��������

    // �������� STM32 ԭ�еġ�������հ�У�顱����
    for (addr = USR_CONFIG_ADDR; addr < (USR_CONFIG_ADDR + USR_CONFIG_MAX_SIZE); addr += 4)
    {
        if (*((volatile uint32_t *)addr) != 0xFFFFFFFF)
        {
            return -2; // У��ʧ��
        }
    }
    return 0;
}

int USR_erease_cogging_map(void)
{
    uint16_t password;
    uint32_t addr;

    password = IfxScuWdt_getSafetyWatchdogPassword();
    IfxScuWdt_clearSafetyEndinit(password);

    // �������ݲ�ӳ�����ڵ���������
    IfxFlash_eraseSector(COGGING_MAP_ADDR);
    IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

    IfxScuWdt_setSafetyEndinit(password);

    // �������� STM32 ԭ�еġ�������հ�У�顱����
    for (addr = COGGING_MAP_ADDR; addr < (COGGING_MAP_ADDR + COGGING_MAP_MAX_SIZE); addr += 4)
    {
        if (*((volatile uint32_t *)addr) != 0xFFFFFFFF)
        {
            return -2; // У��ʧ��
        }
    }
    return 0;
}

/* =========================================================================
 * 5. ��ȫҳд�뺯�� (�����޸ģ��� STM32 8�ֽڲ�������Ϊ TC364 32�ֽڶ���)
 * ========================================================================= */
int USR_save_config(void)
{
    // 1. ��ִ�в����Ϳհ���֤
    if (USR_erease_config()) {
        return -1;
    }

    uint16_t password;
    uint32_t addr;
    uint32_t page_count;
    uint8_t  page_buf[FLASH_PAGE_SIZE]; // 32�ֽڱ��ػ�����

    // ���� CRC У����
    UsrConfig.crc = crc32((uint8_t *)&UsrConfig, sizeof(tUsrConfig) - 4);

    password = IfxScuWdt_getSafetyWatchdogPassword();
    IfxScuWdt_clearSafetyEndinit(password); // ������ȫ�ٽ���

    addr = USR_CONFIG_ADDR;
    // ����ṹ���ܹ���Ҫռ�ö��ٸ������� 32 �ֽ�Ӳ��ҳ
    page_count = (sizeof(tUsrConfig) + (FLASH_PAGE_SIZE - 1U)) / FLASH_PAGE_SIZE;

    for (uint32_t i = 0; i < page_count; i++)
    {
        // ��ջ�����ΪĬ�ϸߵ�ƽ 0xFF
        memset(page_buf, 0xFF, FLASH_PAGE_SIZE);

        // ���㵱ǰҳ��ʣ���ʵ�����ݴ�С����ֹ�ڴ�Խ��
        uint32_t bytes_left = sizeof(tUsrConfig) - (i * FLASH_PAGE_SIZE);
        uint32_t copy_size = (bytes_left > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : bytes_left;
        memcpy(page_buf, ((uint8_t *)&UsrConfig) + (i * FLASH_PAGE_SIZE), copy_size);

        // ���� TC3xx ����д������
        IfxFlash_enterPageMode(addr);
        IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

        // ���� 2 �� 32λ����64λ˫�֣�Ϊһ�飬��4������32�ֽ�Ӳ��������
        uint32_t *p32 = (uint32_t *)page_buf;
        IfxFlash_loadPage2X32(addr, p32[0], p32[1]);
        IfxFlash_loadPage2X32(addr, p32[2], p32[3]);
        IfxFlash_loadPage2X32(addr, p32[4], p32[5]);
        IfxFlash_loadPage2X32(addr, p32[6], p32[7]);

        // ����Ӳ��������ʽд��
        IfxFlash_writePage(addr);
        IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

        addr += FLASH_PAGE_SIZE; // ���밴 32 �ֽڲ���
    }

    IfxScuWdt_setSafetyEndinit(password); // �ر��ٽ���
    return 0;
}

int USR_save_cogging_map(void)
{
    if (pCoggingMap == NULL) {
        return -3;
    }

    // 1. ��ִ�в����Ϳհ���֤
    if (USR_erease_cogging_map()) {
        return -1;
    }

    uint16_t password;
    uint32_t addr;
    uint32_t page_count;
    uint8_t  page_buf[FLASH_PAGE_SIZE];

    // �����ͼ CRC
    pCoggingMap->crc = crc32((uint8_t *)pCoggingMap, sizeof(tCoggingMap) - 4);

    password = IfxScuWdt_getSafetyWatchdogPassword();
    IfxScuWdt_clearSafetyEndinit(password);

    addr = COGGING_MAP_ADDR;
    // �����ͼռ�õ� 32 �ֽ�ҳ��
    page_count = (sizeof(tCoggingMap) + (FLASH_PAGE_SIZE - 1U)) / FLASH_PAGE_SIZE;

    for (uint32_t i = 0; i < page_count; i++)
    {
        memset(page_buf, 0xFF, FLASH_PAGE_SIZE);

        uint32_t bytes_left = sizeof(tCoggingMap) - (i * FLASH_PAGE_SIZE);
        uint32_t copy_size = (bytes_left > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : bytes_left;
        memcpy(page_buf, ((uint8_t *)pCoggingMap) + (i * FLASH_PAGE_SIZE), copy_size);

        IfxFlash_enterPageMode(addr);
        IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

        uint32_t *p32 = (uint32_t *)page_buf;
        IfxFlash_loadPage2X32(addr, p32[0], p32[1]);
        IfxFlash_loadPage2X32(addr, p32[2], p32[3]);
        IfxFlash_loadPage2X32(addr, p32[4], p32[5]);
        IfxFlash_loadPage2X32(addr, p32[6], p32[7]);

        IfxFlash_writePage(addr);
        IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

        addr += FLASH_PAGE_SIZE;
    }

    IfxScuWdt_setSafetyEndinit(password);
    return 0;
}
