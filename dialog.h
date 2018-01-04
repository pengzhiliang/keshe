#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include "gprmc.h"
#include "my_struct.h"

static gprmc_format* GPS_POINTS;
static int lenth;
static int now;

static ponit_format* FENCE;
static int fencelenth;
static int fence_now;

namespace Ui {
class Dialog;
}


class Dialog : public QDialog
{
    Q_OBJECT


public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
//signals:
    //void sendstring(char* string);                       //发出信号，以便mainwindow捕获到//还是内部处理，不传回
public slots:

    //void showInfenceLog( ponit_format log[],int length);//显示再围栏内记录

    //void showInfenceLog();

    //void showOutfenceLog(ponit_format log[],int length);//显示再围栏外记录

    //void showOutfenceLog();

    void NmeaLog(gprmc_format rmc[],int lenth_);

    void FenceLog(ponit_format log[],int lenth_);

    void showNmealog(gprmc_format rmc);

    void showFenceLog(ponit_format* log);

    void ClearLog();    //清除记录

private slots:

    void on_Lastpage_clicked();                 //按了上一页

    void on_Nextpage_clicked();                 //按了下一页

private:
    Ui::Dialog *ui;
};


#endif // DIALOG_H
