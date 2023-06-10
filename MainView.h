#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "GraphGraphicsView.h"
#include "TabWidget.h"
#include "TermLineEdit.h"
#include "PathAlgorithm.h"
#include "FileHelper.h"

enum MODE {SHORT=1, ALL=2, SHORTALL=3};

class MainView : public QWidget
{
    Q_OBJECT

public:
    MainView(QWidget *parent = nullptr);
    ~MainView();

    void setupUI();
    void initLayout();
    void initEvents();
    void CORE(MODE mode, int start, int end);
private:
    QVBoxLayout *mainLayout;
    TabWidget *mainTabWidget;
    TermLineEdit *mainLineEdit; // 显示各种输出信息
    GraphGraphicsView *graphGraphicsView; // 显示顶点与边的GraphicsView
    PathAlgorithm *pathAlgorithm;
    FileHelper *fileHelper;
};
#endif // MAINVIEW_H
