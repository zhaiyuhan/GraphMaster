#include "GraphGraphicsView.h"
GraphGraphicsView::GraphGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    // 优化绘图效果
    this->setOptimizationFlag(QGraphicsView::IndirectPainting);
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setRenderHint(QPainter::Antialiasing);

    this->VertexIncrementer = 1; // 初始化自增器
    mainGraphicsScene = new QGraphicsScene(this);
    this->setScene(mainGraphicsScene);
    this->setMouseTracking(true);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    initEvents();
}

void GraphGraphicsView::initEvents()
{
    connect(mainGraphicsScene, &QGraphicsScene::changed, [=](){

    });
}

void GraphGraphicsView::deleteVertex(int id)
{
    QList<QGraphicsItem*> items = mainGraphicsScene->items();
    foreach(QGraphicsItem* item, items) {
        if (item->data(0).toInt() == id) {
            mainGraphicsScene->removeItem(item);
            delete item;
            foreach(int key, VertexMap.keys()){
                if(VertexMap.value(key) == id) {
                    VertexMap.remove(key);
                    break;
                }
            }
            break;
        }
    }
    sendVertexSet();
}

void GraphGraphicsView::updateVertex(int id, QList<QString> &data)
{
    if(data[0].toInt() >= VertexIncrementer) {
        emit printMessage("设置编号过大");
    } else {
        QList<QGraphicsItem*> items = mainGraphicsScene->items();
        foreach(QGraphicsItem* item, items) {
            if (item->data(0).toInt() == id) {
                if(VertexMap.contains(data[0].toInt())) {
                    // 如果该顶点编号已经存在
                    if(VertexMap.value(data[0].toInt()) == id) {
                        // 如果该顶点编号没变 仅更新顶点信息
                        item->setData(2, data[1]);
                        item->setData(3, data[2]);
                        VertexItem *it = dynamic_cast<VertexItem*>(item);
                        it->updateText(data[1]);
                    } else {
                        // 如果顶点编号冲突
                        qDebug() << "该编号已经有顶点";
                        break;
                    }
                } else {
                    // 该编号还没有所属顶点
                    // 先删除原先的顶点的编号
                    item->setData(1, data[0]);
                    foreach(int key, VertexMap.keys()){
                        if(VertexMap.value(key) == id) {
                            VertexMap.remove(key);
                            break;
                        }
                    }
                    item->setData(2, data[1]);
                    item->setData(3, data[2]);
                    VertexItem *it = dynamic_cast<VertexItem*>(item);
                    it->updateText(data[1]);
                }
                break;
            }
        }
    }
    sendVertexSet();
}

void GraphGraphicsView::updateEdge(int id, QMap<int, int> &data)
{
    QList<QGraphicsItem*> items = mainGraphicsScene->items();
    VertexItem *it = nullptr;
    // 第一次遍历 找出要更新的顶点 并更新其路径表
    foreach(QGraphicsItem* item, items) {
        VertexItem *temp = dynamic_cast<VertexItem*>(item);
        if(temp) {
            if (item->data(0).toInt() == id) {
                it = dynamic_cast<VertexItem*>(item);
                // 在此更新路径信息
                it->setPath(data);
            } else {
                if(temp->path.isEmpty()) {
                    // 没有路径表
                    temp->setPath(data);
                    foreach (auto i, temp->path.keys()) {
                        temp->path[i] = -1;
                    }
                }
                temp->path[id] = data[temp->data(1).toInt()];
            }
        }
    }
    // 第二次遍历 把与之有关的顶点连接起来
    foreach(QGraphicsItem* item, items) {
        if (data.contains(item->data(0).toInt())) {
            VertexItem *temp = dynamic_cast<VertexItem*>(item);
            if(temp == nullptr || item->data(0).toInt() == id) continue;
            // 如果两者之间不存在边的关系
            if(data[item->data(0).toInt()] == -1) {
                EdgeItem *del = nullptr;
                if(it != nullptr && it->edges.contains(temp)) {
                    del = it->edges.value(temp);
                    it->edges.remove(temp);
                }
                if(temp->edges.contains(it)) {
                    temp->edges.remove(it);
                }
                // some problems here
                mainGraphicsScene->removeItem(del);
                delete del;
                continue;
            }
            // 如果两者之间存在边的关系 先判断是否有边线
            if(data[item->data(0).toInt()] != -1) {
                if(it != nullptr && it->edges.contains(temp)) {
                    it->edges[temp]->setLength(data[item->data(0).toInt()]);
                    continue;
                }
            }
            QPointF center1, center2;
            if(it != nullptr) {
                center1 = it->mapToScene(it->boundingRect().center());
            }
            center2 = temp->mapToScene(temp->boundingRect().center());
            EdgeItem *line = new EdgeItem(center1.x(), center1.y(), center2.x(), center2.y());
            line->setPen(QPen(Qt::red, 2));
            line->setLength(data[item->data(0).toInt()]);
            if(it != nullptr) {
                it->edges.insert(temp, line);
            }
            temp->edges.insert(it, line);
            mainGraphicsScene->addItem(line);
        }
    }

}

QList<QString> GraphGraphicsView::collectGraphInfo()
{
    QList<QString> res;
    qDebug() << "开始收集整张表";
    QList<QGraphicsItem*> items = mainGraphicsScene->items(this->sceneRect());
    foreach(QGraphicsItem* item, items) {
        VertexItem *it = dynamic_cast<VertexItem*>(item);
        if(it){
            QString temp;
            temp.append(QString::number(it->data(1).toInt()).append(" ")); // 写入
            for(int i = 1; i < VertexIncrementer; i++) {
                if(it->path.contains(i)) {
                    temp.append(QString::number(it->path[i]).append(" "));
                } else {
                    temp.append(QString::number(-1).append(" "));
                }
            }
            temp.append(it->data(2).toString().append(" "));
            temp.append(it->data(3).toString());
            res.append(temp);
        }
    }
    return res;
}
void GraphGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    // 调用原先的鼠标事件，防止控件元拖动失效
    if(event->button() == Qt::RightButton)
    {

        VertexItem *item = new VertexItem();
        item->setPos(mapToScene(event->pos()));
        item->setData(0, VertexIncrementer++); // 设置ID号
        item->setData(1, VertexIncrementer-1);
        item->setData(2, QString("新增顶点%1").arg(VertexIncrementer-1));
        item->setData(3, "自定义简介");
        item->updateText(QString("新增顶点%1").arg(VertexIncrementer-1));
        connect(item, &VertexItem::findInfo, this, [&](QList<QString> &value, QMap<int, int> &path){
            emit this->notifyInfoWidget(value, path);
        });
        connect(item, &VertexItem::updatePathLine, this, [=](QPointF point) {
            QPointF center1, center2;
            center1 = item->mapToScene(item->boundingRect().center());
            foreach(auto it, item->edges.keys()){
                center2 = it->mapToScene(it->boundingRect().center());
                EdgeItem* line = item->edges.value(it);
                line->setLine(center1.x(), center1.y(), center2.x(), center2.y());
                line->updateText();
            }
        });
        VertexMap.insert(VertexIncrementer-1, VertexIncrementer-1);
        mainGraphicsScene->addItem(item);
        sendVertexSet();
    }

}

void GraphGraphicsView::sendVertexSet()
{
    QList<int> vertexset; // 获得当前的顶点集合
    foreach(int key, VertexMap.keys()){
        vertexset.append(key);
    }
    emit updateVertexSet(vertexset);
}

VertexItem::VertexItem()
{
    text = new QGraphicsTextItem(this); // 创建文字项目
    text->setDefaultTextColor(Qt::black); // 设置文字颜色
    text->setFont(QFont("Arial", 24)); // 设置文字字体和大小
    this->setRect(0, 0, 50, 50);
    this->setFlags(QGraphicsItem::ItemIsMovable|
                   QGraphicsItem::ItemIsFocusScope|
                   QGraphicsItem::ItemIsSelectable|
                   QGraphicsItem::ItemSendsGeometryChanges);
    this->setBrush(QBrush(QColor(121, 224, 238)));
    this->setZValue(1);
}

void VertexItem::updateText(QString str)
{
    text->setPlainText(str); // 设置文字内容
    text->setPos(this->boundingRect().center() - text->boundingRect().center());

}
