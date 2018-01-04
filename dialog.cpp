#include "dialog.h"
#include "ui_dialog.h"
//#include "mainwindow.h"

//extern int gpspoints;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}


Dialog::~Dialog()
{
    delete ui;
}
/*
void Dialog::showInfenceLog(QString log[],int length)
{
    setWindowFlags(Qt::WindowCloseButtonHint|Qt::MSWindowsFixedSizeDialogHint);

    ui->label_2->setText(log[0]);
    ui->label_9->setText(log[11]);

    ui->label_3->setText(log[1]);
    ui->label_10->setText(log[10]);

    ui->label_4->setText(log[2]);
    ui->label_11->setText(log[9]);

    ui->label_5->setText(log[3]);
    ui->label_12->setText(log[8]);

    ui->label_6->setText(log[4]);
    ui->label_13->setText(log[7]);

    ui->label_7->setText(log[5]);
    ui->label_14->setText(log[6]);

    show();
}
*/

/*
void Dialog::showOutfenceLog(ponit_format log[], int length)
{
    setWindowFlags(Qt::WindowCloseButtonHint|Qt::MSWindowsFixedSizeDialogHint);

    ui->label_2->setText(log[0]);
    ui->label_9->setText(log[11]);

    ui->label_3->setText(log[1]);
    ui->label_10->setText(log[10]);

    ui->label_4->setText(log[2]);
    ui->label_11->setText(log[9]);

    ui->label_5->setText(log[3]);
    ui->label_12->setText(log[8]);

    ui->label_6->setText(log[4]);
    ui->label_13->setText(log[7]);

    ui->label_7->setText(log[5]);
    ui->label_14->setText(log[6]);

    show();
}
*/

void Dialog::FenceLog(ponit_format log[],int lenth_)
{
    fencelenth = lenth_;
    lenth = 0;
    if(fencelenth==0)
    {
        QMessageBox::warning(this, tr("警告"), tr("没有数据"));
        return ;
    }
    FENCE = log;
    fence_now = 0;
    showFenceLog(FENCE + fence_now);

}

void Dialog::showFenceLog(ponit_format* log)
{
    ui->label_2->setText(log[0].lon);
    ui->label_9->setText(log[0].lat);

    ui->label_3->setText(log[1].lon);
    ui->label_10->setText(log[1].lat);

    ui->label_4->setText(log[2].lon);
    ui->label_11->setText(log[2].lat);

    ui->label_5->setText(log[3].lon);
    ui->label_12->setText(log[3].lat);

    ui->label_6->setText(log[4].lon);
    ui->label_13->setText(log[4].lat);

    ui->label_7->setText(log[5].lon);
    ui->label_14->setText(log[5].lat);

    setWindowFlags(Qt::WindowCloseButtonHint|Qt::MSWindowsFixedSizeDialogHint); //只显示关闭

    show();
}

void Dialog::ClearLog()
{
    setWindowFlags(Qt::WindowCloseButtonHint|Qt::MSWindowsFixedSizeDialogHint);

    ui->label_2->clear();
    ui->label_9->clear();

    ui->label_3->clear();
    ui->label_10->clear();

    ui->label_4->clear();
    ui->label_11->clear();

    ui->label_5->clear();
    ui->label_12->clear();

    ui->label_6->clear();
    ui->label_13->clear();

    ui->label_7->clear();
    ui->label_14->clear();

    //show();
}


void Dialog::NmeaLog(gprmc_format rmc[], int lenth_)
{
    lenth = lenth_;         //保存长度
    fencelenth = 0;

    if(lenth==0)
    {
        QMessageBox::warning(this, tr("警告"), tr("没有数据，请打开NMEA文档进行解析！"));
        return ;
    }
    GPS_POINTS=rmc;         //保存数据
    now = lenth-1;          //现在显示值
    showNmealog(GPS_POINTS[now]);
}

void Dialog::showNmealog(gprmc_format rmc)
{
    /* 共七栏      第一列           第二列
     * 第一栏      经度：      mmmmmmm    W/E
     * 第二栏      纬度：      mmmmmmm    N/S
     * 第三栏      时间:       date      time
     * 第四栏      速度：      mmmmm   km/h
     * 第五栏      状态：      A=定位，V=未定位
     * 第六栏      方位角：    度（二维方向指向，相当于二维罗盘）
     * 第七栏      磁偏角:    （000 - 180）度   W/E
     *
     */
    QString lat =  QString::number(rmc.lat);
    QString lon =  QString::number(rmc.lon);
    char time_str [80] = "";
    int date = rmc.date;
    int time = (int)rmc.rcv_time;
    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d",
            date / 10000, (date % 10000) / 100,  date % 100,
            time / 10000, (time % 10000) / 100,  time % 100);
    QString time_ = time_str;
    char* status;
    if(rmc.status=='A')
    {
        status = "定位成功";
    }
    else
    {
        status = "定位失败";
    }
    //第一列
    ui->label->setText("经度：");
    ui->label_2->setText("纬度：");
    ui->label_3->setText("时间:");
    ui->label_4->setText("速度：");
    ui->label_5->setText("状态：");
    ui->label_6->setText("方位角：");
    ui->label_7->setText("磁偏角:");
    //第二列
    ui->label_8->setText(lon+"°  "+rmc.lon_direct);
    ui->label_9->setText(lat+"°  "+rmc.lat_direct);
    ui->label_10->setText(time_);
    ui->label_11->setText(QString::number(rmc.speed)+" km/h");
    ui->label_12->setText(status);
    ui->label_13->setText(QString::number(rmc.cog)+"°  ");
    ui->label_14->setText(QString::number(rmc.mag_variation)+"°  "+rmc.mag_var_direct);

    setWindowFlags(Qt::WindowCloseButtonHint|Qt::MSWindowsFixedSizeDialogHint); //只显示关闭

    show();
}

void Dialog::on_Lastpage_clicked()
{
    //char* string="lastpage";
    //emit sendstring(string);
    if(lenth == 0) //处于围栏记录模式
    {
        if(fence_now<6)
        {
            showFenceLog(FENCE);
            QMessageBox::warning(this, tr("警告"), tr("到头了"));
        }
        else
        {
            fence_now = fence_now-6;
            showFenceLog(FENCE+fence_now);
        }
    }
    else            //处于nmea记录模式
    {
        if(now == 0)
          {
            showNmealog(GPS_POINTS[0]);
            QMessageBox::warning(this, tr("警告"), tr("到头了"));
           }
         else
        {
             now = now-1;
             showNmealog(GPS_POINTS[now]);
        }
    }
}

void Dialog::on_Nextpage_clicked()
{
    //char* string="nextpage";
    //emit sendstring(string);
    if(lenth == 0)  //处于围栏显示模式
    {
        if((fencelenth-fence_now)<6)
        {
            showFenceLog(FENCE+fence_now);
            QMessageBox::warning(this, tr("警告"), tr("到底了"));
        }
        else
        {
            fence_now = fence_now+6;
            showFenceLog(FENCE+fence_now);
        }
    }
    else
    {
        if(now == lenth-1)
        {
            showNmealog(GPS_POINTS[lenth-1]);
            QMessageBox::warning(this, tr("警告"), tr("到底了"));
        }
        else
        {
            now = now+1;
            showNmealog(GPS_POINTS[now]);
        }
    }


}
