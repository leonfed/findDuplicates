#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <duplicates.h>
#include <string>
#include <iostream>
#include <experimental/filesystem>
#include <QDesktopServices>

namespace fs = std::experimental::filesystem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->directory->setStyleSheet("color: rgb(173, 7, 40)");
    ui->error->setStyleSheet("color: rgb(173, 7, 40)");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionBrowse_triggered() {
    ui->error->setText("");
    QString str;
    try {
        str = QFileDialog::getExistingDirectory(nullptr, "Directory Dialog", "");
        if (str != "") {
            ui->directory->setText(str);
            ui->directory->setStyleSheet("color: rgb(25, 166, 44)");
        }
       ui->listFiles->clear();
       ui->listDuplicates->clear();
    } catch (...) {
        ui->directory->setText("Directory isn't selected");
    }
}

void MainWindow::fillListFiles() {
    ui->error->setText("");
    ui->listFiles->clear();
    ui->listDuplicates->clear();
    duplicates &dup = duplicates::instance();
    for (auto v : dup.sameFiles) {
        for (auto p : v) {
            QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/file"), QString::fromStdString(p));
            ui->listFiles->addItem(item);
        }
    }
}

void MainWindow::fillListDuplicates(size_t index) {
    ui->error->setText("");
    duplicates &dup = duplicates::instance();
    ui->listDuplicates->clear();
    for (auto p : dup.sameFiles[index]) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/file"), QString::fromStdString(p));
        ui->listDuplicates->addItem(item);
    }
}

void MainWindow::on_actionRun_triggered() {
    ui->error->setText("");
    std::get<0>(indexUp) = false;
    std::get<0>(indexDown) = false;
    std::string directory = ui->directory->text().toStdString();
    if (directory != "" && directory != "Directory isn't selected") {
        duplicates &dup = duplicates::instance();
        try {
            dup.setDirectory(directory);
        } catch(...) {
            ui->error->setText("Can't find duplicates in this directory");
        }
        fillListFiles();
    }
}

void MainWindow::on_listFiles_itemPressed(QListWidgetItem *item) {
    ui->error->setText("");
    std::get<0>(indexDown) = false;
    duplicates &dup = duplicates::instance();
    std::string file = item->text().toStdString();
    indexUp = { true, dup.indexInSameFiles[file].first, dup.indexInSameFiles[file].second };
    fillListDuplicates(dup.indexInSameFiles[file].first);
}

void MainWindow::on_listDuplicates_itemPressed(QListWidgetItem *item) {
    ui->error->setText("");
    duplicates &dup = duplicates::instance();
    std::string file = item->text().toStdString();
    indexDown = { true, dup.indexInSameFiles[file].first, dup.indexInSameFiles[file].second };
}

void MainWindow::on_actionDelete_file_triggered() {
    ui->error->setText("");
    size_t i = 0, j = 0;
    if (std::get<0>(indexDown)) {
        i = std::get<1>(indexDown);
        j = std::get<2>(indexDown);
    } else if (std::get<0>(indexUp)) {
        i = std::get<1>(indexUp);
        j = std::get<2>(indexUp);
    } else {
        return;
    }
    duplicates &dup = duplicates::instance();
    std::string file = dup.sameFiles[i][j];
    try {
        fs::remove(file);
    } catch(...) {
        ui->error->setText("Can't delete file");
        return;
    }
    dup.sameFiles[i].erase(dup.sameFiles[i].begin() + j);
    if (dup.sameFiles[i].size() == 1) {
        dup.sameFiles[i].erase(dup.sameFiles[i].begin());
    }
    fillListFiles();
    fillListDuplicates(i);
}

void MainWindow::on_actionOpen_directory_triggered() {
    ui->error->setText("");
    size_t i = 0, j = 0;
    if (std::get<0>(indexDown)) {
        i = std::get<1>(indexDown);
        j = std::get<2>(indexDown);
    } else if (std::get<0>(indexUp)) {
        i = std::get<1>(indexUp);
        j = std::get<2>(indexUp);
    } else {
        return;
    }
    duplicates &dup = duplicates::instance();
    std::string file = dup.sameFiles[i][j];
    fs::path pathFile(file);
    try {
        fs::path fileDirectory = pathFile.parent_path();
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(fileDirectory)));
    } catch(...) {
        ui->error->setText("Can't open directory");
    }
}
