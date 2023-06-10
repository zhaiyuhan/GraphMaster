#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QTableWidget>
QT_BEGIN_NAMESPACE
class QWidget;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class TableWidget;
QT_END_NAMESPACE


class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TabWidget();
    void updateVertexInfoWidget(QList<QString> data); // 更新顶点信息面板
    void setVertexSet(QList<int> &vertexset); // 设置顶点集
    void setPath(QMap<int, int> &path);
private:
    void setupUI();
    void initVertexInfoWidget(); // 初始化结点信息面板
    void initPathWidget(); // 初始化路径面板
    void initEvents();

private:
    QWidget *VertexInfoWidget; // 顶点信息面板
    QLineEdit *VertexIDLabel, *VertexNameLabel, *VertexInfoLabel; // 顶点编号 名称 简介
    QPushButton *editVertexButton, *deleteVertexButton; // 修改结点 删除结点
    TableWidget *lineTableWidget; // 边
    QPushButton *updateEdgeButton; // 更新边

    QWidget *PathWidget; // 路径算法面板
    QComboBox *startComboBox, *endComboBox; // 起始 末尾
    QPushButton *BFSButton, *DijkstraButton; // 所有路径 最短路径
signals:
    void updateVertex(int id, QList<QString> data); //更新顶点信号
    void updateEdge(int id, QMap<int, int> &data); // 更新边信号
    void deleteVertex(int id); //删除顶点信号
    void tryFindAllPath(int start, int end);
    void tryFindShortestPath(int start, int end);
private:
    int selectedVertexId; // 当前选中的结点id号
    QList<int> vertexlist;
};


class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    TableWidget();
    QMap<int, int> getNewPath();
    void updateRow(int n);
    void setPath(QMap<int, std::tuple<int, bool>>& _vertexVisited) {
        this->vertexVisited = _vertexVisited;
        int i = 0;
        foreach(int key, vertexVisited.keys()) {
            insert(i++, key, std::get<0>(vertexVisited.value(key)));
        }

    }
private:
    void insert(int index, int id, int length);
    QMap<int, std::tuple<int, bool>> vertexVisited;
    int rowCounter;
};

#endif // TABWIDGET_H
