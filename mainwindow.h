#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPointF>
#include <QMap>
#include <QSet>
#include "pathwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onGeneratePoints();
    void clearSelectedPoints();
    void onStartPointChanged();
    void onEndPointChanged();
    void displayPathInWindow();

private:
    QVector<QPointF> generateRandomPoints(int count, int radius, QPointF center);
    QVector<QPair<int, int>> generateConnections(const QVector<QPointF> &points);
    void drawConnections(QPainter &painter, const QVector<QPointF> &points);
    QPair<QVector<int>, double> findShortestPath(int start, int end);
    void updatePointDisplays();

    Ui::MainWindow *ui;

    QVector<QPointF> generatedPoints; // Хранение точек
    QVector<int> selectedPoints; // Выбранные точки
    QVector<QPair<int, int>> connections;  // Соединительные линии

    QMap<int, QVector<int>> adjacencyList;
    PathWindow *pathWindow;
};

#endif // MAINWINDOW_H
