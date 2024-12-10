#include "pathwindow.h"

#include <QPainter>
#include <QFont>

PathWindow::PathWindow(QWidget *parent)
    : QWidget(parent), totalCost(0.0)
{
    setWindowTitle("Graph");
    resize(600, 400);
}

void PathWindow::setPath(const QVector<QPointF> &pathPoints, const QVector<int> &pathIndices, double totalCost)
{

    this->pathPoints = pathPoints;
    this->pathIndices = pathIndices; // Сохраняем индексы точек из generatedPoints
    this->totalCost = totalCost;
    update(); // Обновить отображение
}

void PathWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (pathPoints.isEmpty() || pathIndices.isEmpty())
        return;

    // Задаём отступы и интервалы
    const int margin = 50; // Отступы сверху и по бокам
    const int spacing = (height() - 2 * margin) / (pathPoints.size() - 1); // Вертикальный интервал

    // Вычисляем позиции точек для вертикального расположения
    QVector<QPointF> arrangedNodes;
    for (int i = 0; i < pathPoints.size(); ++i) {
        arrangedNodes.append(QPointF(width() / 2, margin + i * spacing));
    }

    // Рисуем соединения
    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 1; i < arrangedNodes.size(); ++i) {
        painter.drawLine(arrangedNodes[i - 1], arrangedNodes[i]);

        // Подписываем стоимость пути между узлами
        QPointF midPoint = (arrangedNodes[i - 1] + arrangedNodes[i]) / 2;
        double segmentCost = QLineF(pathPoints[i - 1], pathPoints[i]).length() * 10.0;
        painter.setPen(Qt::black);
        painter.drawText(midPoint + QPointF(10, 0), QString("%1 USD").arg(segmentCost, 0, 'f', 2));
    }

    // Рисуем узлы и подписи
    painter.setBrush(Qt::red);
    for (int i = 0; i < arrangedNodes.size(); ++i) {
        painter.drawEllipse(arrangedNodes[i], 5, 5);

        // Подписываем узлы с реальными номерами
        painter.setPen(Qt::black);
        QString nodeLabel = QString::number(i + 1) + " (" + QString::number(pathIndices[i] + 1) + ")";
        painter.drawText(arrangedNodes[i] + QPointF(10, 0), nodeLabel);
    }

    // Общая стоимость
    QString costText = QString("Total Cost: %1 USD").arg(totalCost, 0, 'f', 2);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(rect(), Qt::AlignBottom | Qt::AlignCenter, costText);
}


