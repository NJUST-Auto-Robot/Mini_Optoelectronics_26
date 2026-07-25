/*********************************************************************************************************************
* TC364 Opensourec Library ����TC364 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC364 ��Դ���һ����
*
* TC364 ��Դ�� ���������
* �����Ը���������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù�������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ����֤Э�� ������������Ϊ���İ汾
* ��������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ����֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          zf_device_menc15a
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.10.2
* ����ƽ̨          TC364DP
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-11-02       pudding            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   // Ӳ�� SPI ����
*                   SCL/SPC           �鿴 zf_device_menc15a.h �� MENC15A_SPC_PIN �궨��
*                   SDA/DSI           �鿴 zf_device_menc15a.h �� MENC15A_SDI_PIN �궨��
*                   SA0/SDO           �鿴 zf_device_menc15a.h �� MENC15A_SDO_PIN �궨��
*                   CS                �鿴 zf_device_menc15a.h �� MENC15A_CS_PIN �궨��
*                   VCC               3.3V��Դ
*                   GND               ��Դ��
*                   ������������
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_common_function.h"
#include "zf_device_config.h"
#include "zf_driver_delay.h"
#include "zf_driver_gpio.h"
#include "zf_driver_spi.h"

#include "zf_device_menc15a.h"

uint16 menc15a_absolute_data[2] = {0};

int16 menc15a_absolute_offset_data[2] = {0};

int16  menc15a_speed_data[2] = {0};

//-------------------------------------------------------------------------------------------------------------------
// �������     �����������תƫ��
// ����˵��     encoder_max     ����������  ��дʮ��������
// ����˵��     now_location    ��ǰλ����Ϣ
// ����˵��     last_location   ��һ�ε�λ����Ϣ
// ���ز���     int32           �����ƫ��ֵ
// ʹ��ʾ��     magnetic_encoder_get_offset();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
static int32 magnetic_encoder_get_offset (int32 encoder_max, int32 now_location, int32 last_location)
{
    int32 result_data = 0;
	
    if((encoder_max / 2) < func_abs(now_location - last_location))
    {
        result_data = ((encoder_max / 2) < now_location ? (now_location - encoder_max - last_location) : (now_location + encoder_max - last_location));
    }
    else
    {
        result_data = (now_location - last_location);
    }
	
    return result_data;
}


//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ MENC15A �ű����� �� ����ֵ �Ƕ�����
// ����˵��     menc15a_module      �ű����� ģ���
// ���ز���     uint16              ����ֵ �Ƕ�����
// ʹ��ʾ��     menc15a_1_get_absolute_data();
// ��ע��Ϣ     ִ�иú����󣬿�ֱ��ʹ�÷���ֵ Ҳ����ͨ����ѯ��Ӧ������ȡ���
//-------------------------------------------------------------------------------------------------------------------
uint16 menc15a_get_absolute_data(menc15a_module_enum menc15a_module)
{
    uint16 read_data = 0;
  
    uint16 data_last = menc15a_absolute_data[menc15a_module];
    
    if(menc15a_module == menc15a_1_module)
    {
        MENC15A_1_CS(0);

        read_data = spi_read_16bit_register(MENC15A_1_SPI, 0x8021);

        MENC15A_1_CS(1);
    }
    else
    {
        MENC15A_2_CS(0);
      
        spi_write_8bit(MENC15A_2_SPI, 0x80);
        spi_write_8bit(MENC15A_2_SPI, 0x21);
        read_data = spi_read_8bit(MENC15A_2_SPI);
        read_data = ((read_data << 8) | spi_read_8bit(MENC15A_2_SPI));
        
        MENC15A_2_CS(1);      
    }
  
    menc15a_absolute_data[menc15a_module] = (read_data & 0x7fff);
    
    menc15a_absolute_offset_data[menc15a_module] = (int16)magnetic_encoder_get_offset(32768, menc15a_absolute_data[menc15a_module], data_last);

    menc15a_absolute_offset_data[menc15a_module] = menc15a_absolute_offset_data[menc15a_module] - 16384;
    
    return menc15a_absolute_data[menc15a_module];
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ MENC15A �ű����� �� ת������
// ����˵��     menc15a_module      �ű����� ģ���
// ���ز���     int16               ת������(ÿ����ת���� rad/s)
// ʹ��ʾ��     menc15a_1_get_speed_data();
// ��ע��Ϣ     ִ�иú����󣬿�ֱ��ʹ�÷���ֵ Ҳ����ͨ����ѯ��Ӧ������ȡ���
//-------------------------------------------------------------------------------------------------------------------
int16 menc15a_get_speed_data(menc15a_module_enum menc15a_module)
{
    int16 read_data = 0;
    
    if(menc15a_module == menc15a_1_module)
    {
        MENC15A_1_CS(0);
        
        read_data = spi_read_16bit_register(MENC15A_1_SPI, 0x8032);

        MENC15A_1_CS(1);
    }
    else
    {
        MENC15A_2_CS(0);
      
        read_data = spi_read_16bit_register(MENC15A_2_SPI, 0x8032);

        MENC15A_2_CS(1);
    }
  
    read_data = (read_data & 0x7fff);
    
    if(read_data > 16384)
    {
        read_data = read_data - 32768;
    }
    
    menc15a_speed_data[menc15a_module] = (int16)((float)read_data * 1.917476f);
    
    return menc15a_speed_data[menc15a_module];
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ʼ�� MENC15A
// ����˵��     void
// ���ز���     uint8           1-��ʼ��ʧ�� 0-��ʼ���ɹ�
// ʹ��ʾ��     menc15a_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 menc15a_init(void)
{
    spi_init(MENC15A_1_SPI, SPI_MODE1, MENC15A_1_SPI_SPEED, MENC15A_1_CLK_PIN, MENC15A_1_MOSI_PIN, MENC15A_1_MISO_PIN, SPI_CS_NULL);    // ���� MENC15A-1 �� SPI�˿�
    gpio_init(MENC15A_1_CS_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);                                                                         // ���� MENC15A-1 �� CS�˿�

    spi_init(MENC15A_2_SPI, SPI_MODE1, MENC15A_2_SPI_SPEED, MENC15A_2_CLK_PIN, MENC15A_2_MOSI_PIN, MENC15A_2_MISO_PIN, SPI_CS_NULL);    // ���� MENC15A-2 �� SPI�˿�
    gpio_init(MENC15A_2_CS_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);                                                                         // ���� MENC15A-2 �� CS�˿�
    
    return 0;
}

