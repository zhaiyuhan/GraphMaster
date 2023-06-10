#ifndef GRAPHGRAPHICSVIEW_H
#define GRAPHGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
class VertexItem;
class EdgeItem;
class QGraphicsTextItem;
class GraphGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphGraphicsView(QWidget *parent = nullptr);
    void initEvents();
public:
    void deleteVertex(int id); // 删除顶点 传入参数:ID号
    void updateVertex(int id, QList<QString> &data); // 更新顶点 传入参数:ID号 数据List
    void updateEdge(int id, QMap<int, int> &data); // 更新边 传入参数:ID号 边集信息
    QList<QString> collectGraphInfo(); // 遍历整张表
signals:
    void printMessage(QString message);
    void notifyInfoWidget(QList<QString> &value, QMap<int, int> &path); // 信号 通知面板更新信息
    void updateVertexSet(QList<int> &vertexset); // 信号 更新顶点集
protected:
    void mousePressEvent(QMouseEvent *event) override;
private:
    void sendVertexSet();
    QGraphicsScene *mainGraphicsScene;
    int VertexIncrementer ; // 顶点数记录自增器
    QMap<int, int> VertexMap; // 键-编号 值-自增器
};

class VertexItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    QMap<int, int> path; // 键-相关顶点编号 值-路径
    QMap<VertexItem*, EdgeItem*> edges; // 键-对应顶点号 值-路径线条指针
    VertexItem();
    void updateText(QString str);
    void setPath(QMap<int, int> &data) {
        qDebug() << "遍历路径信息";
        foreach(int key, data.keys()){
            qDebug() << key << data.value(key);
        }
        this->path.clear();
        this->path = data; // 更新路径信息
    }
signals:
    void findInfo(QList<QString> &value, QMap<int, int> &path); // 信号 查看顶点信息
    void updatePathLine(QPointF point); // 更新路径线条的信号
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override
    {
        if (change == QGraphicsItem::ItemPositionHasChanged) {
            emit updatePathLine(this->mapToScene(this->boundingRect().center()));
        }
        return QGraphicsEllipseItem::itemChange(change, value);
    }
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        if(event->button() == Qt::RightButton) {
            qDebug() << "右击了鼠标";
        }
        if(event->button() == Qt::LeftButton){
            QList<QString> vertex_data;
            for(int i = 0; i <= 3; i++) {
                vertex_data.append(this->data(i).toString());
            }
            emit this->findInfo(vertex_data, this->path);
            QGraphicsEllipseItem::mousePressEvent(event);
        }
    }

private:
    QGraphicsTextItem* text;
};

class EdgeItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    EdgeItem(qreal x1, qreal y1, qreal x2, qreal y2)
    {

        this->setLine(x1, y1, x2, y2);
        this->setZValue(0);
        this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        text = new QGraphicsTextItem(this); // 创建文字项目
        text->setDefaultTextColor(Qt::black); // 设置文字颜色
        text->setFont(QFont("Arial", 24)); // 设置文字字体和大小
        text->setZValue(2);
        text->setParentItem(this);
        updateText();
    }

    void setLength(int _length) { this->length = _length; updateText(); }
    void updateText() {
        text->setPlainText(QString::number(length));
        text->setPos(this->line().center());
    }

private:
    int length;
    QGraphicsTextItem* text;
};

#endif // GRAPHGRAPHICSVIEW_H
