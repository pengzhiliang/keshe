#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "china_shift.h"
#include "dialog.h"
#include "ui_dialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("基于GPS的位置追踪及地理围栏"));               //名称
    setWindowIcon(QIcon(":/new/prefix1/photo.jpg"));
    QString strMapPath="file:///";                              //exe文件所在的绝对路径
    strMapPath+=QCoreApplication::applicationDirPath();
    strMapPath+="../../pro2/maponline.html";    //D:\Qt\MyProject\TestMap\build-TestMap-Desktop_Qt_5_5_1_MinGW_32bit-Debug\debug
    QUrl url(strMapPath);
    ui->webView->load(url);     //打开本地的.html文件
    ui->webView->setTextSizeMultiplier(1);//设置网页字体大小

    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(slotPopulateJavaScriptWindowObject()));  //将Qt与js联系起来
    //收到mainFrame()发出的SIGNAL后，this调用slotPopulateJavaScriptWindowObject

    //QT与JavaScript互调是通过QWebFrame的两个函数来实现的:
    //addToJavaScriptWindowObject()将QObject对象传给JS，这样JS就能调用QObject的public slots函数
    //QT通过evaluateJavaScript()直接调用JS中的函数

    statusBar() ;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_parse_clicked()//解析
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                "../TESTDATA/",
                                                tr("Text Files(*.txt)"));
    if(!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Read File"),
                                 tr("Cannot open file:\n%1").arg(path));
            return;
        }

        clear_gpspoint_jsonarray(false);

        path.replace("/","\\\\");

        FILE* input = fopen(path.toLatin1().data(),"rt+");
        char line[BUFSIZ]; // 读取一行 GPRMC
        gprmc_format rmc;  // 存储 GPRMC
        Location gps_point,gpspoint;  // 84(地球)<-->02(火星)
        bool read_rmc = false;

        //地球坐标84 --> 火星坐标02 -->百度坐标 09
        while (fgets(line, BUFSIZ, input)) {
                read_rmc = read_gprmc(line, rmc);
                //qDebug()<<read_rmc;
                if (read_rmc) {

                    GPSPOINTS[gpspoints] = rmc ;                    //存入，之后改变需要改变的量即可

                    rmc.lat = degree_minute2dec_degrees(rmc.lat);   //84 读地球坐标
                    rmc.lon = degree_minute2dec_degrees(rmc.lon);
                    gps_point.lat = rmc.lat ;
                    gps_point.lng = rmc.lon ;
                    gpspoint=transformFromWGSToGCJ(gps_point);

                    gps_point=bd_encrypt(gpspoint);        //02-> BD09

                    //lat_[gpspoints] = gps_point.lat;   //BD-09
                    //lon_[gpspoints] = gps_point.lng;

                    GPSPOINTS[gpspoints].lat = gps_point.lat;               //经纬度
                    GPSPOINTS[gpspoints].lon = gps_point.lng;

                    //插入到json对象当中

                    lat_json.append(GPSPOINTS[gpspoints].lat);              //传入json
                    lon_json.append(GPSPOINTS[gpspoints].lon);

                    QString lat =  QString::number(GPSPOINTS[gpspoints].lat);//"lat:\t" + + "\n"; //数字转换成Qstring
                    QString lon =  QString::number(GPSPOINTS[gpspoints].lon);//"lon:\t" + + "\n";

                    double nm_to_km = 1.852;    //用84
                    GPSPOINTS[gpspoints].speed = rmc.speed * nm_to_km;      //速度
                    QString speed_=  QString::number(GPSPOINTS[gpspoints].speed);//"speed:\t" + + "\n";

                    char time_str [80] = "";
                    int date = rmc.date;
                    //GPSPOINTS[gpspoints].date = rmc.date;                       //日期

                    int time = (int)rmc.rcv_time;
                    //GPSPOINTS[gpspoints].rcv_time = rmc.rcv_time;               //时间

                    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d",
                            date / 10000, (date % 10000) / 100,  date % 100,
                            time / 10000, (time % 10000) / 100,  time % 100);
                    QString time_ = time_str;

                    //gps_result.append ( lat_ + lon_ + speed_ + "time:\t" + time_str + "\n");
                    ui->Longitude->setText("经度:"+ lon+"°  "+rmc.lon_direct);
                    ui->Latitude->setText("纬度:"+  lat+"°  "+rmc.lat_direct);
                    ui->Speed->setText("速度:"+speed_ +"km/h");
                    ui->Time->setText("时间："+ time_);

                    gpspoints = gpspoints+1;        //jia 1
                }

            }
        file.close();
        if(0 == gpspoints)
        {
            QMessageBox::warning(this, tr("警告"), tr("非法数据"));
        }
    } else {
        QMessageBox::warning(this, tr("警告"), tr("您未选择任何文件"));
    }

}

/*
void MainWindow::on_StreetMap_clicked()      //点击“街道图”后执行显示街道图并且标记（lon_，lat_）
{

    QWebFrame *frame = ui->webView->page()->mainFrame();
    QString cmd = QString("showStreetMap()");
    frame->evaluateJavaScript(cmd);         //调用"showStreetMap()"对应的函数

    //qDebug()<<lat_;
    //setCoordinate(QString::number(114.4216315015077),QString::number(30.514952358373492));
    setCoordinate( lon_,lat_);              //并且在指定的经纬度处加上标记
}


void MainWindow::on_SatelliteMap_clicked()  //功能同上
{


    QWebFrame *frame = ui->webView->page()->mainFrame();
    QString cmd = QString("showSatelliteMap()");
    frame->evaluateJavaScript(cmd);

    setCoordinate( lon_,lat_);
}
*/
void MainWindow::slotPopulateJavaScriptWindowObject()
{
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("ReinforcePC", this);
    //addToJavaScriptWindowObject()将QObject对象传给JS，这样JS就能调用QObject的public slots函数
    //addToJavaScriptWindowObject的第一个参数是对象在javascript里的名字， 可以自由命名， 第二个参数是对应的QObject实例指针。
}

void MainWindow::setCoordinate(QString lon,QString lat) //在指定的经纬度处加上标记
{
    QWebFrame *webFrame = ui->webView->page()->mainFrame();
    QString cmd = QString("showAddress(\"%1\",\"%2\")").arg(lon).arg(lat);
    webFrame->evaluateJavaScript(cmd);
}

void MainWindow::getCoordinate(QString lon,QString lat) //显示已经得到的经纬度
{

    ui->Longitude->setText("经度:"+lon+"°");
    ui->Latitude->setText("纬度:"+lat+"°");
}

void MainWindow::on_actionStreet_triggered()
{
    QWebFrame *frame = ui->webView->page()->mainFrame();
    QString cmd = QString("showStreetMap()");
    frame->evaluateJavaScript(cmd);

   // setCoordinate( lon_,lat_);
}

void MainWindow::on_actionHybrid_triggered()
{
    QWebFrame *frame = ui->webView->page()->mainFrame();
    QString cmd = QString("showSatelliteMap()");
    frame->evaluateJavaScript(cmd);         //调用"showStreetMap()"对应的函数

    //qDebug()<<lat_;
    //setCoordinate(QString::number(114.4216315015077),QString::number(30.514952358373492));
    //setCoordinate( lon_,lat_);              //并且在指定的经纬度处加上标记
}

void MainWindow::on_locate_clicked()
{
    if(0 == gpspoints){

        if(QMessageBox::warning(this,tr("警告"),tr("请打开nmea文档进行解析，或者打开围栏用鼠标绘制点"),QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
                on_parse_clicked();
                QWebFrame *webFrame = ui->webView->page()->mainFrame();
                QString cmd = QString("showlocation(\"%1\",\"%2\")").arg(QString::number(GPSPOINTS[gpspoints-1].lon)).arg(QString::number(GPSPOINTS[gpspoints-1].lat));
                webFrame->evaluateJavaScript(cmd);
        }
        else
        {
            return;
        }
        //qDebug()<< "请打开nmea文档进行解析";
    }
    else{
        //如果只是一个点，则显示当前点的位置，如果是多点，则显示最近一点的
        QWebFrame *webFrame = ui->webView->page()->mainFrame();
        QString cmd = QString("showlocation(\"%1\",\"%2\")").arg(QString::number(GPSPOINTS[gpspoints-1].lon)).arg(QString::number(GPSPOINTS[gpspoints-1].lat));
        webFrame->evaluateJavaScript(cmd);
    }

}

void MainWindow::on_trace_clicked()
{
/*    if(!lat_json.isEmpty()) //如果非空，则清空jsonarray
    {
        clear_gpspoint_jsonarray(false);
        on_parse_clicked();
        QJsonDocument lon_document,lat_document;    //将QJsonArray改为QJsonDocument类
        lon_document.setArray(lon_json);
        lat_document.setArray(lat_json);
        QByteArray lon_byteArray = lon_document.toJson(QJsonDocument::Compact);
        QByteArray lat_byteArray = lat_document.toJson(QJsonDocument::Compact);
        QString lonJson(lon_byteArray);             //再转为QString
        QString latJson(lat_byteArray);
        //qDebug() << lonJson;
        QWebFrame *webFrame = ui->webView->page()->mainFrame();
        QString cmd = QString("showtrace(\"%1\",\"%2\")").arg(lonJson).arg(latJson);
        webFrame->evaluateJavaScript(cmd);          //传给javascript
    }*/
    //qDebug()<<gpspoints;
   // else{
    if(0 == gpspoints){
        //QMessageBox::warning(this, tr("警告"),tr("请打开nmea文档进行解析"));
        if(QMessageBox::information(this,tr("提示"),tr("请打开nmea文档进行解析"),QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
                on_parse_clicked();
                QJsonDocument lon_document,lat_document;    //将QJsonArray改为QJsonDocument类
                lon_document.setArray(lon_json);
                lat_document.setArray(lat_json);
                QByteArray lon_byteArray = lon_document.toJson(QJsonDocument::Compact);
                QByteArray lat_byteArray = lat_document.toJson(QJsonDocument::Compact);
                QString lonJson(lon_byteArray);             //再转为QString
                QString latJson(lat_byteArray);
                //qDebug() << lonJson;
                QWebFrame *webFrame = ui->webView->page()->mainFrame();
                QString cmd = QString("showtrace(\"%1\",\"%2\")").arg(lonJson).arg(latJson);
                webFrame->evaluateJavaScript(cmd);          //传给javascript
        }
        else
        {
            return;
        }
        //qDebug()<< "请打开nmea文档进行解析";
    }
    else if(1 == gpspoints)
    {
        QMessageBox::warning(this, tr("警告"),tr("只有一个点无法绘制出轨迹，请选择单点定位"));
    }
    else{                      //多点就传数组，显示轨迹

        QJsonDocument lon_document,lat_document;    //将QJsonArray改为QJsonDocument类
        lon_document.setArray(lon_json);
        lat_document.setArray(lat_json);
        QByteArray lon_byteArray = lon_document.toJson(QJsonDocument::Compact);
        QByteArray lat_byteArray = lat_document.toJson(QJsonDocument::Compact);
        QString lonJson(lon_byteArray);             //再转为QString
        QString latJson(lat_byteArray);
        //qDebug() << lonJson;
        QWebFrame *webFrame = ui->webView->page()->mainFrame();
        QString cmd = QString("showtrace(\"%1\",\"%2\")").arg(lonJson).arg(latJson);
        webFrame->evaluateJavaScript(cmd);          //传给javascript
    }
  //  }
}

void MainWindow::on_actionStart_triggered()     //围栏->开启，打开鼠标绘制栏，并可以绘制围栏
{
    QWebFrame *webFrame = ui->webView->page()->mainFrame();
    QString cmd = QString("startfence()");
    webFrame->evaluateJavaScript(cmd);
}

void MainWindow::on_actionClear_triggered()     //围栏->清除（只能清除围栏，也就是覆盖物）
{
    QWebFrame *webFrame = ui->webView->page()->mainFrame();
    QString cmd = QString("clearAll()");
    webFrame->evaluateJavaScript(cmd);
}


void MainWindow::showinfo(int num,QString info)//QString info)         //消息框，可以接收js返回消息
{
    //QByteArray ba = info.toLatin1();
    //QMessageBox::information(this,tr("消息"),tr(ba.data()));//QString --> char*
    if(num == 1)
    {
       info = QString("在圆形围栏中，且围栏")+info;
       QMessageBox::information(this,tr("消息"),info);
    }
    if(num == 2)
    {
       info = QString("在多边形围栏中，且围栏")+info;
       QMessageBox::information(this,tr("消息"),info);
    }
    if(num == 3)
    {
        info = QString("在圆形围栏外，且围栏")+info;
        QMessageBox::information(this,tr("消息"),info);
    }
    if(num == 4)
    {
       info = QString("在多边形围栏外，且围栏")+info;
       QMessageBox::information(this,tr("消息"),info);
    }
    //QMessageBox::information(this,tr("消息"),tr(ba.data()));    //QString --> char*
    //ui->Time->setText(info);
}

void MainWindow::showwarning(int num,QString info)      //警告框，可以接收js返回消息
{
    //QByteArray ba = warn.toLatin1();
    //QMessageBox::warning(this,tr("警告"),tr(ba.data()));    //QString --> char*
    //ui->Time->setText(info);
    if(num == 1)
    {
        info = QString("您已进入地理围栏内")+info;
        QMessageBox::warning(this,tr("警告"),info);
    }
    if(num == 2)
    {
        info = QString("您已离开地理围栏")+info;
        QMessageBox::warning(this,tr("警告"),info);
    }
    if(num == 3)
    {
        info = QString("请创建地理围栏")+info;
        QMessageBox::warning(this,tr("警告"),info);
    }
    if(num == 4)
    {
        info = QString("请打开NMEA文档解析并定位")+info;
        QMessageBox::warning(this,tr("警告"),info);
    }
}

void MainWindow::on_actionShow_triggered()          //围栏->显示
{
    QWebFrame *webFrame = ui->webView->page()->mainFrame();
    QString cmd = QString("showinfo()");
    webFrame->evaluateJavaScript(cmd);
}



void MainWindow::on_actionShow_2_triggered()    //动态->演示
{
    QMessageBox::information(this,tr("提示消息"),tr("在此模式中，可动态地用鼠标添加定位点或者围栏。请先建立围栏，一个或者多个"));
    //QMessageBox::information(this,tr("消息"),tr("请先建立围栏，一个或者多个"));
    on_actionStart_triggered();
    QWebFrame *webFrame = ui->webView->page()->mainFrame();
    QString cmd = QString("dynamicshowset()");
    webFrame->evaluateJavaScript(cmd);
}

void MainWindow::on_actionTrace_triggered()     //动态->轨迹
{
    //QMessageBox::information(this,tr("消息"),tr("在此模式中，需先打开NMEA文档进行解析"),QMessageBox::Yes | QMessageBox::No);
    //on_parse_clicked();
    if(QMessageBox::information(this,tr("消息"),tr("在此模式中，需先打开NMEA文档进行解析"),QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
            on_parse_clicked();
    }
    else
    {
        return;
    }
    //QMessageBox::information(this,tr("提示消息"),tr("请先建立围栏，一个或者多个"));
    //on_actionStart_triggered();
    on_actionStart_triggered();                 //打开围栏绘制

    QJsonDocument lon_document,lat_document;    //将QJsonArray改为QJsonDocument类
    lon_document.setArray(lon_json);
    lat_document.setArray(lat_json);
    QByteArray lon_byteArray = lon_document.toJson(QJsonDocument::Compact);
    QByteArray lat_byteArray = lat_document.toJson(QJsonDocument::Compact);
    QString lonJson(lon_byteArray);             //再转为QString
    QString latJson(lat_byteArray);
    //qDebug() << lonJson;
    QWebFrame *webFrame = ui->webView->page()->mainFrame();
    QString cmd = QString("dynamictrace(\"%1\",\"%2\")").arg(lonJson).arg(latJson);
    webFrame->evaluateJavaScript(cmd);          //传给javascript

}

void MainWindow::on_Init_clicked()  //初始化
{
    clear_gpspoint_jsonarray(true);
    on_actionClearLog_triggered();

    ui->Longitude->setText("经度:");
    ui->Latitude->setText("纬度:");
    ui->Speed->setText("速度:");
    ui->Time->setText("时间：");

    QString strMapPath="file:///";                              //exe文件所在的绝对路径
    strMapPath+=QCoreApplication::applicationDirPath();
    strMapPath+="/maponline.html";    //D:\Qt\MyProject\TestMap\build-TestMap-Desktop_Qt_5_5_1_MinGW_32bit-Debug\debug
    QUrl url(strMapPath);
    ui->webView->load(url);     //打开本地的.html文件
}

void MainWindow::clear_gpspoint_jsonarray(bool emptygpspoint)
{
    if(emptygpspoint)
    {
        gpspoints = 0; //对已存经纬度长度初始化
    }

     int i=0;
     for(i=0;i<200;i++)      //把两个jsonarray清空
     {
         if(!lat_json.isEmpty())
         {
             lat_json.pop_back();
             lon_json.pop_back();
         }
     }
}


void MainWindow::on_actionInfence_triggered()//记录->进入围栏记录
{
    mdialog = new Dialog();
    //mdialog->showInfenceLog(InfenceCoordinate,infencenumber);
    mdialog->FenceLog(InfenceCoordinate,infencenumber);
    mdialog->setWindowTitle(tr("进入围栏记录"));
    //mdialog->setWindowFlags(Qt::WindowCloseButtonHint|Qt::MSWindowsFixedSizeDialogHint);
    //mdialog->setModal(true);
}

void MainWindow::on_actionOutfence_triggered()      //记录->离开围栏记录
{
    mdialog = new Dialog();
    //mdialog->showOutfenceLog(OutfenceCoordinate,outfencenumber);
    mdialog->FenceLog(OutfenceCoordinate,outfencenumber);
    mdialog->setWindowTitle(tr("离开围栏记录"));
    //mdialog->setWindowFlags(Qt::WindowCloseButtonHint|Qt::MSWindowsFixedSizeDialogHint);
    //mdialog->setModal(true);
}

void MainWindow::on_actionClearLog_triggered()     //记录->清除记录
{
    infencenumber=0;
    outfencenumber=0;
    gpspoints =0;
    /*
    int i;
    for(i=0;i<12;i++)
    {
        InfenceCoordinate[i]=QString("");
        OutfenceCoordinate[i]=QString("");
    }
    */
    mdialog = new Dialog();
    mdialog->ClearLog();
}

void MainWindow::setInfenceCoordinate(QString lon,QString lat)  //js传回进入点经纬度
{
    InfenceCoordinate[infencenumber].lon=lon;
    InfenceCoordinate[infencenumber].lat=lat;
    infencenumber = infencenumber+1;

    //qDebug()<<infencenumber;
    //qDebug()<<InfenceCoordinate[infencenumber-1].lon;
}

void MainWindow::setOutfenceCoordinate(QString lon,QString lat) //js传回离开点经纬度
{
    OutfenceCoordinate[outfencenumber].lon=lon;
    OutfenceCoordinate[outfencenumber].lat=lat;
    outfencenumber = outfencenumber+1;
}

void MainWindow::on_actionNmea_triggered()                      //解析记录
{
    mdialog = new Dialog();
    mdialog->NmeaLog(GPSPOINTS,gpspoints);
    mdialog->setWindowTitle(tr("NMEA详细解析记录"));

}
