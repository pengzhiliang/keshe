#include "gprmc.h"


// 计算校验:异或
int checksum(const char* s)
{
    int c = 0;

    while (*s)
        c ^= *s++;

    return c;
}

// 打印错误信息
int print_error(int err)
{

#ifdef DEBUG

    switch (err) {
    case 1:
        printf("行首不是 $ --> ");
        break;

    case 2:
        printf("校验值错误 --> ");
        break;

    case 3:
        printf("找不到校验值 --> ");
        break;

    case 4:
        printf("不是$GPRMC帧头--> ");
        break;

    default:
        printf("数据错误 --> ");
        break;
    }

    printf("请检查是否 NMEA 183 数据!\n");

#endif

    return err;

}

// 读取一行GPRMC数据，判断是否是GPRMC数据行
bool read_gprmc(const char* line, gprmc_format& rmc)
{

    char line_buf[BUFSIZ];
    strncpy(line_buf, line, BUFSIZ);

    //int chksum = 0;

    if ('$' != line_buf[0]) {
        print_error(1);
        return false;;

    } else {

        char* pch = strrchr(line_buf, '*'); //找到校验和前面的字段
        if (pch != NULL) {
            *pch = '\0';
            rmc.mode = *(pch - 1);
            pch++;
            rmc.chksum = strtol(pch, &pch, 16);//进制转换
            // printf("%X\n", chksum);

            if (rmc.chksum != checksum(line_buf + 1)) {
                print_error(2);
                return false;;
            }
        } else {
            print_error(3);
            return false;;
        }


        if (strstr(line_buf, ",,"))     //判断",,"是否是line_buf的子串
            multi_replace(line_buf, ",,", ",|,");   //是的话就用",|,"代替",,"

        pch = strtok(line_buf, ",");        //分解字符串，以","为分割符
        if ((pch != NULL) && (!strcmp(pch, "$GNRMC") || !strcmp(pch, "$GPRMC"))) {//比较是不是"$GPRMC",相等则返回0
            // printf("%s\n", pch);    //GPRMC GNRMC

            pch = strtok(NULL, ",");
            rmc.rcv_time = atof(pch);

            pch = strtok(NULL, ",");
            rmc.status = *pch;

            pch = strtok(NULL, ",");
            rmc.lat = atof(pch);

            pch = strtok(NULL, ",");
            rmc.lat_direct  = *pch;

            pch = strtok(NULL, ",");
            rmc.lon = atof(pch);

            pch = strtok(NULL, ",");
            rmc.lon_direct = *pch;

            pch = strtok(NULL, ",");
            rmc.speed  = atof(pch);

            pch = strtok(NULL, ",");
            rmc.cog  = atof(pch);

            pch = strtok(NULL, ",");
            rmc.date  = atoi(pch);

            pch = strtok(NULL, ",");
            rmc.mag_variation   = atof(pch);

            pch = strtok(NULL, ",");
            rmc.mag_var_direct  = *pch;

            rmc.mode = rmc.mode; // 之前已经读到

        } else {
            print_error(4);
            return false;;

        }

    }

    return true;
}


void fprintf_gps(FILE* outfile,gprmc_format gps_point)
{
    //经纬度转换
    double lat = gps_point.lat;
    double lon = gps_point.lon;

    //lat = degree_minute2dec_degrees(gps_point.lat);
    //lon = degree_minute2dec_degrees(gps_point.lon);
    lat = lat / 100.0;
    lon = lon / 100.0;

    fprintf(outfile, "纬度：%.6f\n", lat);       // 纬度
    fprintf(outfile, "经度：%.6f\n", lon);        // 经度

    //double nm_to_km = 1.852;  // 时速:节换算公里
    double nm_to_km = 1.0;    //cJ-02记录的是公里
    fprintf(outfile, "速度：%.1f\n", gps_point.speed * nm_to_km);

    //时间转换
    char time_str [80] = "";
    int date = gps_point.date;
    int time = (int)gps_point.rcv_time;

    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d",
            date / 10000, (date % 10000) / 100,  date % 100,
            time / 10000, (time % 10000) / 100,  time % 100);
    fprintf(outfile, "%s\n", time_str);
    fprintf(outfile,"%s\n","finished!");
   // fclose(outfile);
}


// 经纬度ddmm.mmmm，度分格式 转换成 10进制 度小数
double degree_minute2dec_degrees(double deg)
{
    deg = (deg / 100.0 - (int)deg / 100) * 100.0 / 60.0 + (int)deg / 100 ;
    return deg;
}


// c_string 字符串替换
char* cs_replace(char* cs, const char* str1, const char* str2)
{
    char* ret = cs;
    char* pc = new char[strlen(cs) + 1];

    char* pch = strstr(cs, str1); // 找到替换处
    if (pch == NULL)
        return pch;

    // 从目标出重新组合
    strcpy(pc, cs);     // 复制副本
    strcpy(pch, str2);
    strcat(pch + strlen(str2), pc + (pch - cs) + strlen(str1));

    delete[] pc;

    return ret;
}

// c_string 多重字符串替换
char* multi_replace(char* cs, const char* str1, const char* str2)
{
    char* pch = strstr(cs, str1); // 检查是否要替换
    if (pch == NULL)
        return pch;

    while (strstr(cs, str1))
        cs_replace(cs, str1, str2);
    return cs;
}
