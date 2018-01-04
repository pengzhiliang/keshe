#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextCodec>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QDebug>
#include <QTextEdit>
#include <QFileDialog>
#include <QTextBrowser>
#include <QTextEdit>
#include <QWidget>
#include <QWebView>
#include <QtWebKit>
#include <QWebFrame>
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include "dialog.h"
#include "gprmc.h"
#include "my_struct.h"


//定义全局变量，点的经纬度
//static QString lat_;
//static QString lon_;
//static double lat_[300];
//static double lon_[300];
static gprmc_format GPSPOINTS[300];     //用结构体数组存更多的信息
static int gpspoints;                   //计数，已使用结构体

static QJsonArray lat_json;             //传轨迹时使用
static QJsonArray lon_json;

static ponit_format InfenceCoordinate[300];   //记录进入和离开围栏的坐标
static ponit_format OutfenceCoordinate[300];

static int infencenumber;               //记录上述个数
static int outfencenumber;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Dialog *mdialog;
public slots:
    void getCoordinate(QString lon,QString lat);
    
    void showinfo(int num,QString info);

    void showwarning(int num,QString info);

    void setInfenceCoordinate(QString lon,QString lat);

    void setOutfenceCoordinate(QString lon,QString lat);


private slots:
    void clear_gpspoint_jsonarray(bool emptygpspoint);
    
    void slotPopulateJavaScriptWindowObject();
       
    void on_actionStreet_triggered();

    void on_actionHybrid_triggered();

    void on_locate_clicked();

    void on_trace_clicked();

    void on_actionClear_triggered();

    void on_actionStart_triggered();

    void on_actionShow_triggered();

    void on_actionShow_2_triggered();

    void on_actionTrace_triggered();

    void on_parse_clicked();

    void on_Init_clicked();

    void on_actionInfence_triggered();

    void on_actionOutfence_triggered();

    void on_actionClearLog_triggered();

    void on_actionNmea_triggered();



private:
    Ui::MainWindow *ui;
    QTimer *timer;
    void setCoordinate(QString lon,QString lat);
};
/*
class locationstatus
{
public:
    static QString lat_;
    static QString lon_;
};
*/
#endif // MAINWINDOW_H
