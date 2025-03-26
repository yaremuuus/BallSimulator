#include "BallSimulator.h"

BasketGame::BasketGame(QWidget *parent) : QMainWindow(parent) {
    // Инициализация интерфейса
    setupUI();
    // Подключение сигналов кнопок
    connectSignals();
    // Обновление отображаемых данных
    updateDisplay();
}

void BasketGame::setupUI() {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // Поля ввода начальных значений
        QGroupBox *initGroup = new QGroupBox("Начальные значения");
        QFormLayout *initLayout = new QFormLayout();

    b1BlueInput = new QLineEdit("10");
    b1RedInput = new QLineEdit("10");
    b2BlueInput = new QLineEdit("3");
    b2RedInput = new QLineEdit("7");
    initBtn = new QPushButton("Установить значения");

    initLayout->addRow("Синие в корзине 1:", b1BlueInput);
    initLayout->addRow("Красные в корзине 1:", b1RedInput);
    initLayout->addRow("Синие в корзине 2:", b2BlueInput);
    initLayout->addRow("Красные в корзине 2:", b2RedInput);
    initLayout->addRow(initBtn);
    initGroup->setLayout(initLayout);

        // Корзина 1
        QGroupBox *basket1Group = new QGroupBox("Корзина 1");
        QVBoxLayout *basket1Layout = new QVBoxLayout();
        basket1Info = new QLabel();
        move1to2Btn = new QPushButton("Переложить шар в корзину 2");
        basket1Layout->addWidget(basket1Info);
        basket1Layout->addWidget(move1to2Btn);
        basket1Group->setLayout(basket1Layout);

        // Корзина 2
        QGroupBox *basket2Group = new QGroupBox("Корзина 2");
        QVBoxLayout *basket2Layout = new QVBoxLayout();
        basket2Info = new QLabel();
        move2to1Btn = new QPushButton("Переложить шар в корзину 1");
        basket2Layout->addWidget(basket2Info);
        basket2Layout->addWidget(move2to1Btn);
        basket2Group->setLayout(basket2Layout);

    // Вероятности
    QGroupBox *probGroup = new QGroupBox("Вероятности");
    QVBoxLayout *probLayout = new QVBoxLayout();
    prob2Blue = new QLabel("Вероятность 2 синих: 0%");
    prob2Red = new QLabel("Вероятность 2 красных: 0%");
    prob1Each = new QLabel("Вероятность 1 синий и 1 красный: 0%");
    remove2Btn = new QPushButton("Извлечь 2 шара");
    probLayout->addWidget(prob2Blue);
    probLayout->addWidget(prob2Red);
    probLayout->addWidget(prob1Each);
    probLayout->addWidget(remove2Btn);
    probGroup->setLayout(probLayout);

    mainLayout->addWidget(initGroup);
    mainLayout->addWidget(basket1Group);
    mainLayout->addWidget(basket2Group);
    mainLayout->addWidget(probGroup);
        setCentralWidget(centralWidget);

        // Установка начальных значений
        basket1.total = 20;
        basket1.blue = 10;
        basket1.red = 10;
        basket2.total = 10;
        basket2.blue = 3;
        basket2.red = 7;
    }

void BasketGame::connectSignals() {
    connect(initBtn, &QPushButton::clicked, this, &BasketGame::setInitialValues);
    connect(move1to2Btn, &QPushButton::clicked, this, &BasketGame::moveFrom1To2);
    connect(move2to1Btn, &QPushButton::clicked, this, &BasketGame::moveFrom2To1);
    connect(remove2Btn, &QPushButton::clicked, this, &BasketGame::removeTwoBalls);
}

void BasketGame::setInitialValues() {
    bool ok;
    basket1.blue = b1BlueInput->text().toInt(&ok);
    if (!ok || basket1.blue < 0) basket1.blue = 0;

    basket1.red = b1RedInput->text().toInt(&ok);
    if (!ok || basket1.red < 0) basket1.red = 0;

    basket2.blue = b2BlueInput->text().toInt(&ok);
    if (!ok || basket2.blue < 0) basket2.blue = 0;

    basket2.red = b2RedInput->text().toInt(&ok);
    if (!ok || basket2.red < 0) basket2.red = 0;

    basket1.total = basket1.blue + basket1.red;
    basket2.total = basket2.blue + basket2.red;

    basket1.lastAction = "Значения установлены";
    basket2.lastAction = "Значения установлены";

    updateDisplay();
}


void BasketGame::updateDisplay() {
    // Обновление информации о корзине 1
    QString basket1Text = QString(
        "Шаров в корзине: %1\n"
        "Синих шаров: %2\n"
        "Красных шаров: %3\n"
        "Вероятность синего: %4%\n"
        "Вероятность красного: %5%\n"
        "Последнее действие: %6"
    ).arg(basket1.total).arg(basket1.blue).arg(basket1.red)
     .arg(basket1.total ? basket1.blue * 100 / basket1.total : 0)
     .arg(basket1.total ? basket1.red * 100 / basket1.total : 0)
     .arg(basket1.lastAction);
    basket1Info->setText(basket1Text);

    // Обновление информации о корзине 2
    QString basket2Text = QString(
        "Шаров в корзине: %1\n"
        "Синих шаров: %2\n"
        "Красных шаров: %3\n"
        "Вероятность синего: %4%\n"
        "Вероятность красного: %5%\n"
        "Последнее действие: %6"
    ).arg(basket2.total).arg(basket2.blue).arg(basket2.red)
     .arg(basket2.total ? basket2.blue * 100 / basket2.total : 0)
     .arg(basket2.total ? basket2.red * 100 / basket2.total : 0)
     .arg(basket2.lastAction);
    basket2Info->setText(basket2Text);

    // Расчет вероятностей для извлечения 2 шаров
    calculateProbabilities();
}

void BasketGame::calculateProbabilities() {
        int totalBalls = basket1.total + basket2.total;
        if (totalBalls < 2) {
            prob2Blue->setText("Вероятность достать 2 синих шара: 0%");
            prob2Red->setText("Вероятность достать 2 красных шара: 0%");
            prob1Each->setText("Вероятность достать 1 красный и 1 синий шар: 0%");
            return;
        }

        // Вероятности для разных сценариев
        double p2Blue = 0.0, p2Red = 0.0, p1Each = 0.0;

        // Сценарий 1: Обе корзины не пустые
        if (basket1.total > 0 && basket2.total > 0) {
            // Вероятность выбрать корзину 1 или 2
            double pB1 = (double)basket1.total / totalBalls;
            double pB2 = (double)basket2.total / totalBalls;

            // Добавляем вероятности для комбинаций из разных корзин
            p2Blue += 2 * pB1 * pB2 * ((double)basket1.blue / basket1.total) * ((double)basket2.blue / basket2.total);
            p2Red += 2 * pB1 * pB2 * ((double)basket1.red / basket1.total) * ((double)basket2.red / basket2.total);
            p1Each += pB1 * pB2 * (
                ((double)basket1.blue / basket1.total) * ((double)basket2.red / basket2.total) +
                ((double)basket1.red / basket1.total) * ((double)basket2.blue / basket2.total)
            );
        }

        // Сценарий 2: Достаем 2 шара из корзины 1
        if (basket1.total >= 2) {
            double p = (double)(basket1.total * (basket1.total - 1)) / (totalBalls * (totalBalls - 1));
            p2Blue += p * (basket1.blue * (basket1.blue - 1)) / (basket1.total * (basket1.total - 1));
            p2Red += p * (basket1.red * (basket1.red - 1)) / (basket1.total * (basket1.total - 1));
            p1Each += p * (2 * basket1.blue * basket1.red) / (basket1.total * (basket1.total - 1));
        }

        // Сценарий 3: Достаем 2 шара из корзины 2
        if (basket2.total >= 2) {
            double p = (double)(basket2.total * (basket2.total - 1)) / (totalBalls * (totalBalls - 1));
            p2Blue += p * (basket2.blue * (basket2.blue - 1)) / (basket2.total * (basket2.total - 1));
            p2Red += p * (basket2.red * (basket2.red - 1)) / (basket2.total * (basket2.total - 1));
            p1Each += p * (2 * basket2.blue * basket2.red) / (basket2.total * (basket2.total - 1));
        }

        // Обновляем интерфейс
        prob2Blue->setText(QString("Вероятность достать 2 синих шара: %1%").arg(p2Blue * 100, 0, 'f', 1));
        prob2Red->setText(QString("Вероятность достать 2 красных шара: %1%").arg(p2Red * 100, 0, 'f', 1));
        prob1Each->setText(QString("Вероятность достать 1 красный и 1 синий шар: %1%").arg(p1Each * 100, 0, 'f', 1));
    }


void BasketGame::moveFrom1To2() {
    if (basket1.total == 0) {
        QMessageBox::warning(this, "Ошибка", "Корзина 1 пуста!");
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, basket1.total);

    bool isBlue = dis(gen) <= basket1.blue;
    if (isBlue) {
        basket1.blue--;
        basket2.blue++;
        basket1.lastAction = "Переложили синий шар в корзину 2";
    } else {
        basket1.red--;
        basket2.red++;
        basket1.lastAction = "Переложили красный шар в корзину 2";
    }
    basket1.total--;
    basket2.total++;

    updateDisplay();
}

void BasketGame::moveFrom2To1() {
    if (basket2.total == 0) {
        QMessageBox::warning(this, "Ошибка", "Корзина 2 пуста!");
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, basket2.total);

    bool isBlue = dis(gen) <= basket2.blue;
    if (isBlue) {
        basket2.blue--;
        basket1.blue++;
        basket2.lastAction = "Переложили синий шар в корзину 1";
    } else {
        basket2.red--;
        basket1.red++;
        basket2.lastAction = "Переложили красный шар в корзину 1";
    }
    basket2.total--;
    basket1.total++;

    updateDisplay();
}

void BasketGame::removeTwoBalls() {
    int totalBalls = basket1.total + basket2.total;
    if (totalBalls < 2) {
        QMessageBox::warning(this, "Ошибка", "Недостаточно шаров для извлечения!");
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, totalBalls);

    // Первый шар
    int ball1 = dis(gen);
    bool fromBasket1 = ball1 <= basket1.total;
    bool isBlue1 = false;

    if (fromBasket1) {
        isBlue1 = ball1 <= basket1.blue;
        if (isBlue1) basket1.blue--;
        else basket1.red--;
        basket1.total--;
        basket1.lastAction = isBlue1 ? "Извлекли синий шар" : "Извлекли красный шар";
    } else {
        isBlue1 = (ball1 - basket1.total) <= basket2.blue;
        if (isBlue1) basket2.blue--;
        else basket2.red--;
        basket2.total--;
        basket2.lastAction = isBlue1 ? "Извлекли синий шар" : "Извлекли красный шар";
    }

    // Второй шар
    int ball2 = dis(gen);
    bool fromBasket2 = ball2 <= (fromBasket1 ? basket1.total : basket1.total + 1);
    bool isBlue2 = false;

    if (fromBasket2) {
        isBlue2 = ball2 <= basket1.blue;
        if (isBlue2) basket1.blue--;
        else basket1.red--;
        basket1.total--;
        basket1.lastAction = isBlue2 ? "Извлекли синий шар" : "Извлекли красный шар";
    } else {
        isBlue2 = (ball2 - basket1.total) <= basket2.blue;
        if (isBlue2) basket2.blue--;
        else basket2.red--;
        basket2.total--;
        basket2.lastAction = isBlue2 ? "Извлекли синий шар" : "Извлекли красный шар";
    }

    updateDisplay();
}

