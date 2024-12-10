#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Рандомные точки и соединения");
    srand(static_cast<unsigned>(time(nullptr))); // Инициализация генератора случайных чисел
    pathWindow = new PathWindow();

    // Подключение кнопки к слоту
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onGeneratePoints);
    connect(ui->ClearSelect, &QPushButton::clicked, this, &MainWindow::clearSelectedPoints);
    connect(ui->GraphicsButton, &QPushButton::clicked, this, &MainWindow::displayPathInWindow);

    connect(ui->lineEditStart, &QLineEdit::textChanged, this, &MainWindow::onStartPointChanged);
    connect(ui->lineEditEnd, &QLineEdit::textChanged, this, &MainWindow::onEndPointChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Параметры круга
    QPointF center(350, 350); // Центр круга
    int radius = 300;         // Радиус круга

    // Рисуем внешний круг
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(center, radius, radius);

    if (!generatedPoints.isEmpty()) {
    // Рисуем точки
        painter.setFont(QFont("Arial", 8));
        for (int i = 0; i < generatedPoints.size(); ++i) {
            const QPointF &point = generatedPoints[i];

            if (selectedPoints.contains(i)) {
                painter.setBrush(Qt::yellow); // Выбранные точки подсвечиваются
            } else {
                painter.setBrush(Qt::blue);
            }

            painter.setPen(Qt::NoPen);
            painter.drawEllipse(point, 5, 5); // Увеличенный размер для выбранных точек

            // Номера точек
            painter.setPen(Qt::black);
            painter.drawText(point + QPointF(5, -5), QString::number(i + 1));
        }


    // Соединяем точки линиями
        painter.setPen(QPen(Qt::red, 1));
        for (const QPair<int, int> &connection : connections) {
            painter.drawLine(generatedPoints[connection.first], generatedPoints[connection.second]);
        }


        // Рисуем кратчайший путь, если выбраны две точки
        if (selectedPoints.size() == 2) {
            auto [path, totalDistance] = findShortestPath(selectedPoints[0], selectedPoints[1]);
            painter.setPen(QPen(Qt::green, 2)); // Зелёный цвет для пути

            for (int i = 1; i < path.size(); ++i) {
                painter.drawLine(generatedPoints[path[i - 1]], generatedPoints[path[i]]);
            }

            // Отображаем стоимость пути
            double cost = totalDistance * 10.0;
            QString costText = QString("Total Cost: %1 USD").arg(cost, 0, 'f', 2);

            painter.setPen(Qt::black);
            painter.setFont(QFont("Arial", 12, QFont::Bold));
            painter.drawText(rect(), Qt::AlignTop | Qt::AlignCenter, costText);
        }
    }
}

void MainWindow::onGeneratePoints()
{

    generatedPoints = generateRandomPoints(100, 300, QPointF(350, 350));// Генерация точек
    connections = generateConnections(generatedPoints);// Генерация соединений

    // Построение графа
    adjacencyList.clear();
    for (const QPair<int, int> &connection : connections) {
        adjacencyList[connection.first].append(connection.second);
        adjacencyList[connection.second].append(connection.first); // Граф неориентированный
    }

    update();
}

QVector<QPointF> MainWindow::generateRandomPoints(int count, int radius, QPointF center)
{
    QVector<QPointF> points;

    for (int i = 0; i < count; ++i) {
        double angle = static_cast<double>(rand()) / RAND_MAX * 2 * M_PI; // Угол в радианах
        double r = sqrt(static_cast<double>(rand()) / RAND_MAX) * radius; // Радиус для равномерного распределения
        double x = center.x() + r * cos(angle);
        double y = center.y() + r * sin(angle);
        points.append(QPointF(x, y));
    }

    return points;
}

void MainWindow::drawConnections(QPainter &painter, const QVector<QPointF> &points)
{
    for (const QPointF &point : points) {
        int connections = 2 + rand() % 5; // От 2 до 6 соединений
        QVector<int> connectedIndices;

        while (connectedIndices.size() < connections) {
            int index = rand() % points.size();
            if (index != points.indexOf(point) && !connectedIndices.contains(index)) {
                painter.drawLine(point, points[index]);
                connectedIndices.append(index);
            }
        }
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // Найти ближайшую точку
    const int clickRadius = 5; // Радиус клика
    for (int i = 0; i < generatedPoints.size(); ++i) {
        if (QLineF(event->pos(), generatedPoints[i]).length() <= clickRadius) {
            if (!selectedPoints.contains(i)) {
                if (selectedPoints.size() < 2) {
                    selectedPoints.append(i); // Выбираем точку
                    updatePointDisplays(); // Обновляем отображение
                }
            } else {
                selectedPoints.removeOne(i); // Убираем точку из выбранных
                updatePointDisplays(); // Обновляем отображение
            }
            update(); // Перерисовываем окно
            return;
        }
    }
}
void MainWindow::clearSelectedPoints()
{
    // Очищаем список выбранных точек
    selectedPoints.clear();

    // Очищаем текст в полях ввода
    ui->lineEditStart->clear();
    ui->lineEditEnd->clear();

    // Обновляем отображение, не вызывая валидацию
    updatePointDisplays(); // Обновляем отображение
    update(); // Перерисовываем окно
}
QVector<QPair<int, int>> MainWindow::generateConnections(const QVector<QPointF> &points)
{
    QVector<QPair<int, int>> result;

    for (int i = 0; i < points.size(); ++i) {
        int connectionsCount = 2 + rand() % 5; // От 2 до 6 соединений
        QSet<int> connectedIndices;

        while (connectedIndices.size() < connectionsCount) {
            int index = rand() % points.size();
            if (index != i && !connectedIndices.contains(index)) {
                connectedIndices.insert(index);
                result.append(qMakePair(i, index));
            }
        }
    }

    return result;
}

QPair<QVector<int>, double> MainWindow::findShortestPath(int start, int end)
{
    QMap<int, double> distances; // Расстояния до всех точек
    QMap<int, int> previous;    // Предыдущая точка в пути
    QSet<int> unvisited;        // Непосещённые вершины

    // Инициализация
    for (int i = 0; i < generatedPoints.size(); ++i) {
        distances[i] = std::numeric_limits<double>::max(); // "Бесконечность"
        unvisited.insert(i);
    }
    distances[start] = 0;

    while (!unvisited.isEmpty()) {
        // Найти точку с минимальным расстоянием
        int current = *std::min_element(unvisited.begin(), unvisited.end(),
                                        [&distances](int a, int b) { return distances[a] < distances[b]; });

        if (current == end) {
            break; // Найден путь
        }

        unvisited.remove(current);

        // Обновление соседей
        for (int neighbor : adjacencyList[current]) {
            if (unvisited.contains(neighbor)) {
                double alt = distances[current] + QLineF(generatedPoints[current], generatedPoints[neighbor]).length();
                if (alt < distances[neighbor]) {
                    distances[neighbor] = alt;
                    previous[neighbor] = current;
                }
            }
        }
    }

    // Построение пути
    QVector<int> path;
    double totalDistance = 0.0;
    for (int at = end; previous.contains(at); at = previous[at]) {
        totalDistance += QLineF(generatedPoints[at], generatedPoints[previous[at]]).length();
        path.prepend(at);
    }
    path.prepend(start);

    return qMakePair(path, totalDistance);
}

void MainWindow::displayPathInWindow()
{
    if (generatedPoints.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала сгенерируйте точки.");
        return;
    }
    if (selectedPoints.size() != 2) {
        QMessageBox::warning(this, "Ошибка", "Выберите ровно две точки.");
        return;
    }
    if (selectedPoints[0]==selectedPoints[1]) {
        QMessageBox::warning(this, "Ошибка", "Выберите две точки.");
        return;
    }
    auto [pathIndices, totalDistance] = findShortestPath(selectedPoints[0], selectedPoints[1]);

    // Преобразуем индексы в координаты точек
    QVector<QPointF> pathPoints;
    for (int index : pathIndices) {
        pathPoints.append(generatedPoints[index]);
    }

    // Вычисляем стоимость пути
    double totalCost = totalDistance * 10.0;

    // Передаём данные в PathWindow (координаты и индексы)
    pathWindow->setPath(pathPoints, pathIndices, totalCost);

    // Отображаем окно с графом
    pathWindow->move(this->geometry().center() - pathWindow->rect().center());
    pathWindow->show();
}



void MainWindow::updatePointDisplays()
{
    // Обновляем текст в полях для ввода
    if (selectedPoints.size() > 0) {
        ui->lineEditStart->setText(QString::number(selectedPoints[0] + 1)); // +1 для отображения в 1-based индексации
    } else {
        ui->lineEditStart->clear();
    }

    if (selectedPoints.size() > 1) {
        ui->lineEditEnd->setText(QString::number(selectedPoints[1] + 1)); // +1 для отображения в 1-based индексации
    } else {
        ui->lineEditEnd->clear();
    }
}



void MainWindow::onStartPointChanged()
{
    QString startText = ui->lineEditStart->text();

    // Если поле пустое, не выполняем валидацию
    if (startText.isEmpty()) {
        selectedPoints.clear();
        updatePointDisplays(); // Обновляем отображение
        update(); // Перерисовываем окно
        return;
    }

    bool startOk;
    int start = startText.toInt(&startOk) - 1; // Преобразуем в 0-based индекс

    // Проверяем, чтобы индекс был в пределах допустимых значений
    if (startOk && start >= 0 && start < generatedPoints.size()) {
        // Если новый стартовый индекс отличается от текущего, обновляем
        if (selectedPoints.size() > 0) {
            selectedPoints[0] = start;
        } else {
            selectedPoints.append(start);
        }

        // Запускаем расчет пути
        if (selectedPoints.size() == 2) {
            updatePointDisplays(); // Обновляем отображение
        }
    } else {
        // Если введен неправильный индекс, очищаем путь
        ui->lineEditStart->setText("");  // Очищаем поле
        if (selectedPoints.size() > 1) {
            selectedPoints[0] = selectedPoints[1];
        } else {
            selectedPoints.clear();
        }
    }
    update(); // Перерисовываем окно
}

void MainWindow::onEndPointChanged()
{
    QString endText = ui->lineEditEnd->text();

    // Если поле пустое, не выполняем валидацию
    if (endText.isEmpty()) {
        selectedPoints.clear();
        updatePointDisplays(); // Обновляем отображение
        update(); // Перерисовываем окно
        return;
    }

    bool endOk;
    int end = endText.toInt(&endOk) - 1; // Преобразуем в 0-based индекс

    // Проверяем, чтобы индекс был в пределах допустимых значений
    if (endOk && end >= 0 && end < generatedPoints.size()) {
        // Если новый конечный индекс отличается от текущего, обновляем
        if (selectedPoints.size() > 1) {
            selectedPoints[1] = end;
        } else if (selectedPoints.size() > 0) {
            selectedPoints.append(end);
        } else {
            selectedPoints.append(end);
            selectedPoints.append(end);
        }

        // Запускаем расчет пути
        if (selectedPoints.size() == 2) {
            updatePointDisplays(); // Обновляем отображение
        }
    } else {
        // Если введен неправильный индекс, очищаем путь
        ui->lineEditEnd->setText("");  // Очищаем поле
        selectedPoints.remove(1);
    }
    update(); // Перерисовываем окно
}


