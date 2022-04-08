#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>

struct  __wh {
    char                riff[4];        // RIFF Header      Magic header
    unsigned long       chunkSize;      // RIFF Chunk Size
    char                wave[4];        // WAVE Header
    char                fmt[4];         // FMT header
    unsigned long       subChunk1Size;  // Size of the fmt chunk
    unsigned short      audioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    unsigned short      numOfChannel;   // Number of channels 1=Mono 2=Sterio
    unsigned long       samplesPerSec;  // Sampling Frequency in Hz
    unsigned long       bytesPerSec;    // bytes per second
    unsigned short      blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    unsigned short      bitsPerSample;  // Number of bits per sample
    char                subChunk2Id[4]; // "data"  string
    unsigned long       subChunk2Size;  // Sampled data length

} wave_header;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->openFile, SIGNAL(clicked()), this, SLOT(open_file_dialog()));
    connect(ui->play, SIGNAL(clicked()), this, SLOT(play_file()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_file_dialog() {

    filename = QFileDialog::getOpenFileName(this,
                                            tr("Open wave file"),
                                            "/home/roz3x/Desktop/project",
                                            tr("image files")
                                           );
    QFileInfo f(filename);
    ui->openFileName->setText(f.fileName());

}

void MainWindow::play_file() {
    FILE* f = fopen(filename.toStdString().c_str(), "rb");
    if (f == 0x00) {
        ui->logs->append("cant open file "+filename);
        return;
    }
    ui->logs->setText("file is open...\n");

    // parseing the wave header
    // compute the file size first

    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    ui->logs->append("file has size: " + QString::number(filesize) + "\n");

    if (filesize < 2048){
        ui->logs->append("filesize if too low");
        return;
    }
    fseek(f, 0,SEEK_SET);
    fread(&wave_header, sizeof(wave_header), 1, f);
    long sizeInAudio = filesize - sizeof(wave_header);
    long sampleSize = wave_header.bitsPerSample/8;

    ui->logs->append("audio samples :"
                     +QString::number(sizeInAudio/sampleSize)
                     + "\n" );
    ui->logs->append("left off sample :"
                     +QString::number(sizeInAudio%sampleSize)
                     + "\n" );


    return ;
}
