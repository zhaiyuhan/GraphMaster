#include "MainView.h"
#include <QtConcurrentRun>
#include <QThread>
#include <QFuture>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QRegularExpression>
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
    QFuture<QList<QString>> f1 = QtConcurrent::run(&GraphGraphicsView::collectGraphInfo, graphGraphicsView);
    f1.waitForFinished();
    res = f1.result();
    // 线程2 在此把遍历得到的结果写入文件内
    QFuture<bool> f2= QtConcurrent::run([=]{
        int count = 0; // 用来统计有效顶点个数

        QFile file(QApplication::applicationDirPath() + "/graph.txt");
        qDebug() << file.fileName();
        if(file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)){
            qDebug() << "打开文件";
            QTextStream textstream(&file);
            for(const auto &i : res) {
                bool allMinusOne = true;
                QStringList parts = i.split(" ");
                for (int i = 1; i <= parts.size() - 3; i++) {
                    qDebug() << parts[i];
                    if (parts[i] != "-1") {
                        allMinusOne = false;
                        break;
                    }
                }

                if(!allMinusOne){
                    textstream << i << "\n";
                    count++;
                }
            }
            file.close();
        }
        qDebug() << "计数结果为" << count;
        if(count == 0) return false;
        return true;
    });
    f2.waitForFinished();
    bool vaild_flag = f2.result(); // 判断该图是否有效
    qDebug() << vaild_flag;
    if(vaild_flag){
        QList<QList<int>> graph;
        // 线程3 把文件转化为图
        QFuture<QList<QList<int>>> f3 = QtConcurrent::run(&FileHelper::ConvertToGraph, fileHelper, QString("graph.txt"));
        graph = f3.result();
        f3.waitForFinished();
        // 线程4 计算
        QFuture<void> f4= QtConcurrent::run([mode, graph, start, end, this]{
            QList<QList<int>> temp_graph = graph;

            switch(mode) {
            case ALL:
                // 尝试执行查找所有路径算法
                try {
                    pathAlgorithm->shortestAllPath(temp_graph, start, end);
                } catch (const std::exception& e) {
                    mainLineEdit->sendMessage("ERROR", "出错");
                }
                break;
            case SHORT:
                // 尝试执行最短路径算法
                try {
                    pathAlgorithm->shortestSinglePath(temp_graph, start, end);
                } catch (const std::exception& e) {
                    mainLineEdit->sendMessage("ERROR", "出错");
                }
                break;
            case SHORTALL:
                break;
            }
        });
        f4.waitForFinished();
    }else{
        mainLineEdit->sendMessage("ERROR", "没有解决路径");
    }



}

void MainView::getVertexName(QMap<int, QString> &map)
{
    QFile file(QApplication::applicationDirPath() + "/graph.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(" ");
            int first = parts.at(0).toInt();
            QString last = parts.at(parts.size() - 2);
            map.insert(first, last);
        }
        file.close();
    }
}

void MainView::convertVertexName(QString &res)
{
    // 1->2->3 10
    QStringList parts = res.split(" ");
    QStringList nums = parts[0].split("->");
    int value = parts[1].toInt();
    QMap<int, QString> temp;
    this->getVertexName(temp);
    for (int i = 0; i < nums.size(); i++) {
        QString num = nums[i];
        if (temp.contains(num.toInt())) {
            nums[i] = temp[num.toInt()];
        }
    }

    QString result = nums.join("->") + " " + QString::number(value);
    mainLineEdit->sendMessage("NORAML", "名称: "+result+"\n");
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
        mainLineEdit->sendMessage("SUCCESS", "找到全部路径");
        QString res_print;
        for(auto &i : res){
            mainLineEdit->sendMessage("NORAML", "编号: "+i);
            this->convertVertexName(i);
        }
        mainLineEdit->sendMessage("NORAML", res_print);
    });
    connect(pathAlgorithm, &PathAlgorithm::singleFinish, this, [=](QString res){
        mainLineEdit->sendMessage("SUCCESS", "找到最短路径");
        mainLineEdit->sendMessage("NORAML", "编号: "+res);
        this->convertVertexName(res);

    });

}

