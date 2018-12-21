#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <duplicates.h>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_actionBrowse_triggered();

    void on_actionRun_triggered();

    void on_listFiles_itemPressed(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
