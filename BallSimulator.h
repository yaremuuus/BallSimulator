
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QGroupBox>
#include <random>
#include <QFormLayout>

class BasketGame : public QMainWindow {
    Q_OBJECT

public:
    BasketGame(QWidget *parent = nullptr);

private:
    // Данные корзин
    struct Basket {
        int total = 0;
        int blue = 0;
        int red = 0;
        QString lastAction = "Нет действий";
    } basket1, basket2;

    // Элементы интерфейса
    QLabel *basket1Info, *basket2Info;
    QLabel *prob2Blue, *prob2Red, *prob1Each;
    QPushButton *move1to2Btn, *move2to1Btn, *remove2Btn;
    QLineEdit *b1BlueInput, *b1RedInput, *b2BlueInput, *b2RedInput;
    QPushButton *initBtn;

    void setupUI();

    void connectSignals();

    void setInitialValues();

    void updateDisplay();

    void calculateProbabilities();

private slots:
    void moveFrom1To2();

    void moveFrom2To1();

    void removeTwoBalls();
};
