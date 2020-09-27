#include "mainwindow.h"
#include "ui_mainwindow.h"

#define  SIMPLE
#define  MAX_BLOCK_TIME          (6000)
#define  TIMER_INTERRUPT_TIME     (300)
#define  MIN_PROGRESSBAR_VALUE    (0)
#define  MAX_PROGRESSBAR_VALUE    (100)
#define  INIT_PROGRESSBAR_VALUE   MIN_PROGRESSBAR_VALUE
/*
  * @brief timer 中断处理函数
  * @param  None
  * @retval None
*/
void MainWindow::timer_update()
{
    FindSerial();
#ifndef SIMPLE
    QR_DOWNLOAD_STATUS  current_status;
    current_status=Get_current_status();
    if((current_status!=QR_DOWNLOAD_SUCCESS)&&(current_status==last_qr_down_status))
    {
      status_block_time++;
      if(status_block_time>(MAX_BLOCK_TIME/TIMER_INTERRUPT_TIME))
      {
        Stop_execute_cmd();
      }
    }
    else
    {
        last_qr_down_status=current_status;
        status_block_time=0;
    }
#endif
}

void MainWindow::Stop_execute_cmd()
{
  Trans_to_status(QR_DOWNLOAD_ERROR);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    Init_ui();
    Init_timer();
    Init_call_process();
}

//execute process
void  MainWindow::Init_call_process()
{
    m_Process=new QProcess();
    m_Process->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_Process,SIGNAL(readyReadStandardOutput()),this,SLOT(onOutput()));
}

//init assist  timer
void  MainWindow::Init_timer()
{
    timer = new QTimer(this);
    timer->start(TIMER_INTERRUPT_TIME);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_update()));
}

//init the testedit ui
void MainWindow::Init_ui_lable()
{
  ui->label->setText("接收二维码");
  ui->label_2->setText("主文件烧录进度");
}

//init the testedit ui
void MainWindow::Init_ui_textedit()
{
  ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
}

//init  progressbar ui
void MainWindow::Init_ui_progressbar()
{
    ui->progressBar->setRange(MIN_PROGRESSBAR_VALUE,MAX_PROGRESSBAR_VALUE); //设置进度条最小值和最大值(取值范围)
    ui->progressBar->setMinimum(MIN_PROGRESSBAR_VALUE); //设置进度条最小值
    ui->progressBar->setMaximum(MAX_PROGRESSBAR_VALUE); //设置进度条最大值
    ui->progressBar->setValue(INIT_PROGRESSBAR_VALUE);  //设置当前的运行值
}

//init  window ui
void MainWindow::Init_ui_window()
{
    ui->setupUi(this);
    this->showMaximized();
    this->setWindowFlags(windowFlags() &~ Qt::WindowMaximizeButtonHint);
    setWindowTitle("二维码烧录");
}

//init  the ui
void MainWindow::Init_ui()
{
    Init_ui_window();
    Init_ui_progressbar();
    Init_ui_lable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::QR_DOWN_FMS_RUN(QByteArray log)
{
    switch (qr_down_status)
    {
        case QR_DOWNLOAD_START:
        {
            if(log.contains("Connecting"))
            {
                Trans_to_status(QR_DOWNLOAD_CONNECT_SERIAL);
            }
        }
        break;
        case QR_DOWNLOAD_CONNECT_SERIAL:
        {
            if(log.contains("MAC: ")&&log.contains("Configuring flash size"))
            {
               Trans_to_status(QR_DOWNLOAD_MAIN_FILE);
            }
        }
        break;
        case QR_DOWNLOAD_MAIN_FILE:
        {
            //log = m_Process->readAllStandardOutput();
            int   progress;
            int   start_index,size,value;
            if(log.contains('%')&&log.contains('('))
            {
                size=log.size();
                start_index=log.indexOf('(');
                start_index+=1;
                progress=0;
                while(start_index<size)
                {
                   value=log[start_index];
                   if(value>='0'&&value<='9'&&value!='%')
                   {
                       progress=progress*10+(value-'0');
                       start_index+=1;
                   }
                   else
                   {
                      break;
                   }
                }
                if(progress>0&&progress<=100)
                {
                   ui->progressBar->setValue(progress);
                }
                if(progress==100)
                {
                    Trans_to_status(QR_DOWNLOAD_SIGNATURE);
                }
                log.clear();
            }
        }
        break;
        case QR_DOWNLOAD_SIGNATURE:
        {
            Trans_to_status(QR_DOWNLOAD_SUCCESS);
        }
        break;
        case QR_DOWNLOAD_ERROR:
        {
           Stop_execute_cmd();
        }
        break;
        default:
        break;
    }




}

QR_DOWNLOAD_STATUS MainWindow::Get_current_status()
{
    return qr_down_status;
}
void MainWindow::Trans_to_status(QR_DOWNLOAD_STATUS new_status)
{
    qr_down_status=new_status;
    log.clear();
    switch (new_status)
    {
        case QR_DOWNLOAD_START:
        {
           //clear the log
        }
        break;
        case QR_DOWNLOAD_CONNECT_SERIAL:
        {


        }
        break;
        case QR_DOWNLOAD_SUCCESS:
        {
          QMessageBox::about(NULL, "提示", "SN烧录成功");
        }
        break;
        case QR_DOWNLOAD_ERROR:
        {
            m_Process->kill();
            QMessageBox::about(NULL, "提示", "SN烧录失败");
        }
        break;
        default:
        break;
    }
}
void MainWindow::onOutput()
{
#ifdef  SIMPLE
    QByteArray qbt = m_Process->readAllStandardOutput();
    int   progress;
    int   start_index,size,value;
    if(qbt.contains('%')&&qbt.contains('('))
    {
//      setWindowTitle("ok");
        size=qbt.size();
        start_index=qbt.indexOf('(');
        start_index+=1;
        progress=0;
        while(start_index<size)
        {
           value=qbt[start_index];
           if(value>='0'&&value<='9'&&value!='%')
           {
               progress=progress*10+(value-'0');
               start_index+=1;
           }
           else
           {
              break;
           }
        }
        if(progress>0&&progress<=100)
        {
           ui->progressBar->setValue(progress);
        }
        //progress=
       //
    }
    QString msg = QString::fromLocal8Bit(qbt);
    ui->textEdit->insertPlainText(msg);
    QTextCursor cursor=ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
    //ui->textEdit->update();
#else
    log.append(m_Process->readAllStandardOutput());
    QR_DOWN_FMS_RUN(log);
# if 0
    int   progress;
    int   start_index,size,value;
    if(qbt.contains('%')&&qbt.contains('('))
    {
//      setWindowTitle("ok");
        size=qbt.size();
        start_index=qbt.indexOf('(');
        start_index+=1;
        progress=0;
        while(start_index<size)
        {
           value=qbt[start_index];
           if(value>='0'&&value<='9'&&value!='%')
           {
               progress=progress*10+(value-'0');
               start_index+=1;
           }
           else
           {
              break;
           }
        }
        if(progress>0&&progress<=100)
        {
           ui->progressBar->setValue(progress);
        }
        //progress=
       //
    }
    QString msg = QString::fromLocal8Bit(qbt);
    ui->textEdit->insertPlainText(msg);
    QTextCursor cursor=ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
    //ui->textEdit->update();
#endif
#endif
}
/*
  * @brief  查找串口号并放入QComboBox中
  * @param  None
  * @retval None
*/
void MainWindow::FindSerial()
{
    QStringList strList;
    QString    current_serial_name;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        //如果成功，则返回true;否则返回false
        if(serial.open(QIODevice::ReadWrite))
        {
            strList.append(serial.portName());
            serial.close();
        }
    }
    current_serial_name=ui->comboBox->currentText();
    ui->comboBox->clear();
    ui->comboBox->addItems(strList);
    if(strList.contains(current_serial_name))
    {
       ui->comboBox->setCurrentText(current_serial_name);
    }
}
int MainWindow:: Executed_batch()
{
    QString strInfo;
    QStringList args;
    QString curPath = QDir::currentPath();//获取当前应用程序目录
    QString toolpath;
    toolpath = "D:/Andrew/AWS_Module/tools/AWS_Certificate_Generate/test.bat";    // 当前运行路径下的文件夹（其中是需要调用的.bat
    curPath= "D:/Andrew/AWS_Module/tools/AWS_Certificate_Generate";
    m_Process->setWorkingDirectory(curPath); //设置工作目录
    // toolpath = "/make.bat";  //要调用的脚本文件
    // curPath.append(toolpath); //添加脚本文件字符串
    Trans_to_status(QR_DOWNLOAD_START);
    last_qr_down_status=Get_current_status();
    status_block_time=0;
    args.clear();
    //args.append("COM8");
    args.append(ui->comboBox->currentText());
    args.append(ui->textEdit_qr->toPlainText());
    m_Process->start(toolpath,args);  //运行脚本文件
//    if(m_Process->waitForFinished())
//    {      //等待脚本运行完成，超时时间默认是3000s,超时返回0，正常返回1
//       strInfo = "完成！";
//    }
//    else
//    {
//       strInfo = "bat运行错误！";
//    }
//    ui->textEdit->append(strInfo);
  //  qDebug() << process.errorString();
    return 1;

}

void MainWindow::on_pushButton_clicked()
{

   Executed_batch();
   // TEST();
    //executeCmd();
}
