#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QThread>

#include <alsa/asoundlib.h>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QString filename;

    snd_pcm_t *pcm_handle;
    unsigned int rate;
    int channels;
    FILE* fd;

    QThread *thread;


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void play_file();
    void open_file_dialog();
    void stop_file();

};
#endif // MAINWINDOW_H
