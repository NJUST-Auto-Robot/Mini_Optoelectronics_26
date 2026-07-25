#include "math.h"
#include "all.h"

#define WHITE       255         //二值化后白点的灰度值
#define BLACK       200         //二值化后黑点的灰度值，设置为灰色，其他重要线条设置为黑色便于观察
#define CHECK_STEP  3           //扫描时的间隔(每个2个像素点扫描一次)
#define CAR_CENTRE 105          //镜头中的车辆中心对应横坐标

//由于近大远小，该数据为每行像素点的权重
const float RowQuanzhong[28] =
    {1,1,1, 13.0/12 , 13.0/12 , 13.0/11 , 13.0/11 , 13.0/11 , 13.0/11 , 13.0/11 , 13.0/11 , 13.0/11 , 13.0/11 , 13.0/10
    , 13.0/10 , 13.0/10 , 13.0/10 , 13.0/9 , 13.0/9 , 13.0/9 , 13.0/9 , 13.0/8 , 13.0/7 , 13.0/7 , 13.0/7 , 13.0/6 , 13.0/6 };
volatile track_attribute_enum track_attribute = SIMPLE_TRACK;    //赛道特征
uint8 curve_thre = 15;
int16 Start_Track_Err = 0;
int16 Qianzhan_Track_Angle = 0;

/**********************************
*@brief:为图像画一个两层宽的黑框防止爬出界
*@param:
*@return:
**********************************/
void drawHeikuang(uint8 * image,uint16 image_w,uint16 image_h)
{
    uint16 i;
    for(i = 0;i < image_w;i++)
    {
        image[0 * image_w + i] = BLACK;
        image[1 * image_w + i] = BLACK;
        image[(image_h - 1) * image_w + i] = BLACK;
        image[(image_h - 2) * image_w + i] = BLACK;
    }
    for(i = 0;i < image_h;i++)
    {
        image[i * image_w + 0] = BLACK;
        image[i * image_w + 1] = BLACK;
        image[i * image_w + image_w - 1] = BLACK;
        image[i * image_w + image_w - 2] = BLACK;
    }
}

/**********************************
*@brief:Bresenham算法绘制线段
*@param:image 指向图像数据的指针（二维数组）
* @param :width 图像宽度
* @param :height 图像高度
* @param :x0 起点x坐标
* @param :y0 起点y坐标
* @param :x1 终点x坐标
* @param :y1 终点y坐标
* @param :dir 拓宽线方向 -1:向左  1:向右  0:不进行拓宽
*@return:成功返回0，失败返回-1
**********************************/
int draw_line(uint8* image, uint16 width, uint16 height, uint8 x0, uint8 y0, uint8 x1, uint8 y1,int8 dir,uint8 colour)
{
    // 边界检查
    if (image == NULL || width <= 0 || height <= 0)
    {
        return -1;
    }
    // 计算坐标差
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    // 绘制线段
    while (1)
    {
        // 检查坐标是否在图像范围内
        if (x0 >= 0 && x0+1< width && y0 >= 0 && y0< height)
        {
            //画一条两个单位宽度的线段
            image[y0 * width + x0] = colour;
            image[y0 * width + x0+dir] = colour;
            //再宽一点
            image[(y0-dir) * width + x0] = colour;
            image[(y0-dir) * width + x0+dir] = colour;
        }
        // 判断是否到达终点
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        // Bresenham算法核心
        e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
    return 0;
}

uint8 qianzhan_num = 9;
uint8 column_left_zero_track_count;
uint8 column_left_two_track_count;
uint8 column_left_one_track_count;
uint8 column_right_zero_track_count;
uint8 column_right_one_track_count;
uint8 column_right_two_track_count;
uint8 row_below_zero_track_count;
uint8 row_below_one_track_count;
uint8 row_below_two_track_count;
uint8 row_above_zero_track_count;
uint8 row_above_one_track_count;
uint8 row_above_two_track_count;
//0:上升沿即从黑跳变到白;1:下降沿即从白跳变到黑;2:白点数量
uint8 check_row[28][3] = {0};               //从左往右每隔2个像素扫一行
uint8 check_column[43][3] = {0};            //从下往上每隔2个像素扫一列
uint8 start_border_count;
uint8 qianzhan_border_count;
uint8 row_max_white[2] = {0,0};             //0:行序号;1:白点数
uint8 column_max_white[2] = {0,0};          //0:列序号;1:白点数

uint8 qianzhan_border_x_last = CAR_CENTRE;
uint8 start_border_x_last =    CAR_CENTRE;
uint8 qianzhan_border_x =      CAR_CENTRE;   //最终前瞻处中点横坐标
uint8 start_border_x =         CAR_CENTRE;   //最终起始处中点横坐标

/**********************************
*@brief:图像处理
*@param:
*@return:
*@note:最终使用的视觉处理方案
**********************************/
uint8 imageProcess(uint8 * image,uint16 image_w,uint16 image_h)
{

    //0:上升沿即从黑跳变到白;1:下降沿即从白跳变到黑;2:白点数量
    //check_row[28][3] = {0};   //从左往右每隔2个像素扫一行
    //check_column[43][3] = {0};    //从下往上每隔2个像素扫一列
    column_max_white[0] = 0;
    column_max_white[1] = 0;
    row_max_white[0] = 0;
    row_max_white[1] = 0;

    uint8 qianzhan_border_left_x[4] = {0};   //前瞻处的边界横坐标
    uint8 qianzhan_border_right_x[4] = {0};
    qianzhan_border_count = 0;    //前瞻处赛道数量
    uint8 start_border_left_x[4] = {0};  //起始处的边界横坐标
    uint8 start_border_right_x[4] = {0};
    start_border_count = 0; //起始边界赛道数量

    column_left_zero_track_count = 0;       //主赛道左边列方向无赛道计数
    column_left_one_track_count = 0;        //主赛道左边列方向单赛道计数
    column_left_two_track_count = 0;        //主赛道左边列方向双赛道计数
    column_right_zero_track_count = 0;      //主赛道右边列方向无赛道计数
    column_right_one_track_count = 0;       //主赛道右边列方向单赛道计数
    column_right_two_track_count = 0;       //主赛道右边列方向双赛道计数
    row_below_zero_track_count = 0;
    row_below_one_track_count = 0;
    row_below_two_track_count = 0;
    row_above_zero_track_count = 0;
    row_above_one_track_count = 0;
    row_above_two_track_count = 0;
    uint8 row_two_track_count = 0;          //横向双赛道计数
    uint8 above_one_track_count = 0;        //上方13个扫描行单赛道计数

    uint16 i,j,temp;

//清零
    for(i = 0;i < 43;i++)
    {
        if(i < 28)
        {
            check_column[i][0] = check_column[i][1] = check_column[i][2] = 0;
            check_row[i][0] = check_row[i][1] = check_row[i][2] = 0;
        }
        else check_column[i][0] = check_column[i][1] = check_column[i][2] = 0;
    }

//画黑边界防止寻界异常
    for(i = 0;i < 82;i++)
    {
        image[(119-i)*image_w + 35] = BLACK;
        image[(119-i)*image_w + 165] = BLACK;
    }

//扫每一行
for(i = 0;i < 28;i++)
{
    if(i >= 24)     //抛掉前方两个角处的信息以防干扰
    {
        for(j = 65;j < 135;j++)
        {
            if(image[(119-i*CHECK_STEP)*image_w + j] == WHITE)  //若当前为白点
            {
                check_row[i][2]++;  //白点数加一
                //如果是右边界
                if(image[(119-i*CHECK_STEP)*image_w + j+1] == BLACK)
                {
                    check_row[i][1]++;  //右边界计数加一
                }
            }
            else
            {
                //如果是左边界
                if(image[(119-i*CHECK_STEP)*image_w + j+1] == WHITE)
                {
                    check_row[i][0]++;  //左边界计数加一
                }
            }
        }
    }
    else for(j = 35;j < 165;j++)
    {
        if(image[(119-i*CHECK_STEP)*image_w + j] == WHITE)  //若当前为白点
        {
            check_row[i][2]++;  //白点数加一
            //如果是右边界
            if(image[(119-i*CHECK_STEP)*image_w + j+1] == BLACK)
            {
                check_row[i][1]++;  //右边界计数加一
                if(i == qianzhan_num)    //如果是前瞻右边界
                {
                    qianzhan_border_right_x[qianzhan_border_count] = j + 1;
                    //若左右间距过小(3)，则视为干扰
                    if(qianzhan_border_left_x[qianzhan_border_count] != 0 && qianzhan_border_right_x[qianzhan_border_count]-qianzhan_border_left_x[qianzhan_border_count] >= 3)
                    {
                        if(qianzhan_border_count < 4)
                        qianzhan_border_count++;
                    }
                    else
                    {
                        qianzhan_border_right_x[qianzhan_border_count] = 0;
                        qianzhan_border_left_x[qianzhan_border_count] = 0;
                    }
                }
                else if(i == 0)     //如果是起始右边界
                {
                    start_border_right_x[start_border_count] = j + 1;
                    //若左右间距过小(3)，则视为干扰
                    if(start_border_left_x[start_border_count] != 0 && start_border_right_x[start_border_count]-start_border_left_x[start_border_count] >= 3)
                    {
                        if(start_border_count < 4)
                            start_border_count++;
                    }
                    else
                    {
                        start_border_right_x[start_border_count] = 0;
                        start_border_left_x[start_border_count] = 0;
                    }
                }
            }
        }
        else   //若当前为黑点
        {
            //如果是左边界
            if(image[(119-i*CHECK_STEP)*image_w + j+1] == WHITE)
            {
                check_row[i][0]++;  //左边界计数加一
                if(i == qianzhan_num)    //如果是前瞻左边界
                {
                    qianzhan_border_left_x[qianzhan_border_count] = j + 1;
                }
                else if(i == 0)     //如果是起始左边界
                {
                    start_border_left_x[start_border_count] = j + 1;
                }
            }
        }
    }
    check_row[i][2] *= RowQuanzhong[i]; //每行白点数乘权重

    if(check_row[i][0] == 2 && check_row[i][1] == 2)
    {
        //记录横向双赛道数量
        row_two_track_count++;
    }

    //跟新横向白点数最多的赛道信息
    if(check_row[i][2] > row_max_white[1])
    {
        row_max_white[1] = check_row[i][2];
        row_max_white[0] = i;
    }
}

//扫每一列
for(i = 0;i < 43;i++)
    {
        if(i <= 10 || i >= 33)     //抛掉前方两个角处的信息以防干扰
        {
            for(j = 119;j > 48;j--)
            {
                if(image[j*image_w + (35+i*CHECK_STEP)] == WHITE)   //若当前为白点
                {
                    check_column[i][2]++;   //白点数加一
                    if(image[(j-1)*image_w + (35+i*CHECK_STEP)] == BLACK)   //若下一个点为黑点
                    {
                        //若上下间距过小(3)，则视为干扰
                        if((temp - (j-1)) <= 3)
                        {
                            check_column[i][0]--;
                            temp = 0;
                        }
                        else check_column[i][1]++;  //若从白跳变到黑
                    }
                }
                else
                {
                    if(image[(j-1)*image_w + (35+i*CHECK_STEP)] == WHITE)
                    {
                        check_column[i][0]++;  //若从黑跳变到白
                        temp = j;   //记录其纵坐标
                    }
                }
            }
        }
        else for(j = 119;j > 38;j--)
        {
            if(image[j*image_w + (35+i*CHECK_STEP)] == WHITE)
            {
                check_column[i][2]++;   //白点数加一
                if(image[(j-1)*image_w + (35+i*CHECK_STEP)] == BLACK)
                {
                    //若上下间距过小(3)，则视为干扰
                    if((temp - (j-1)) <= 3)
                    {
                        check_column[i][0]--;
                        temp = 0;
                    }
                    else check_column[i][1]++;  //若从白跳变到黑
                }
            }
            else
            {
                if(image[(j-1)*image_w + (35+i*CHECK_STEP)] == WHITE)
                {
                    check_column[i][0]++;  //若从黑跳变到白
                    temp = j;   //记录其纵坐标
                }
            }
        }
        //更新列最大白点的数量和所在列序号,并认为其为主赛道
        if(check_column[i][2] > column_max_white[1])
        {
            column_max_white[1] = check_column[i][2];
            column_max_white[0] = i;
        }
    }

//从主赛道左边扫描
    for(i = 0;i < (column_max_white[0]);i++)
    {
        if(check_column[i][2] < 3)
            column_left_zero_track_count++;
        else if(check_column[i][2] < 22)
            column_left_one_track_count++;
        else if(check_column[i][2] < 35)
            column_left_two_track_count++;
    }

//从主赛道右边扫描
    for(i = (column_max_white[0]+1);i < 43;i++)
    {
        if(check_column[i][2] < 3)
            column_right_zero_track_count++;
        else if(check_column[i][2] < 22)
            column_right_one_track_count++;
        else if(check_column[i][2] < 35)
            column_right_two_track_count++;
    }

//从横向白点数最多赛道下方扫描
    for(i = 0;i < row_max_white[0];i++)
    {
        if(check_row[i][2] < 3)
            row_below_zero_track_count++;
        else if(check_row[i][2] < 22)
        {
            row_below_one_track_count++;
            if(i >= 15)above_one_track_count++;
        }
        else if(check_row[i][2] < 35)
            row_below_two_track_count++;
    }

//从横向白点数最多赛道上方扫描
    for(i = row_max_white[0];i < 28;i++)
    {
        if(check_row[i][2] < 3)
            row_above_zero_track_count++;
        else if(check_row[i][2] < 22)
        {
            row_above_one_track_count++;
            if(i >= 15)above_one_track_count++;
        }
        else if(check_row[i][2] < 35)
            row_above_two_track_count++;
    }

//判断赛道元素
    switch(track_attribute)
    {
        case SIMPLE_TRACK:
        case LEFT_CURVE:
        case RIGHT_CURVE:
        case T_FORK:
        case LEFT_T_FORK:
        case RIGHT_T_FORK:
        {
            if(row_max_white[1] <= 40 && (float)column_left_zero_track_count/(column_max_white[0]-0) >= 0.7 &&
                (float)column_right_zero_track_count/(42-column_max_white[0]) >= 0.7)   //左右列0赛道数量占比大于70%
            {
                track_attribute = SIMPLE_TRACK;
            }
            else if(row_max_white[1] > 35 && column_max_white[1] >= 35 && row_max_white[0] <= curve_thre &&
                (float)row_below_one_track_count/(row_max_white[0]-0) >= 0.45 && (float)row_above_zero_track_count/(27-row_max_white[0]) >= 0.45 &&
                (float)column_left_zero_track_count/(column_max_white[0]-0) >= 0.45 && (float)column_right_one_track_count/(42-column_max_white[0]) >= 0.45)
            {
                track_attribute = RIGHT_CURVE;
            }
            else if(row_max_white[1] > 35 && column_max_white[1] >= 35 && row_max_white[0] <= curve_thre &&
                (float)row_below_one_track_count/(row_max_white[0]-0) >= 0.45 && (float)row_above_zero_track_count/(27-row_max_white[0]) >= 0.45 &&
                (float)column_left_one_track_count/(column_max_white[0]-0) >= 0.45 && (float)column_right_zero_track_count/(42-column_max_white[0]) >= 0.45)
            {
                track_attribute = LEFT_CURVE;
            }
            else if(row_max_white[1] > 70 && column_max_white[1] >= 35 && row_max_white[0] <= curve_thre &&
                (float)row_below_one_track_count/(row_max_white[0]-0) >= 0.45 && (float)row_above_zero_track_count/(27-row_max_white[0]) >= 0.45 &&
                (float)column_left_one_track_count/(column_max_white[0]-0) >= 0.45 && (float)column_right_one_track_count/(42-column_max_white[0]) >= 0.45)
            {
                track_attribute = T_FORK;
            }
            else if(row_max_white[1] > 35 && column_max_white[1] >= 70 && row_max_white[0] <= curve_thre &&
                (float)row_below_one_track_count/(row_max_white[0]-0) >= 0.45 && (float)row_above_one_track_count/(27-row_max_white[0]) >= 0.45 &&
                (float)column_left_one_track_count/(column_max_white[0]-0) >= 0.45 && (float)column_right_zero_track_count/(42-column_max_white[0]) >= 0.45)
            {
                track_attribute = LEFT_T_FORK;
            }
            else if(row_max_white[1] > 35 && column_max_white[1] >= 70 && row_max_white[0] <= curve_thre &&
                (float)row_below_one_track_count/(row_max_white[0]-0) >= 0.45 && (float)row_above_one_track_count/(27-row_max_white[0]) >= 0.45 &&
                (float)column_left_zero_track_count/(column_max_white[0]-0) >= 0.45 && (float)column_right_one_track_count/(42-column_max_white[0]) >= 0.45)
            {
                track_attribute = RIGHT_T_FORK;
            }
            break;
        }

        default:break;
    }

//开始确定起始赛道及前瞻赛道中点
    switch(track_attribute)
    {
        case SIMPLE_TRACK:  //简单赛道
        {
            //确定起始赛道中点
            start_border_x = 0.5*(start_border_left_x[0] + start_border_right_x[start_border_count-1]);
            //确定前瞻赛道中点
            qianzhan_border_x = 0.5*(qianzhan_border_left_x[0] + qianzhan_border_right_x[qianzhan_border_count-1]);
            break;
        }

        default:break;
    }

//为扫描边界画一个黑框，只做观察
    for(i = 0;i < 82;i++)
    {
        image[(119-i)*image_w + 34] = 0;
        image[(119-i)*image_w + 166] = 0;
    }
    for(i = 34;i < 167;i++)
    {
        image[38*image_w + i] = 0;
    }

//得出最终数据
    switch(track_attribute)
    {
        case LEFT_CURVE:
        {
            Start_Track_Err = start_border_x - CAR_CENTRE;
            Qianzhan_Track_Angle = -1*70;   //该处对直角弯的控制已经没用了，新的控制在pid.c中
            break;
        }
        case RIGHT_CURVE:
        {
            Start_Track_Err = start_border_x - CAR_CENTRE;
            Qianzhan_Track_Angle = 70;   //该处对直角弯的控制已经没用了，新的控制在pid.c中
            break;
        }
        default:
        {
            Start_Track_Err = start_border_x - CAR_CENTRE;
            Qianzhan_Track_Angle = fabs(57*atan((double)((qianzhan_border_x - CAR_CENTRE)/27.0)));      //先赋值再填符号
            if((qianzhan_border_x-CAR_CENTRE) < 0)Qianzhan_Track_Angle = -Qianzhan_Track_Angle;
            break;
        }
    }

//显示巡线
    draw_line(image,image_w,image_h,start_border_x,119,qianzhan_border_x,119-CHECK_STEP*qianzhan_num,0,0);
    return 0.5f*(start_border_left_x[0]+start_border_right_x[0]);
}
