/*
 * @Author: Ptisak
 * @Date: 2022-03-12 16:02:29
 * @LastEditors: skybase
 * @LastEditTime: 2024-07-01 00:43:06
 * @Description: file content
 */

#ifndef CAM_HUI_H
#define CAM_HUI_H

#include "zf_common_headfile.h"

int myotsu(uint8 *image, uint16 col, uint16 row);
void resize(uint8 *img_data, uint8 *img_out, int width, int height);
void threshold(uint8 *img_data, uint8 *output_data, int width, int height, int thres);
int threshold_calc(uint8 *img_data, int width, int height);
void chabihe_find_edge(int chabihe_thres);
void Bin_Image_Filter(void);
#endif