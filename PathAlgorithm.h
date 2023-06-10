#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <QObject>
#include <vector>
using namespace std;

class PathAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit PathAlgorithm(QObject *parent = nullptr);
public slots:
    void shortestSinglePath(QList<QList<int>> &graph, int start, int end);
    void shortestAllPath(QList<QList<int>> &data, int start, int end);
    void multiSourceShortestPath(QList<QList<int>> &data, QList<int> starts, int end);
    void findAllPathsDFS(const vector<vector<int>>& graph, vector<bool>& visited, vector<int>& path, vector<vector<int>>& paths, int current, int end, int weight);

signals:

    void singleFinish(QString);
    void allFinish(QList<QString>);
    void multiFinish(QString);
};

#endif // DIJKSTRA_H
