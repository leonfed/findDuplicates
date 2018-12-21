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
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionBrowse_triggered() {
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

void MainWindow::fillListFiles() {
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
    duplicates &dup = duplicates::instance();
    ui->listDuplicates->clear();
    for (auto p : dup.sameFiles[index]) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/file"), QString::fromStdString(p));
        ui->listDuplicates->addItem(item);
    }
}

void MainWindow::on_actionRun_triggered() {
    std::get<0>(indexSelectedItem) = false;
    std::string directory = ui->directory->text().toStdString();
    if (directory != "" && directory != "Directory isn't selected") {
        duplicates &dup = duplicates::instance();
        dup.setDirectory(directory);
        fillListFiles();
    }
}

void MainWindow::on_listFiles_itemPressed(QListWidgetItem *item) {
    std::get<0>(indexSelectedItem) = false;
    duplicates &dup = duplicates::instance();
    std::string file = item->text().toStdString();
    fillListDuplicates(dup.indexInSameFiles[file].first);
}

void MainWindow::on_listDuplicates_itemPressed(QListWidgetItem *item) {
    duplicates &dup = duplicates::instance();
    std::string file = item->text().toStdString();
    indexSelectedItem = { true, dup.indexInSameFiles[file].first, dup.indexInSameFiles[file].second };
}

void MainWindow::on_actionDelete_file_triggered() {
    if (!std::get<0>(indexSelectedItem)) {
        return;
    }
    duplicates &dup = duplicates::instance();
    size_t i = std::get<1>(indexSelectedItem), j = std::get<2>(indexSelectedItem);
    std::string file = dup.sameFiles[i][j];
    fs::remove(file);
    dup.sameFiles[i].erase(dup.sameFiles[i].begin() + j);
    if (dup.sameFiles[i].size() == 1) {
        dup.sameFiles[i].erase(dup.sameFiles[i].begin());
    }
    fillListFiles();
    fillListDuplicates(i);
}

void MainWindow::on_actionOpen_directory_triggered() {
    if (!std::get<0>(indexSelectedItem)) {
        return;
    }
    duplicates &dup = duplicates::instance();
    size_t i = std::get<1>(indexSelectedItem), j = std::get<2>(indexSelectedItem);
    std::string file = dup.sameFiles[i][j];
    fs::path pathFile(file);
    fs::path fileDirectory = pathFile.parent_path();
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(fileDirectory)));
}
