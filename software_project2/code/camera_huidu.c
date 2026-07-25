/*
 * @Author: Ptisak
 * @Date: 2022-04-09 15:10:51
 * @LastEditors: skybase
 * @LastEditTime: 2024-07-01 00:47:37
 */
#include "camera_huidu.h"
/**
 * @brief : 大津法二值化
 * @param {uint8} *img_data
 * @param {int} width
 * @param {int} height
 */
int threshold_calc(uint8 *img_data, int width, int height)
{
    uint8 grey_array[256] = {0}; //灰度直方图数组
    uint8 thres;                //二值化阈值
    int i, pix_num = width * height;
    //    static uint8  last_thres;

    for (i = 0; i < pix_num; i++) //遍历所有像素
    {
        grey_array[img_data[i]]++; //统计灰度直方图
    }

    uint8 peak1_height = 0; //第一高峰海拔值
    uint8 peak1_pos = 0;   //第一高峰位置

#define KUAN 30 //山峰宽度阈值

    for (i = 0; i < 255; i++) //遍历灰度直方图
    {
        if (grey_array[i] > peak1_height) //寻找最大值
        {
            peak1_height = grey_array[i]; //记录第一高峰海拔
            peak1_pos = i;                //记录第一高峰位置
        }
    }
    //    uint8 peak2_height=0;  //第二高峰海拔值
    uint8 peak2_pos = 0; //第二高峰位置
    uint8 OK = 0;        //是否找到第二高峰

    int j;
    for (i = peak1_height - 5; i > 0; i -= 5) //向下切，找第二高峰
    {
        for (j = 0; j < 256; j++) //遍历这一行
        {
            if (grey_array[j] > i && abs(j - peak1_pos) > KUAN) //有上面部分
            {
                //                peak2_height = i; //记录第二高峰海拔
                peak2_pos = j; //记录第二高峰位置
                OK = 1;        //标志置位
                break;
            }
        }
        if (OK)
            break; //如果找到，直接跳出
    }

    uint8 H3 = 4800; //山谷海拔值
    uint8 D3 = 0;   //山谷位置

    if (OK) //已经找到2座山
    {
        if (peak1_pos < peak2_pos) //找山谷,左向右找
        {
            for (i = peak1_pos; i < peak2_pos; i++) //遍历两座山峰之间
            {
                if (grey_array[i] < H3) //寻找最小值
                {
                    H3 = grey_array[i]; //记录山谷海拔
                    D3 = i;             //记录山谷位置
                }
            }
        }
        else //右向左找
        {
            for (i = peak2_pos; i < peak1_pos; i++) //遍历两座山峰之间
            {
                if (grey_array[i] < H3) //寻找最小值
                {
                    H3 = grey_array[i]; //记录山谷海拔
                    D3 = i;             //记录山谷位置
                }
            }
        }

        thres = D3; //获取阈值
    }
    else
    {
        thres = 127;
    }
    return thres;
    //    if (last_thres== 0){
    //        last_thres = thres;
    //    }
    //        ips200_show_wave_1(0, 60, grey_array + 14, 240, 150, 240, 90);
}
#define WHITE_COLOR 255
/**
 * @brief :
 * @param {uint8} *img_data
 * @param {uint8} *output_data
 * @param {int} width
 * @param {int} height
 * @param {int} thres
 * @return 
 */
void threshold(uint8 *img_data, uint8 *output_data, int width, int height, int thres)
{
    int i, pix_num = width * height;

    //for (i = 0; i < pix_num; i++)
    for (i = 38*width; i < pix_num; i++)
    {
        if (img_data[i] > thres) //判断灰度值
        {
            output_data[i] = WHITE_COLOR; //白点
        }
        else
        {
            output_data[i] = 200; //黑点
        }
    }
}
/**
 * @brief : 图像宽高减半
 * @param {uint8} *img_data
 * @param {uint8} *img_out
 * @param {int} width
 * @param {int} height
 */
void resize(uint8 *img_data, uint8 *img_out, int width, int height)
{
    int i, j, k = 0, in = 2;
    for (j = 0; j < height; j += in)
    {
        for (i = 0; i < width; i += in)
        {
            img_out[k] = img_data[j * width + i];
            k++;
        }
    }
}

/**
 * @brief : 动态二值化, 借鉴, 效果不错
 * @param {uint8} *image
 * @param {uint16} col
 * @param {uint16} row
 */
int myotsu(uint8 *image, uint16 col, uint16 row)
{
#define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height / 4;
    uint8 thres = 0;
    uint8 *data = image; //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum = 0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            pixelCount[(int)data[i * width + j]]++; //将当前的点的像素值作为计数数组的下标
            gray_sum += (int)data[i * width + j];   //灰度值总和
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;

    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {

        w0 += pixelPro[j];        //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j]; //背景部分 每个灰度值的点的比例 *灰度值

        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;

        u0 = u0tmp / w0;   //背景平均灰度
        u1 = u1tmp / w1;   //前景平均灰度
        u = u0tmp + u1tmp; //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            thres = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }

    return thres;
}

/**
void chabihe_find_edge(int chabihe_thres_)
{
#define EDGE_DISTANCE 1
    int chabihe;
    int diff, sum_;
    int x, y;
    for (y = IMG_H - 1; y >= 0; y--)
    {
        for (x = IMG_W / 2; x < IMG_W - 1; x++)
        {
            diff = image_resize_gray[y][x] - image_resize_gray[y][x - EDGE_DISTANCE];
            sum_ = image_resize_gray[y][x] + image_resize_gray[y][x - EDGE_DISTANCE];
            chabihe = ((abs(diff)) << 9) / sum_;
            image_resize_chabihe_gray[y][x] = chabihe;
            if (chabihe > chabihe_thres_)
            {
                image_resize_bit[y][x] = 0;
            }
            else
            {
                image_resize_bit[y][x] = 100;
            }
        }
        for (x = 0; x < IMG_W / 2; x++)
        {
            diff = image_resize_gray[y][x] - image_resize_gray[y][x + EDGE_DISTANCE];
            sum_ = image_resize_gray[y][x] + image_resize_gray[y][x + EDGE_DISTANCE];
            chabihe = ((abs(diff)) << 9) / sum_;
            image_resize_chabihe_gray[y][x] = chabihe;

            if (chabihe > chabihe_thres_)
            {
                image_resize_bit[y][x] = 0;
            }
            else
            {
                image_resize_bit[y][x] = 100;
            }
        }
    }
    for (x = 0; x < IMG_W - 1; x++)
    {
        for (y = 0; y < IMG_H / 2; y++)
        {
            diff = image_resize_gray[y][x] - image_resize_gray[y + EDGE_DISTANCE][x];
            sum_ = image_resize_gray[y][x] + image_resize_gray[y + EDGE_DISTANCE][x];
            chabihe = ((abs(diff)) << 9) / sum_;
            image_resize_chabihe_gray[y][x] = chabihe;

            if (chabihe > chabihe_thres_)
            {
                image_resize_bit[y][x] = 0;
            }
            else
            {
                // image_resize_bit[y][x] = 100;
            }
        }
        for (y = IMG_H / 2; y < IMG_H - 1; y++)
        {
            diff = image_resize_gray[y][x] - image_resize_gray[y - EDGE_DISTANCE][x];
            sum_ = image_resize_gray[y][x] + image_resize_gray[y - EDGE_DISTANCE][x];
            chabihe = ((abs(diff)) << 9) / sum_;
            image_resize_chabihe_gray[y][x] = chabihe;

            if (chabihe > chabihe_thres_)
            {
                image_resize_bit[y][x] = 0;
            }
            else
            {
                // image_resize_bit[y][x] = 100;
            }
        }
    }
}


/*---------------------------------------------------------------
 【函    数】Bin_Image_Filter
 【功    能】过滤噪点
 【参    数】无
 【返 回 值】无
 【注意事项】
 ----------------------------------------------------------------*/
 /**
void Bin_Image_Filter(void)
{
    int16_t nr; //行
    int16_t nc; //列

    for (nr = 1; nr < IMG_H - 1; nr++)
    {
        for (nc = 1; nc < IMG_W - 1; nc = nc + 1)
        {
            if ((image_resize_bit[nr][nc] == 0) && (image_resize_bit[nr - 1][nc] + image_resize_bit[nr + 1][nc] + image_resize_bit[nr][nc + 1] + image_resize_bit[nr][nc - 1] > 2 * WHITE_COLOR))
            {
                image_resize_bit[nr][nc] = WHITE_COLOR;
            }
            else if ((image_resize_bit[nr][nc] == WHITE_COLOR) && (image_resize_bit[nr - 1][nc] + image_resize_bit[nr + 1][nc] + image_resize_bit[nr][nc + 1] + image_resize_bit[nr][nc - 1] < 2 * WHITE_COLOR))
            {
                image_resize_bit[nr][nc] = 0;
            }
        }
    }
}

*/