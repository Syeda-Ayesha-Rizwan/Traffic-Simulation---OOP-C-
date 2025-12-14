#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simulationarea.h"
#include <QTimer>
#include <QTime>
#include <QDate>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Traffic Simulation System");
    setWindowIcon(QIcon(":/images/ImageTANCE/logo_inter.png"));

    // Create simulation area
    SimulationArea *simulationArea = new SimulationArea(this);
    simulationArea->setGeometry(50, 50, 700, 500);

    // Hide QGraphicsView
    ui->simulationFrame->setVisible(false);

    // Set logo
    QPixmap logo(":/images/ImageTANCE/Logo-AI.png");
    if (!logo.isNull()) {
        ui->logoLabel->setPixmap(logo.scaled(100, 100));
    }

    // Set button icons
    ui->playButton->setIcon(QIcon(":/images/ImageTANCE/play-icon.png"));
    ui->pauseButton->setIcon(QIcon(":/images/ImageTANCE/pause-icon.png"));
    ui->resetButton->setIcon(QIcon(":/images/ImageTANCE/reset-icon.png"));

    // Set initial time
    ui->dateLabel->setText(QDate::currentDate().toString("dd-MM-yyyy"));
    ui->timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));

    // Connect buttons
    connect(ui->playButton, &QPushButton::clicked, simulationArea, &SimulationArea::startSimulation);
    connect(ui->pauseButton, &QPushButton::clicked, simulationArea, &SimulationArea::stopSimulation);
    connect(ui->resetButton, &QPushButton::clicked, simulationArea, &SimulationArea::resetSimulation);

    // Start simulation
    simulationArea->startSimulation();

    // Update time
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTime);
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTime()
{
    ui->timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));
    ui->dateLabel->setText(QDate::currentDate().toString("dd-MM-yyyy"));
}
