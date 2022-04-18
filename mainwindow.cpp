#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <QThread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->openFile, SIGNAL(clicked()), this, SLOT(open_file_dialog()));
    connect(ui->play, SIGNAL(clicked()), this, SLOT(play_file()));
    connect(ui->stop, SIGNAL(clicked()), this, SLOT(stop_file()));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::open_file_dialog() {

    filename = QFileDialog::getOpenFileName(this,
                                            tr("Open wave file"),
                                            "/home/roz3x/Desktop/project/sample-songs/raws",
                                            tr("image files")
                                           );
    QFileInfo f(filename);
    ui->openFileName->setText(f.fileName());

}

void MainWindow::stop_file()
{
    snd_pcm_pause(pcm_handle,0 );
    snd_pcm_drop(pcm_handle);
}


void play_song(Ui::MainWindow *ui, snd_pcm_t *pcm_handle, int channels, unsigned int rate , FILE* fd) {
    const char* PCM_DEVICE = "default";
    int pcm ;
    unsigned int tmp;
    ui->logs->append("playing sound now...\n");
    ui->logs->append("rate: "+QString::number(rate) + "\n");

    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;

    char *buff;
    int buff_size;

    pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0  );
    if (pcm < 0) {
        ui->logs->append("cant open pcm device\n");
        return;
    }


    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params,
                                       SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);
    pcm = snd_pcm_hw_params(pcm_handle, params);
    if (pcm < 0) {
        // setup has failed somewhere
        ui->logs->append("errors while setup");
        return ;
    }

    snd_pcm_hw_params_get_rate(params, &tmp, 0);
    snd_pcm_hw_params_get_period_size(params, &frames, 0);
    buff_size = frames * channels * 2;
    buff = (char*) malloc(buff_size);

    snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
    while ( true ) {
        pcm = read(fileno(fd),buff, buff_size);
        if (pcm == 0) {
            ui->logs->append("complete file is pushed \n");
            snd_pcm_drain(pcm_handle);
            snd_pcm_hw_free(pcm_handle);
            free(buff);
            return ;
        }
        if ((pcm =snd_pcm_writei(pcm_handle, buff, frames))==-EPIPE) {
            snd_pcm_prepare(pcm_handle);
        } else if (pcm < 0) {
            // this event might correspond to our pausing of the song form ui.
            // log error snd_strerror(pcm);
            ui->logs->append(QString::fromLocal8Bit(snd_strerror(pcm),-1));
            return ;
        }
    }
    assert(false);

}
void MainWindow::play_file() {
    FILE* f = fopen(filename.toStdString().c_str(), "rb");
    thread = QThread::create(play_song , ui, pcm_handle , 1, 44100 , f);
    thread->start();

}

