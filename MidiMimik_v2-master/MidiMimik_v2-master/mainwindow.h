#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int threshhold;
    QString hihatString;
    QString snareString;
    QString guitarString;
    QString speedString;
    QString bassString;
    int instrumentIndex;
    bool turnUp;
    void refresh();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_checkBox_toggled(bool checked);

    void on_bassButton_clicked();

    void on_hihatButton_clicked();

    void on_snareButton_clicked();

    void on_guitarButton_clicked();

    void on_speedButton_clicked();


private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
