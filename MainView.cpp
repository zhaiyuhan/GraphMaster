#include "MainView.h"
#include <QtConcurrentRun>
#include <QThread>
#include <QFuture>
#include <QFile>
#include <QTextStream>
#include <QApplication>
MainView::MainView(QWidget *parent)
    : QWidget(parent)
{
    pathAlgorithm = new PathAlgorithm();
    fileHelper = new FileHelper();
    this->resize(800, 600);
    this->setWindowTitle("校园导航");
    setupUI();
    initLayout();
    initEvents();
}

MainView::~MainView()
{
}

void MainView::setupUI()
{
    graphGraphicsView = new GraphGraphicsView();
    mainTabWidget = new TabWidget();
    mainLineEdit = new TermLineEdit();
}

void MainView::initLayout()
{
    mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(graphGraphicsView, 7);
    topLayout->addWidget(mainTabWidget, 3);
    mainLayout->addLayout(topLayout, 7);
    mainLayout->addWidget(mainLineEdit, 3);

}

void MainView::CORE(MODE mode, int start, int end){
    QList<QString> res;
    // 线程1 在此遍历图的全部顶点
    QFuture<QList<QString>> f = QtConcurrent::run(&GraphGraphicsView::collectGraphInfo, graphGraphicsView);
    res = f.result();
    f.waitForFinished();
    // 线程2 在此把遍历得到的结果写入文件内
    QFuture<void> f2= QtConcurrent::run([=]{
        QFile file(QApplication::applicationDirPath() + "/graph.txt");
        qDebug() << file.fileName();
        if(file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)){
            qDebug() << "打开文件";
            QTextStream textstream(&file);
            for(const auto &i : res) {
                textstream << i << "\n";
            }
            file.close();
        }
    });
    f2.waitForFinished();
    QList<QList<int>> graph;
    // 线程3 把文件转化为图
    QFuture<QList<QList<int>>> f3 = QtConcurrent::run(&FileHelper::ConvertToGraph, fileHelper, QString("graph.txt"));
    graph = f3.result();
    f3.waitForFinished();
    switch(mode) {
    case ALL:
        pathAlgorithm->shortestAllPath(graph, start, end);
        break;
    case SHORT:
        pathAlgorithm->shortestSinglePath(graph, start, end);
        break;
    case SHORTALL:
        break;
    }
}


void MainView::initEvents()
{
    connect(graphGraphicsView, &GraphGraphicsView::printMessage, this, [&](QString message){
        mainLineEdit->sendMessage("SUCCESS", "插入");
        mainLineEdit->sendMessage("ERROR", message);
    });
    connect(graphGraphicsView, &GraphGraphicsView::notifyInfoWidget, this, [&](QList<QString> &value, QMap<int, int> &path){
        mainTabWidget->updateVertexInfoWidget(value);
        mainTabWidget->setPath(path);
    });
    connect(graphGraphicsView, &GraphGraphicsView::updateVertexSet, this, [&](QList<int> &vertexset){
        mainTabWidget->setVertexSet(vertexset);
    });
    connect(mainTabWidget, &TabWidget::updateVertex, this, [=](int id, QList<QString> info) {
        graphGraphicsView->updateVertex(id, info);
    });
    connect(mainTabWidget, &TabWidget::updateEdge, this, [=](int id, QMap<int, int> info) {
        graphGraphicsView->updateEdge(id, info);
    });
    connect(mainTabWidget, &TabWidget::deleteVertex, this, [=](int id) {
        graphGraphicsView->deleteVertex(id);
    });
    connect(mainTabWidget, &TabWidget::tryFindAllPath, this, [=](int start, int end){
        qDebug() << "尝试寻找所有路径";
        CORE(ALL, start, end);
    });
    connect(mainTabWidget, &TabWidget::tryFindShortestPath, this, [=](int start, int end){
        qDebug() << "尝试寻找最短路径";
        CORE(SHORT, start, end);

    });
    connect(pathAlgorithm, &PathAlgorithm::allFinish, this, [=](QList<QString> res){
        mainLineEdit->append("找到全部路径");
        for(auto &i : res){
            mainLineEdit->append(i);
        }
    });
    connect(pathAlgorithm, &PathAlgorithm::singleFinish, this, [=](QString res){
        mainLineEdit->append("找到最短路径");
        mainLineEdit->append(res);
    });

}

