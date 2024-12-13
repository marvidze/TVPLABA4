#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <list>
#include <algorithm>
using namespace std;

void open_fail(string&);
void proverka(string);
void proverka_start(string);
void proverka_variable(const vector<string>&);

int main()
{
    try {
        setlocale(LC_ALL, "ru");
        string str;
        open_fail(str);
        proverka(str);
    }
    catch (const char* msg)
    {
        cout << "\n��������� ������!" << endl << msg << endl << endl;
    }
}

void open_fail(string& str) {
    ifstream in("test.txt");
    if (in.is_open()) getline(in, str);
    in.close();
    str += ' ';
}

void proverka(string str) {
    cout << "��� ���� ���������:\n" << endl << str << endl;
    vector<string> words;
    string word;
    if (str.find(":") == -1) throw "��������� ��������� ':' ";
    if (str.find("INTEGER") == -1) throw "�����������/�������� ������������ ���� ������";
    if (str.find("INTEGER") - str.find(":") != 2) throw "������������� ����������(��) ��������� �� �����";
    int i = 0;
    while (str.length() != 0)
    {
        int pos = str.find(" ");
        words.push_back(word.assign(str, 0, pos));
        str.erase(0, pos + 1);
    }

    proverka_start(words[0]); //���� �� VAR � ����� ������
    proverka_variable(words); //�������� �� ��������� ����������
    cout << "\n��������� ��������� �������!\n";
}

void proverka_start(string first) {
    if (first != "VAR") throw "� ������ ����������� VAR";
}

void proverka_variable(const vector<string>& variables) {
    vector<list<string>> variables_in_operations;
    int count = 0;
    regex regular("[a-zA-Z_][a-zA-Z0-9_]{0,7}");
    vector<string> variables_for_work;
    int pos = -1;
    for (int i = 1; i < variables.size(); i++) {
        if (variables[i] == ":") { pos = i; break; }
    }
    if (pos < 3 || pos>14) throw "������������� ����������(��) ��������� �� �����";

    for (int i = 1; i < pos; i++) {
        if (!regex_match(variables[i], regular)) throw "���������� �� ��������� �������� ��� �����������";
        for (int j = 0; j < variables_for_work.size(); j++) {
            if (variables[i] == variables_for_work[j]) throw "������ ��������� 2 ���������� � ���������� ���������";
        }
        count++; variables_for_work.push_back(variables[i]);
        list<string> list_variable; list_variable.push_back(variables[i]); variables_in_operations.push_back(list_variable);

        if (regex_match(variables[i + 2], regular)) {
            if (variables[i + 1] != ",")  throw "��� �������� ���������� ���������� ��������� ������� (,) ����� ������ ����������";
        }
        i++;
    }

    int begin = pos + 2;//pos - ��� ��������� ����� INTEGER
    if (variables[begin - 1] != "INTEGER;") throw "����� INTEGER ��������� ����� � ������� ';'";
    if (variables[begin] != "BEGIN") throw "��������� BEGIN";
    if (variables[variables.size() - 1] != "END") throw "� ����� �� �������� END";

    int count_semicolon = 1; //���������� ����� � �������

    //�������� �� ������������ ������������ �������� ����������
    for (int i = begin + 1; i < variables.size() - 1; i++) {
        regex reg_for_math_operation("((^-?\\(?-?[1-9]+\\)?;?$)\+|0;?$)|\\*{1}|-{1}|\\+{1}");
        bool find = false; string its_variable; int list_index = -1;
        for (int j = 0; j < variables_for_work.size(); j++) {
            if (variables[i] == variables_for_work[j]) { find = true; its_variable = variables[i]; list_index = j; break; }
        }
        if (!find) throw "������ ���������� �� ���� ������������������� �����";
        i++;
        if (variables[i] != "=") throw "����������� ���� ������������ '='";

        do {
            i++;
            bool norm = false;
            if (variables[i].find(its_variable) != -1) throw "������ ��������� ���������� ����� ����";
            for (int j = 0; j < variables_for_work.size(); j++) {
                if (variables[i].find(variables_for_work[j]) != -1) { norm = true; variables_in_operations[list_index].push_back(variables_for_work[j]); break; }
            }


            if (!regex_match(variables[i], reg_for_math_operation)) {
                if (!norm) throw "������������ ���������� ��������� �������";
            }
        } while (variables[i].find(";") == -1);
        count_semicolon++;
    }
    if (count_semicolon != count + 1) throw "����� ���������� ������������ �������� ����������, ��������� ����� � ������� ';'";

    //�������� �� ������������ ���������� ����� ����
    for (int i = 0; i < variables_in_operations.size(); i++) {
        variables_in_operations[i].sort();
    }
    for (int i = 0; i < variables_in_operations.size(); i++) {
        static string _var;
        list<string> _list = variables_in_operations[i];
        if (_list.size() == 1) { _var = _list.front(); continue; }
        for (int k = i + 1; k < variables_in_operations.size(); k++) {
            list<string> next_list = variables_in_operations[k];
            if (next_list.size() == 1) { _var = next_list.front(); continue; }
            if (_list.front() == next_list.front()) {
                auto first = find(_list.begin(), _list.end(), _var);
                auto next = find(next_list.begin(), next_list.end(), _var);
                if (first == _list.end() || next == next_list.end())throw "���������� ����������� ���� �����";
            }
        }
    }
}
