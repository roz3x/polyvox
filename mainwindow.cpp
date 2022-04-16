#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "alsa-play.h"
#include <stdlib.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>


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
    ui->logs->append("playing the file ... \n");
    FILE* f = fopen(filename.toStdString().c_str(), "rb");
    setup_and_play(ui, f, 22500, 2);
    return ;
}
