#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <duplicates.h>
#include <QListWidgetItem>
#include <tuple>

namespace Ui {
    class MainWindow;
}

static std::tuple<bool, size_t, size_t> indexUp = {false, 0, 0};
static std::tuple<bool, size_t, size_t> indexDown = {false, 0, 0};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void fillListFiles();
    void fillListDuplicates(size_t index);

private slots:

    void on_actionBrowse_triggered();

    void on_actionFind_triggered();

    void on_listFiles_itemPressed(QListWidgetItem *item);

    void on_listDuplicates_itemPressed(QListWidgetItem *item);

    void on_actionDelete_file_triggered();

    void on_actionOpen_directory_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
