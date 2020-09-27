#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QProcess>
#include <QDir>
#include <QString>
#include <QMainWindow>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QLineEdit> //单行文本控件类
#include <QPushButton> //按钮类
#include <QFileDialog> //引用文件浏览对话框类

namespace Ui {
class MainWindow;
}
#define LOG_BUFFER_LENGTH   (1024*1024*4)//4M
typedef enum
{
    QR_DOWNLOAD_START,
    QR_DOWNLOAD_CONNECT_SERIAL,
    QR_DOWNLOAD_MAIN_FILE,
    QR_DOWNLOAD_READ_MAC,
    QR_DOWNLOAD_SIGNATURE,
    QR_DOWNLOAD_SUCCESS,
    QR_DOWNLOAD_ERROR,
}QR_DOWNLOAD_STATUS;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     int  Executed_batch();
     void executeCmd();
     void FindSerial();
     void Init_call_process();
     void Init_timer();
     void Init_ui();
     void Init_ui_window();
     void Init_ui_textedit();
     void Init_ui_progressbar();
     void Init_ui_lable();
     void QR_DOWN_FMS_RUN(QByteArray log);
     void Trans_to_status(QR_DOWNLOAD_STATUS new_status);
     void Stop_execute_cmd();
     QR_DOWNLOAD_STATUS Get_current_status();
private slots:
     void on_pushButton_clicked();
     void onOutput();
     void timer_update();
private:
    Ui::MainWindow *ui;
    QProcess    *m_Process;
    QTimer*     timer;//定时刷新界面
    QR_DOWNLOAD_STATUS  qr_down_status,last_qr_down_status;
    QByteArray      log;
    int          status_block_time;
};

#endif // MAINWINDOW_H
