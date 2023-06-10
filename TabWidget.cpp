#include "TabWidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
TabWidget::TabWidget()
{
    selectedVertexId = -1; // 初始化选中结点
    setupUI();
    this->addTab(VertexInfoWidget, "景点信息");
    this->addTab(PathWidget, "查找路径");
    this->initEvents();
}

void TabWidget::updateVertexInfoWidget(QList<QString> data)
{
    this->selectedVertexId = data[0].toInt();
    VertexIDLabel->setText(data[1]);
    VertexNameLabel->setText(data[2]);
    VertexInfoLabel->setText(data[3]);
    qDebug() << "当前选中结点的id号" << this->selectedVertexId;
}

void TabWidget::setVertexSet(QList<int> &vertexset)
{
    qDebug() << "更新顶点集";
    this->vertexlist.clear();
    startComboBox->clear();
    endComboBox->clear();
    vertexlist = vertexset;
    lineTableWidget->updateRow(vertexlist.size());
    std::sort(vertexlist.begin(), vertexlist.end());
    for(auto i = 0; i < vertexlist.size(); i++){
        startComboBox->addItem(QString::number(vertexlist[i]));
        endComboBox->addItem(QString::number(vertexlist[i]));
    }

}

void TabWidget::setPath(QMap<int, int> &path)
{
    QList<int> tempVertexList = this->vertexlist;

    QMap<int, std::tuple<int, bool>> vertexVisited;
    // 将已存在的边初始化
    foreach(int key, path.keys()) {
        qDebug() << "当前存在的id" << key << path.value(key);
        tempVertexList.removeOne(key); // 已经分配完的边删除
        vertexVisited.insert(key, std::make_tuple(path.value(key), true));
    }
    foreach(int vertex, tempVertexList) {
        qDebug() << "进入了";

        vertexVisited.insert(vertex, std::make_tuple(-1, false));
    }
    lineTableWidget->setPath(vertexVisited); // 将修改好的数据传递给lineTableWidget
}

void TabWidget::setupUI()
{
    VertexInfoWidget = new QWidget();
    PathWidget = new QWidget();
    initVertexInfoWidget();
    initPathWidget();
}

void TabWidget::initVertexInfoWidget()
{
    QVBoxLayout *tempLayout = new QVBoxLayout();
    QList<QLineEdit*> lineeditlist;
    QList<QString> list = {"编号", "名称", "简介"};
    VertexIDLabel = new QLineEdit();
    VertexIDLabel->setValidator(new QIntValidator());
    VertexNameLabel = new QLineEdit();
    VertexInfoLabel = new QLineEdit();
    lineeditlist.append(VertexIDLabel);
    lineeditlist.append(VertexNameLabel);
    lineeditlist.append(VertexInfoLabel);

    for (const auto &item : list) {
        static int index = 0;
        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(new QLabel(item));
        layout->addWidget(lineeditlist[index++]);
        tempLayout->addLayout(layout);
    }
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    editVertexButton = new QPushButton("修改");
    deleteVertexButton = new QPushButton("删除");
    buttonLayout->addWidget(editVertexButton);
    buttonLayout->addWidget(deleteVertexButton);
    tempLayout->addLayout(buttonLayout);

    lineTableWidget = new TableWidget();

    tempLayout->addWidget(lineTableWidget);
    updateEdgeButton = new QPushButton("更新距离");
    tempLayout->addWidget(updateEdgeButton);
    VertexInfoWidget->setLayout(tempLayout);

}

void TabWidget::initPathWidget()
{
    QVBoxLayout *tempLayout = new QVBoxLayout();
    startComboBox = new QComboBox();
    endComboBox = new QComboBox();
    BFSButton = new QPushButton("查找所有路径");
    DijkstraButton = new QPushButton("查找最短路径");
    QHBoxLayout *startLayout = new QHBoxLayout();
    startLayout->addWidget(new QLabel("起始顶点"));
    startLayout->addWidget(startComboBox);
    QHBoxLayout *endLayout = new QHBoxLayout();
    endLayout->addWidget(new QLabel("终止顶点"));
    endLayout->addWidget(endComboBox);
    tempLayout->addLayout(startLayout);
    tempLayout->addLayout(endLayout);
    tempLayout->addWidget(BFSButton);
    tempLayout->addWidget(DijkstraButton);
    PathWidget->setLayout(tempLayout);
}

void TabWidget::initEvents()
{
    connect(editVertexButton, &QPushButton::clicked, [=](){
        emit updateVertex(this->selectedVertexId, {VertexIDLabel->text(), VertexNameLabel->text(), VertexInfoLabel->text()});
    });
    connect(deleteVertexButton, &QPushButton::clicked, [=](){
        emit deleteVertex(this->selectedVertexId);
    });
    connect(updateEdgeButton, &QPushButton::clicked, [=](){
        // 更新边信息 把边信息发送给GraphGraphicsView
        auto data = lineTableWidget->getNewPath();
        emit updateEdge(this->selectedVertexId, data);
    });
    connect(BFSButton, &QPushButton::clicked, [=](){
        emit tryFindAllPath(startComboBox->currentText().toInt(), endComboBox->currentText().toInt());
    });
    connect(DijkstraButton, &QPushButton::clicked, [=](){
        emit tryFindShortestPath(startComboBox->currentText().toInt(), endComboBox->currentText().toInt());
    });
}

TableWidget::TableWidget()
{
    rowCounter = 0;
    QStringList headerLabels = {"顶点编号", "距离"};
    this->setColumnCount(headerLabels.size());
    this->setHorizontalHeaderLabels(headerLabels);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

QMap<int, int> TableWidget::getNewPath()
{
    QMap<int, int> newPath;
    for(int i = 0; i < rowCounter; i++) {
        QLabel *comboBox = dynamic_cast<QLabel*>(this->cellWidget(i, 0));
        if(comboBox) {
        }
        QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(this->cellWidget(i, 1));
        if(lineEdit) {
        }
        newPath.insert(comboBox->text().toInt(), lineEdit->text().toInt());
    }
    return newPath;
}

void TableWidget::updateRow(int n)
{
    this->setRowCount(n);
    for(int i = 0; i < n; i++) {
        this->setCellWidget(i, 0, new QLabel);
        QIntValidator *validator = new QIntValidator(-1, INT_MAX, this);
        QLineEdit* tempLineEdit = new QLineEdit();
        tempLineEdit->setValidator(validator);
        this->setCellWidget(i, 1, tempLineEdit);
    }
    rowCounter = n;
}

void TableWidget::insert(int index, int id, int length)
{
    QLabel *comboBox = dynamic_cast<QLabel*>(this->cellWidget(index, 0));
    if(comboBox) {
        comboBox->setText(QString::number(id));
    }
    QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(this->cellWidget(index, 1));
    if(lineEdit) {
        lineEdit->setText(QString::number(length));
    }

}
