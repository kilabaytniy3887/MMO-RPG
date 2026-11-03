#include <iostream>
#include <clocale>
#include "GameField.h"
#include "UnitFactory.h"

void fillTestField(GameField& field, UnitFactory& dmgFactory, UnitFactory& tankFactory, UnitFactory& flyFactory) {
    field.populateField(dmgFactory, "Mage", 1, 1);
    field.populateField(tankFactory, "Paladin", 4, 2);
    field.populateField(flyFactory, "Pegas", 7, 4);
    field.populateField(dmgFactory, "Berserk", 2, 5);
}

int main() {
    std::setlocale(LC_ALL, "Russian");

    GameField field(10, 10, 5);
    DamageFactory dmgFactory;
    TankFactory tankFactory;
    FlyingFactory flyFactory;

    int choice = -1;

    do {
        std::cout << "\n_____________________________________________\n";
        std::cout << "Какой сценарий вы хотите запустить?\n";
        std::cout << "Сценарий 1: Создание поля и наполнение\n";
        std::cout << "Сценарий 2: Глубокое копирование\n";
        std::cout << "Сценарий 3: Итерация по юнитам\n";
        std::cout << "Сценарий 4: Бой\n";
        std::cout << "Сценарий 5: Перемещение поля\n";
        std::cout << "Сценарий 6: Свободнаяя игра\n";
        std::cout << "Сценарий 0: Окончание работы программы\n";
        std::cout << "Ваш выбор: ";

        std::cin >> choice;

        switch (choice) {
        case 1: {
            std::cout << "Сценарий 1: Создание поля и наполнение\n";
            field.clear();
            fillTestField(field, dmgFactory, tankFactory, flyFactory);
            std::cout << "Юнитов на поле: " << field.size() << "\n";
            field.printField();
            break;
        }

        case 2: {
            std::cout << "Сценарий 2: Глубокое копирование\n";
            field.clear();
            fillTestField(field, dmgFactory, tankFactory, flyFactory);

            GameField copyField = field;

            if (field.getUnit(0)) {
                field.getUnit(0)->move(5, 5);

                std::cout << "Оригинал: юнит[0] = (" << field.getUnit(0)->getX()
                    << ", " << field.getUnit(0)->getY() << ")\n";
                std::cout << "Копия:    юнит[0] = (" << copyField.getUnit(0)->getX()
                    << ", " << copyField.getUnit(0)->getY() << ")\n";
                std::cout << "Копия независима от оригинала.\n";
            }
            break;
        }

        case 3: {
            std::cout << "Сценарий 3: Итерация по юнитам\n";
            if (field.empty()) {
                fillTestField(field, dmgFactory, tankFactory, flyFactory);
            }

            for (auto it = field.begin(); it != field.end(); ++it) {
                Unit* u = *it;
                std::cout << "  " << u->getType()
                    << " (" << u->getX() << ", " << u->getY() << ")"
                    << " HP = " << u->getHealth().getValue() << '\n';
            }
            break;
        }

        case 4: {
            std::cout << "Сценарий 4: Бой\n";
            field.clear();
            fillTestField(field, dmgFactory, tankFactory, flyFactory);

            Unit* attacker = field.getUnit(0);
            Unit* defender = field.getUnit(1);

            if (attacker && defender) {
                std::cout << attacker->getType() << " атакует "
                    << defender->getType() << '\n';
                std::cout << "HP защитника до:   "
                    << defender->getHealth().getValue() << '\n';
                int dmg = attacker->attackUnit(*defender);
                std::cout << "Нанесён урон:      " << dmg << '\n';
                std::cout << "HP защитника после:"
                    << ' ' << defender->getHealth().getValue() << "\n";
            }
            else {
                std::cout << "Ошибка: недостаточно юнитов для боя!\n";
            }
            break;
        }

        case 5: {
            std::cout << "Сценарий 5: Перемещение поля\n";
            field.clear();
            fillTestField(field, dmgFactory, tankFactory, flyFactory);

            GameField movedField = std::move(field);

            std::cout << "Размер нового поля: " << movedField.size() << '\n';
            std::cout << "Размер старого:     " << field.size() << "\n\n";

            std::cout << "Новое поле:\n";
            movedField.printField();
            std::cout << "\nСтарое (пустое) поле:\n";
            field.printField();
            break;
        }

        case 6: {
            std::cout << "Сценарий 6: Свободная игра \n";
            int action = -1;

            do {
                std::cout << "\n Меню свободной игры \n";
                std::cout << "1. Показать поле\n";
                std::cout << "2. Добавить юнита\n";
                std::cout << "3. Переместить юнита\n";
                std::cout << "4. Атаковать\n";
                std::cout << "5. Удалить юнита\n";
                std::cout << "6. Список юнитов (индексы)\n";
                std::cout << "0. Вернуться в главное меню\n";
                std::cout << "Ваш выбор: ";
                std::cin >> action;

                switch (action) {
                case 1: {
                    field.printField();
                    break;
                }
                case 2: {
                    std::string type;
                    int x, y;
                    std::cout << "Доступные типы: Mage, Berserk, Paladin, Vampir, Pegas, Dark Knight\n";
                    std::cout << "Введите тип юнита: ";
                    std::cin.ignore();
                    std::getline(std::cin, type);

                    std::cout << "Введите X и Y: ";
                    std::cin >> x >> y;

                    if (type == "Mage" || type == "Berserk") {
                        field.populateField(dmgFactory, type, x, y);
                    }
                    else if (type == "Paladin" || type == "Vampir") {
                        field.populateField(tankFactory, type, x, y);
                    }
                    else if (type == "Pegas" || type == "Dark Knight") {
                        field.populateField(flyFactory, type, x, y);
                    }
                    else {
                        std::cout << "Ошибка: неизвестный тип юнита!\n";
                    }
                    break;
                }
                case 3: {
                    int index, newX, newY;
                    std::cout << "Введите индекс юнита для перемещения: ";
                    std::cin >> index;
                    Unit* u = field.getUnit(index);
                    if (u) {
                        std::cout << "Введите новые X и Y: ";
                        std::cin >> newX >> newY;
                        u->move(newX, newY);
                        std::cout << "Юнит успешно перемещен.\n";
                    }
                    else {
                        std::cout << "Ошибка.\n";
                    }
                    break;
                }
                case 4: {
                    int atkIdx, defIdx;
                    std::cout << "Введите индекс атакующего: ";
                    std::cin >> atkIdx;
                    std::cout << "Введите индекс защитника: ";
                    std::cin >> defIdx;

                    Unit* attacker = field.getUnit(atkIdx);
                    Unit* defender = field.getUnit(defIdx);

                    if (attacker && defender) {
                        int dmg = attacker->attackUnit(*defender);
                        std::cout << attacker->getType() << " атаковал " << defender->getType() << "\n";
                        std::cout << "Нанесено урона: " << dmg << "\n";
                        std::cout << "HP защитника: " << defender->getHealth().getValue() << "\n";

                        if (defender->getHealth().isZero()) {
                            std::cout << defender->getType() << " погиб и был удален с поля!\n";
                            field.removeUnit(defIdx);
                        }
                    }
                    else {
                        std::cout << "Ошибка: неверные индексы юнитов.\n";
                    }
                    break;
                }
                case 5: {
                    int index;
                    std::cout << "Введите индекс юнита для удаления: ";
                    std::cin >> index;
                    field.removeUnit(index);
                    std::cout << "Юнит удален (если он существовал).\n";
                    break;
                }
                case 6: {
                    std::cout << "\nСписок юнитов на поле:\n";
                    for (size_t i = 0; i < field.size(); ++i) {
                        Unit* u = field.getUnit(i);
                        std::cout << "[" << i << "] " << u->getType()
                            << " | Позиция: (" << u->getX() << ", " << u->getY() << ")"
                            << " | HP: " << u->getHealth().getValue() << "\n";
                    }
                    if (field.empty()) std::cout << "Поле пусто.\n";
                    break;
                }
                case 0: {
                    std::cout << "Возврат в главное меню...\n";
                    break;
                }
                default: {
                    std::cout << "Неверный выбор!\n";
                    break;
                }
                }
            } while (action != 0);

            break;
        }

        case 0: {
            std::cout << "Выход из программы...\n";
            break;
        }

        default: {
            std::cout << "Неверный выбор, введите число от 0 до 5.\n";
            break;
        }
        }
    } while (choice != 0);

    return 0;
}