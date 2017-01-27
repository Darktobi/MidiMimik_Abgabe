#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    threshhold(20),
    bassString("10"),
    hihatString("10"),
    snareString("10"),
    guitarString("10"),
    speedString("300"),
    instrumentIndex(0),
    turnUp(true),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    this->threshhold = value;
    ui->label_2->setText(QString::number(value));

}

void MainWindow::refresh(){

    //Die Listenitems werden immer neu erstellt, nicht schön aber extern bearbeiten geht nicht
    QTableWidgetItem *bassVel = new QTableWidgetItem;
    QTableWidgetItem *hihatVel = new QTableWidgetItem;
    QTableWidgetItem *snareVel = new QTableWidgetItem;
    QTableWidgetItem *guitarVel = new QTableWidgetItem;
    QTableWidgetItem *speed = new QTableWidgetItem;

    //Setz den Text des Items auf den String den wir mit den Augen bearbeiten
    bassVel->setText(bassString);
    hihatVel->setText(hihatString);
    snareVel->setText(snareString);
    guitarVel->setText(guitarString);
    speed->setText(speedString);

    //Setz das Item mit dem richtigen Text in die passende Zelle
    ui->table->setItem(0, 0, bassVel);
    ui->table->setItem(1, 0, hihatVel);
    ui->table->setItem(2, 0, snareVel);
    ui->table->setItem(3, 0, guitarVel);
    ui->table->setItem(4, 0, speed);

    //Die Radiobuttons sollen auch gecheckt werden wenn wir aus dem Programm das Instrument verändern
    if(instrumentIndex == 0){
        ui->bassButton->toggle();
    } else if(instrumentIndex == 1){
        ui->hihatButton->toggle();
    } else if(instrumentIndex == 2){
        ui->snareButton->toggle();
    } else if(instrumentIndex == 3){
        ui->guitarButton->toggle();
    } else if(instrumentIndex == 4){
        ui->speedButton->toggle();
    }

}

void MainWindow::on_checkBox_toggled(bool checked)
{
    this->turnUp = checked;
}


void MainWindow::on_bassButton_clicked()
{
    this->instrumentIndex = 0;
}

void MainWindow::on_hihatButton_clicked()
{
    this->instrumentIndex = 1;
}

void MainWindow::on_snareButton_clicked()
{
    this->instrumentIndex = 2;
}

void MainWindow::on_guitarButton_clicked()
{
    this->instrumentIndex = 3;
}

void MainWindow::on_speedButton_clicked()
{
    this->instrumentIndex = 4;
}


