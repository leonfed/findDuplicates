#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <duplicates.h>
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionBrowse_triggered()
{
    QString str;
    try {
        str = QFileDialog::getExistingDirectory(nullptr, "Directory Dialog", "");
        if (str != "") {
            ui->directory->setText(str);
        }
       ui->listFiles->clear();
       ui->listDuplicates->clear();
    } catch (...) {
        ui->directory->setText("Directory isn't selected");
    }
}

void MainWindow::on_actionRun_triggered()
{
    std::string directory = ui->directory->text().toStdString();
    if (directory != "" && directory != "Directory isn't selected") {
        duplicates &dup = duplicates::instance();
        dup.setDirectory(directory);
        ui->listFiles->clear();
        ui->listDuplicates->clear();
        for (auto v : dup.sameFiles) {
            for (auto p : v) {
                QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/file"), QString::fromStdString(p));
                ui->listFiles->addItem(item);
            }
        }
    }
}


void MainWindow::on_listFiles_itemPressed(QListWidgetItem *item)
{
    duplicates &dup = duplicates::instance();
    ui->listDuplicates->clear();
    std::string file = item->text().toStdString();
    for (auto p : dup.sameFiles[dup.indexInSameFiles[file]]) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/file"), QString::fromStdString(p));
        ui->listDuplicates->addItem(item);
    }
}
