#ifndef PATHWINDOW_H
#define PATHWINDOW_H

#include <QWidget>
#include <QVector>
#include <QPointF>

class PathWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PathWindow(QWidget *parent = nullptr);
    void setPath(const QVector<QPointF> &pathPoints, const QVector<int> &pathIndices, double totalCost);


protected:
    void paintEvent(QPaintEvent *event) override;


private:
    //QVector<QPointF> forceDirectedLayout(int iterations, double width, double height) const;
    //QVector<QPointF> arrangeNodesCircularly(int count, double radius) const;
    QVector<QPointF> pathPoints;
    double totalCost;
    QVector<int> pathIndices;
};

#endif // PATHWINDOW_H
