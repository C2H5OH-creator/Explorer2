#include "mainwindow.h"

#include <QApplication>
#include <QString>

#include <iostream>
#include "list.h"

//Значение и связи ячеек
element::element(QString value) {
    this->value = value;

    prev = nullptr;
    next = nullptr;
}

//Создание пустого списка
List::List() {
    head = nullptr;
    tail = nullptr;
    len = 0;
}

//Добавление элемента в начало списка
element* List::add_to_start(QString value) {
    element* newElement = new element(value); // Создаём новый элемент
    newElement->next = head; // Связываем новый элемент с текущим началом списка
    newElement->prev = nullptr; // Новый элемент становится началом списка, поэтому у него нет предыдущего элемента

    if (head != nullptr) {
        head->prev = newElement; // Обновляем связь предыдущего начального элемента с новым элементом
    }
    else {
        tail = newElement; // Если список был пуст, новый элемент также становится концом списка
    }

    head = newElement; // Обновляем указатель на начало списка
    len++;
    return head; // Возвращаем указатель на новый начальный элемент
}

//Добавление элемента в конец списка
element* List::add_to_end(QString value) {
    element* newElement = new element(value);
    newElement->prev = tail;
    newElement->next = nullptr;

    if (tail != nullptr) { tail->next = newElement; }
    else { head = newElement; }
    tail = newElement;
    len++;
    return tail;
}

//Удаление списка
void List::delete_list() {
    element* current = head; // Начинаем с начала списка
    while (current != nullptr) {
        element* next = current->next; // Сохраняем ссылку на следующий элемент
        delete current; // Удаляем текущий элемент
        current = next; // Переходим к следующему элементу
    }
    head = nullptr;
    tail = nullptr;
    len = 0;
}

//Обычная печать списка
QString List::print() {
    element* current = head;
    QString output = "";
    output += "[";
    while (current != nullptr) {
        output += current->value;
        if (current->next != nullptr) {
            output += ", ";
        }
        current = current->next;
    }
    output += "]\n";
    return output;
}

//Обычная печать списка
void List::lineEdit_print() {
    element* current = head;
    //("[");
    while (current != nullptr) {
        //std::cout << current->value;
        if (current->next != nullptr) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]\n");
}


//Реверс печать списка
void List::rprint() {
    element* current = tail;
    printf("[");
    while (current != nullptr) {
        //printf("%d", current->value);
        if (current->prev != nullptr) {
            printf(", ");
        }
        current = current->prev;
    }
    printf("]\n");
}

//Замена числа по индексу слева
QString List::replace_value(int input_index, QString value) {
    element* current = head;
    long index = 0;
    while (current != nullptr) {
        if (index == input_index) {
            current->value = value;
            break;
        }
        index++;
        current = current->next;
    }
    return current->value;
}

//Замена числа по индексу справа
QString List::rreplace_value(int input_index, QString value) {
    element* current = tail;
    int rindex = 0;
    while (current != nullptr) {
        if (rindex == input_index) {
            current->value = value;
            break;
        }
        rindex++;
        current = current->prev;
    }
    return current->value;
}

//Длина
long long List::length() {
    return len;
}

//Индекс справа
long long List::index(QString value) {
    element* current = head;
    int index = 0;
    int no_coincidences = 0;
    while (current != nullptr) {
        if (current->value == value) {
            return index;
            break;
        }
        index++;
        no_coincidences++;
        current = current->next;
        if (no_coincidences == length()) {
            return -1;
            break;
        }
    }
}

//Индекс слева
long long List::rindex(QString value) {
    element* current = tail;
    int index = 0;
    int no_coincidences = 0;
    while (current != nullptr) {
        if (current->value == value) {
            return index;
            break;
        }
        index++;
        current = current->prev;
        no_coincidences++;
        if (no_coincidences == length()) {
            return -1;
            break;
        }
    }
}

// Сортировка списка по возрастанию
void List::sort_ascending() {
    if (head == nullptr || head->next == nullptr) {
        // Если список пуст или содержит только один элемент, то он уже отсортирован по возрастанию
        return;
    }

    element* current = head->next;
    while (current != nullptr) {
        element* key = current;
        QString value = key->value;
        element* prev = key->prev;

        // Перемещаем элементы, пока не найдем правильную позицию для вставки текущего элемента
        while (prev != nullptr && prev->value > value) {
            key->value = prev->value;
            key = prev;
            prev = prev->prev;
        }

        key->value = value;
        current = current->next;
    }
}

// Сортировка списка по убыванию
void List::sort_descending() {
    if (head == nullptr || head->next == nullptr) {
        // Если список пуст или содержит только один элемент, то он уже отсортирован по убыванию
        return;
    }

    element* current = head->next;
    while (current != nullptr) {
        element* key = current;
        QString value = key->value;
        element* prev = key->prev;

        // Перемещаем элементы, пока не найдем правильную позицию для вставки текущего элемента
        while (prev != nullptr && prev->value < value) {
            key->value = prev->value;
            key = prev;
            prev = prev->prev;
        }

        key->value = value;
        current = current->next;
    }
}

//Возвращает указатель на элемент по индексу справа
element* List::get_index(long index) {
    element* current = head;
    long temp_index = 0;
    long no_coincidences = 0;
    while (current != nullptr) {
        if (temp_index == index) {
            return current;
            break;
        }
        temp_index++;
        no_coincidences++;
        current = current->next;
        if (no_coincidences == length()) {
            return nullptr;
            break;
        }
    }
}

//Возвращает указатель на элемент по индексу слева
element* List::get_rindex(long rindex) {
    element* current = tail;
    long temp_index = 0;
    long no_coincidences = 0;
    while (current != nullptr) {
        if (temp_index == rindex) {
            return current;
            break;
        }
        temp_index++;
        no_coincidences++;
        current = current->prev;
        if (no_coincidences == length()) {
            return nullptr;
            break;
        }
    }
}

//Вставка элемента по индексу справа
void List::insert_value(long index, QString value) {
    long temp_index = 0;
    element* newElement = new element(value); //Новый элемент
    element* prev_el = get_index(index - 1); //Указатель на элемент справа от которого вставка
    element* next_el = get_index(index + 1); //Указатель на элемент слева от которого вставка

    if (prev_el == nullptr || next_el == nullptr) {
        // Невозможно вставить элемент
        return ;
    }
    //Обрываем связи между (n - 1) и (n + 1) элементами
    prev_el->next = nullptr;
    next_el->prev = nullptr;
    //Привязываем (n - 1) и (n + 1) элементы к n-ому элементу
    prev_el->next = newElement;
    newElement->prev = prev_el;
    newElement->next = next_el;
    next_el->prev = newElement;
    len++;
}

//Вставка элемента по индексу слева
void List::rinsert_value(long index, QString value) {
    long temp_index = 0;
    element* newElement = new element(value); //Новый элемент
    element* prev_el = get_rindex(index + 1); //Указатель на элемент справа от которого вставка
    element* next_el = get_rindex(index - 1); //Указатель на элемент слева от которого вставка

    if (prev_el == nullptr || next_el == nullptr) {
        // Невозможно вставить элемент
        return;
    }

    //Обрываем связи между (n - 1) и (n + 1) элементами
    prev_el->next = nullptr;
    next_el->prev = nullptr;
    //Привязываем (n - 1) и (n + 1) элементы к n-ому элементу
    prev_el->next = newElement;
    newElement->prev = prev_el;
    newElement->next = next_el;
    next_el->prev = newElement;
    len++;
}

//Вырезка элемента по индексу
long List::cut_value(long index) {
    long temp_index = 0;
    QString temp_val = "";
    element* curr_el = get_index(index);  //Нужный элемент
    element* prev_el = get_index(index - 1); //Указатель на элемент справа от которого вставка
    element* next_el = get_index(index + 1); //Указатель на элемент слева от которого вставка

    if (curr_el == nullptr) {
        // Невозможно вырезать элемент
        return NULL;
    }

    temp_val = curr_el->value; // Сохраняем значение элемента, которое нужно вернуть

    //Отвязываем n-ый элемент от списка
    if (prev_el != nullptr) prev_el->next = next_el;
    if (next_el != nullptr) next_el->prev = prev_el;

    curr_el->next = nullptr;
    curr_el->prev = nullptr;
    delete curr_el;

    len--; // Уменьшаем длину списка
    //return temp_val;
}

//Элемент по индексу слева
QString List::get(long input_index) {
    element* current = head;
    int temp_index = 0;
    int no_coincidences = 0;
    while (current != nullptr) {
        if (temp_index == input_index) {
            return current->value;
            break;
        }
        temp_index++;
        no_coincidences++;
        current = current->next;
        if (no_coincidences == length()) {
            return NULL;
            break;
        }
    }
}

//Элемент по индексу справа
QString List::rget(long input_index) {
    element* current = tail;
    int temp_index = 0;
    int no_coincidences = 0;
    while (current != nullptr) {
        if (temp_index == input_index) {
            return current->value;
            break;
        }
        temp_index++;
        no_coincidences++;
        current = current->prev;
        if (no_coincidences == length()) {
            return NULL;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
