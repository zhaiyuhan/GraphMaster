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
    for(auto list:data)                                                                       // 找到输入数据中的最大顶点值
    {
        n=max(n,list.at(0));
        n=max(n,list.at(1));
    }
    Graph graph(n+1,vector<int>(n+1,0));                                                      // 创建一个由2D向量表示的图
    for(auto list:data)                                                                       // 将QList数据转换为向量，并填充图
    {
        int i=list.at(0),j=list.at(1),w=list.at(2);
        graph[i][j]=graph[j][i]=w;
    }
    // ---------- 输出邻接矩阵 start -----------
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
    // ---------- 输出邻接矩阵 end ------------
    // -------- QList -> vector end ---------
    int numVertices = graph.size();
    vector<int> distances(numVertices, numeric_limits<int>::max());                         // 距离
    vector<int> previous(numVertices, -1);                                                  // 前一个顶点状态
    vector<bool> visited(numVertices, false);                                               // 访问状态
    distances[start] = 0;
    auto compare = [](const pair<int, int>& a, const pair<int, int>& b) {                   // 自定义比较函数用于优先队列
        return a.second > b.second;
    };
    priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(compare)> pq(compare);  // 创建一个优先队列，根据距离进行排序
    pq.push(make_pair(start, 0));
    while (!pq.empty()) {
        int u = pq.top().first;                                                             // 从优先队列中提取距离最小的顶点
        pq.pop();

        if (visited[u])
            continue;

        visited[u] = true;

        for (int v = 0; v < numVertices; ++v) {                                             // 遍历当前顶点的邻居
            if (graph[u][v] != 0 && !visited[v]) {                                          // 检查当前顶点与邻居之间是否有边
                int alt = distances[u] + graph[u][v];                                       // 计算通过当前顶点到达邻居的新距离
                if (alt < distances[v]) {                                                   // 如果新距离小于之前的距离，则更新距离和前一个顶点，并将邻居添加到优先队列中
                    distances[v] = alt;
                    previous[v] = u;
                    pq.push(make_pair(v, alt));
                }
            }
        }
    }
    vector<int> path;                                                                       // 重新构建从起始顶点到结束顶点的最短路径
    int current = end;
    while (current != -1) {
        path.insert(path.begin(), current);                                                 // 将当前顶点插入路径的开头
        current = previous[current];                                                        // 移动到最短路径中的前一个顶点
    }
    // -------- 结果返回 start ---------
    QString resStr = QString::number(path[0]),tmpStr;                                       // 将路径向量转换为字符串表示形式
    for (int i = 1; i < path.size(); i++) {
        tmpStr=QString("->").append(QString::number(path[i]));
        resStr.append(tmpStr);
    }
    int length=0;
    for(int i=0;i<path.size()-1;i++)
    {
        int pre = path[i];
        int next = path[i+1];
        length += graph[pre][next];
    }
    tmpStr=QString(" ").append(QString::number(length));
    resStr.append(tmpStr);
    emit singleFinish(resStr);                                                              // 使用 singleFinish 信号发出最终的最短路径结果
    // -------- 结果返回  end  ---------
}
void PathAlgorithm::shortestAllPath(QList<QList<int>> &data, int start, int end)
{

    // -------- QList -> vector start ---------
    int n=0;
    for(auto list:data)                                                                     // 找到输入数据中的最大顶点值
    {
        n=max(n,list.at(0));
        n=max(n,list.at(1));
    }
    Graph graph(n+1,vector<int>(n+1,0));                                                    // 创建一个由2D向量表示的图
    for(auto list:data)                                                                     // 将QList数据转换为向量，并填充图
    {
        int i=list.at(0),j=list.at(1),w=list.at(2);
        graph[i][j]=graph[j][i]=w;
    }
    // ---------- 输出邻接矩阵 start -----------
    //        QTextStream out(stdout);
    //        out << "邻接矩阵" << "\n";
    //        out << "{\n";
    //        for (int i = 0; i < n; i++)
    //        {
    //            out << '{';
    //            for (int j = 0; j < n; j++)
    //            {
    //                if (j!=n-1)
    //                    out << graph[i][j] << ',';
    //                else
    //                    out << graph[i][j];
    //            }
    //            out << "},\n";
    //        }
    //        out << "}\n";
    // ---------- 输出邻接矩阵 end ------------
    // -------- QList -> vector end ---------
    vector<bool> visited(graph.size(), false);
    vector<int> path;
    vector<vector<int>> paths;
    findAllPathsDFS(graph, visited, path, paths, start, end, 0);                            // 使用深度优先搜索查找所有路径
    QList<QString> resList;                                                                 // 将路径转换为QString列表
    for (auto path: paths)
    {
        QString resStr = QString::number(path[0]),tmpStr;
        for (int i = 1; i < path.size(); i++) {
            tmpStr=QString("->").append(QString::number(path[i]));
            resStr.append(tmpStr);
        }
        int length=0;
        for(int i=0;i<path.size()-1;i++)
        {
            int pre = path[i];
            int next = path[i+1];
            length += graph[pre][next];
        }
        tmpStr=QString(" ").append(QString::number(length));
        resStr.append(tmpStr);
        resList.append(resStr);
    }
    emit allFinish(resList);                                                                // 发出最终的所有路径结果
}

void PathAlgorithm::findAllPathsDFS(const vector<vector<int>>& graph, vector<bool>& visited, vector<int>& path, vector<vector<int>>& paths, int current, int end, int weight)
{
    visited[current] = true;                                                                // 标记当前顶点为已访问
    path.push_back(current);                                                                // 将当前顶点添加到路径中
    if (current == end) {                                                                   // 如果当前顶点为结束顶点，将路径添加到 paths 中
        paths.push_back(path);
    } else {
        for (int i = 0; i < graph.size(); i++) {                                            // 遍历当前顶点的邻居
            if (graph[current][i] != 0 && !visited[i]) {                                    // 如果当前顶点与邻居之间有边且邻居未被访问，则继续深度优先搜索
                findAllPathsDFS(graph, visited, path, paths, i, end, weight + graph[current][i]);
            }
        }
    }
    visited[current] = false;                                                               // 回溯：将当前顶点标记为未访问
    path.pop_back();                                                                        // 并从路径中移除
}
