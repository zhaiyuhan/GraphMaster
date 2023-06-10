#include "PathAlgorithm.h"
#include <QFile>
#include <QDebug>
#include <iostream>
#include <vector>
#include <limits>
#include <vector>
#include <queue>
#include <algorithm>
#include <QTextStream>
using namespace std;

typedef vector<vector<int> > Graph;
const int INF = numeric_limits<int>::max();

PathAlgorithm::PathAlgorithm(QObject *parent)
    : QObject{parent}
{

}


void PathAlgorithm::shortestSinglePath(QList<QList<int>> &data, int start, int end)
{
    // -------- QList -> vector start ---------
    int n=0;
    for(auto list:data)
    {
        n=max(n,list.at(0));
        n=max(n,list.at(1));
    }
    Graph graph(n+1,vector<int>(n+1,0));
    for(auto list:data)
    {
        int i=list.at(0),j=list.at(1),w=list.at(2);
        graph[i][j]=graph[j][i]=w;
    }
    // 输出邻接矩阵 start
    //    QTextStream out(stdout);
    //    out << "邻接矩阵" << "\n";
    //    out << "{\n";
    //    for (int i = 0; i < n; i++)
    //    {
    //        out << '{';
    //        for (int j = 0; j < n; j++)
    //        {
    //            if (j!=n-1)
    //                out << graph[i][j] << ',';
    //            else
    //                out << graph[i][j];
    //        }
    //        out << "},\n";
    //    }
    //    out << "}\n";
    // 输出邻接矩阵 end
    // -------- QList -> vector end ---------
    int numVertices = graph.size();
    vector<int> distances(numVertices, numeric_limits<int>::max());
    vector<int> previous(numVertices, -1);
    vector<bool> visited(numVertices, false);
    distances[start] = 0;
    auto compare = [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second > b.second;
    };
    priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(compare)> pq(compare);
    pq.push(make_pair(start, 0));

    while (!pq.empty()) {
        int u = pq.top().first;
        pq.pop();

        if (visited[u])
            continue;

        visited[u] = true;

        for (int v = 0; v < numVertices; ++v) {
            if (graph[u][v] != 0 && !visited[v]) {
                int alt = distances[u] + graph[u][v];
                if (alt < distances[v]) {
                    distances[v] = alt;
                    previous[v] = u;
                    pq.push(make_pair(v, alt));
                }
            }
        }
    }
    vector<int> path;
    int current = end;
    while (current != -1) {
        path.insert(path.begin(), current);
        current = previous[current];
    }
    // --- 结果返回 start ----
    QString resStr = QString::number(path[0]),tmpStr;
    for (int i = 1; i < path.size(); i++) {
        tmpStr=QString("->").append(QString::number(path[i]));
        resStr.append(tmpStr);
    }
    emit singleFinish(resStr);
    // --- 结果返回 end ----
}
void PathAlgorithm::shortestAllPath(QList<QList<int>> &data, int start, int end)
{

    // -------- QList -> vector start ---------
    int n=0;
    for(auto list:data)
    {
        n=max(n,list.at(0));
        n=max(n,list.at(1));
    }
    Graph graph(n+1,vector<int>(n+1,0));
    for(auto list:data)
    {
        int i=list.at(0),j=list.at(1),w=list.at(2);
        graph[i][j]=graph[j][i]=w;
    }
    // 输出邻接矩阵 start
    //    QTextStream out(stdout);
    //    out << "邻接矩阵" << "\n";
    //    out << "{\n";
    //    for (int i = 0; i < n; i++)
    //    {
    //        out << '{';
    //        for (int j = 0; j < n; j++)
    //        {
    //            if (j!=n-1)
    //                out << graph[i][j] << ',';
    //            else
    //                out << graph[i][j];
    //        }
    //        out << "},\n";
    //    }
    //    out << "}\n";
    // 输出邻接矩阵 end
    // -------- QList -> vector end ---------
    vector<bool> visited(graph.size(), false);
    vector<int> path;
    vector<vector<int>> paths;
    findAllPathsDFS(graph, visited, path, paths, start, end, 0);
    QList<QString> resList;
    for (auto path: paths)
    {
        QString resStr = QString::number(path[0]),tmpStr;
        for (int i = 1; i < path.size(); i++) {
            tmpStr=QString("->").append(QString::number(path[i]));
            resStr.append(tmpStr);
        }
        resList.append(resStr);
    }
    emit allFinish(resList);
}

void PathAlgorithm::findAllPathsDFS(const vector<vector<int>>& graph, vector<bool>& visited, vector<int>& path, vector<vector<int>>& paths, int current, int end, int weight)
{
    visited[current] = true;
    path.push_back(current);

    if (current == end) {
        // 将路径添加到 paths 中
        paths.push_back(path);
    } else {
        for (int i = 0; i < graph.size(); i++) {
            if (graph[current][i] != 0 && !visited[i]) {
                findAllPathsDFS(graph, visited, path, paths, i, end, weight + graph[current][i]);
            }
        }
    }
    // 回溯
    visited[current] = false;
    path.pop_back();
}

void PathAlgorithm::multiSourceShortestPath(QList<QList<int>> &data, QList<int> starts, int end)
{
    QString resStr("1->2->5");
    emit multiFinish(resStr);
}
