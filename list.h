#pragma once
#include <QString>

class element;

class List {
public:
	List();
    element* add_to_start(QString value);
    element* add_to_end(QString value);
	void delete_list();
    QString print();
    void lineEdit_print();
	void rprint();
    QString replace_value(int input_index, QString value);
    QString rreplace_value(int input_index, QString value);
    long long length();
    long long index(QString value);
    QString get(long index);
    QString rget(long index);
    long long rindex(QString value);
	void sort_descending();
	void sort_ascending();
	element* get_index(long index);
	element* get_rindex(long rindex);
    void insert_value(long index, QString value);
	long cut_value(long index);
	long rcut_value(long rindex);
    void rinsert_value(long rindex, QString value);

private:
	element *head, *tail;
	unsigned len;
};

class element
{
public:
    QString value;
    element(QString value);

private:
	element* prev, * next;

    friend element* List::add_to_start(QString value);
    friend element* List::add_to_end(QString value);
	friend void List::delete_list();
    friend QString List::print();
    friend void List::lineEdit_print();
	friend void List::rprint();
    friend QString List::replace_value(int input_index, QString value);
    friend QString List::rreplace_value(int input_index, QString value);
	friend long long List::length();
    friend long long List::index(QString value);
    friend QString List::get(long index);
    friend QString List::rget(long index);
    friend long long List::rindex(QString value);
	friend void List::sort_descending();
	friend void List::sort_ascending();
	friend element* List::get_index(long index);
	friend element* List::get_rindex(long index);
    friend void List::insert_value(long index, QString value);
	friend long List::cut_value(long index);
	friend long List::rcut_value(long rindex);
    friend void List::rinsert_value(long rindex, QString value);
};
