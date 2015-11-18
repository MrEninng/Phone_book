#include "changeinfobutton_window.h"
#include "ui_changeinfobutton_window.h"
#include <QRegExpValidator>
#include <QRegExp>
#include <QMessageBox>


changeInfoButton_window::changeInfoButton_window(int id, QTableView *table, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changeInfoButton_window)
{
    ui->setupUi(this);
    current_id = id;

    //УСТАНОВКА ИКОНКИ ОКНА
    icon = new QIcon();
    icon->addFile(":/imgs/icon3.png");
    setWindowIcon(*icon);
    ui->numberLine->setInputMask("8(000)000-00-00");


    tableView = table; // указатель на таблицу чтобы менялись там данные
    // В данном блоке задается максимальная длина для полей заполнения
    // И валидаторы для всех кроме Адреса! для него он не нужен(вероятно)
    ui->nameLine->setMaxLength(15);
    ui->secondNameLine->setMaxLength(15);
    ui->surnameLine->setMaxLength(15);
    ui->addressLine->setMaxLength(30);

    ui->dateEdit->setDisplayFormat("dd.MM.yyyy");

    QRegExp regExpWordInput("[A-Z|А-Я][a-z | а-я]*");//Первый символ обзательно с заглавной буквы дальше с маленькой
    QRegExpValidator *validatorWordInput = new QRegExpValidator(regExpWordInput,this);

    ui->nameLine->setValidator(validatorWordInput);
    ui->secondNameLine->setValidator(validatorWordInput);
    ui->surnameLine->setValidator(validatorWordInput);

    QSqlQuery query;
    {
        QString strF = "SELECT number, name, surname FROM phone_book WHERE id = %1";
        QString str = strF.arg(current_id);
        //qDebug() << str;
        query.exec(str);
        QString name, surname, number;
        while (query.next()) {
            number = query.value(0).toString();
            ui->numberLine->setText(number);
            started_number = number;
            name = query.value(1).toString();
            //qDebug() << name;
            ui->nameLine->setText(name);
            surname = query.value(2).toString();
            ui->surnameLine->setText(surname);
        }
        strF = "SELECT secondName, address, gender, date FROM phone_book_info WHERE id = %1";
        str = strF.arg(current_id);
//        qDebug() << str;
        query.exec(str);
        QString secondName, address, gender, date;
        while (query.next()) {
            secondName = query.value(0).toString();
            ui->secondNameLine->setText(secondName);
            address = query.value(1).toString();
            ui->addressLine->setText(address);
            gender = query.value(2).toString();
            if (gender == "Male")
                ui->maleRadioButton->setChecked(true);
            else if (gender == "Female")
                ui->femaleRradioButton->setChecked(true);
            date = query.value(3).toString();
//            qDebug() << date;
            QDate d = QDate::fromString(date,"dd.MM.yyyy");
//            qDebug() << d;
            ui->dateEdit->setDate(d);
        }
    }



    connect(ui->submitButton,SIGNAL(clicked(bool)),this,SLOT(is_submitButton_clicked()));

    // костыльне конекты как и в основном окне
    connect(ui->numberLine,SIGNAL(textChanged(QString)),this,SLOT(change_numberLine_background()));
    connect(ui->nameLine,SIGNAL(textChanged(QString)),this,SLOT(change_nameLine_background()));
    connect(ui->secondNameLine,SIGNAL(textChanged(QString)),this,SLOT(change_secondNameLine_background()));
    connect(ui->surnameLine,SIGNAL(textChanged(QString)),this,SLOT(change_surnameLine_background()));
    connect(ui->addressLine,SIGNAL(textChanged(QString)),this,SLOT(change_addressLine_background()));

}

changeInfoButton_window::~changeInfoButton_window()
{
    delete ui;
}

void changeInfoButton_window::is_submitButton_clicked()
{
    addValidFlag = true;
    QSqlQuery query;
    person_information record;
    // bool addValidFlag = true

    // ВОЗМОЖНО СТОИТ ЗАМЕНИТЬ ВСЕ НА 1 ЭКСЕПШЕН ТИПА Fill lines!
    try {
        if (ui->numberLine->text().length() < 15) {
            addValidFlag = false;
            ui->numberLine->setStyleSheet("QLineEdit{background:red;}");
            throw (std::invalid_argument("Enter full Phone Number"));
        } else {
            QString exmpl;
            bool no_repeats_flag = true;
            query.exec("SELECT Number FROM phone_book");
            while (query.next() && no_repeats_flag) {
                 QSqlRecord rec = query.record();
                 exmpl = query.value(rec.indexOf("Number")).toString();
//                 qDebug() << exmpl << ui->numberLine->text();
                 if (ui->numberLine->text() == exmpl && started_number != ui->numberLine->text()) {
                     no_repeats_flag = false;
                     addValidFlag = false;
                     throw (std::invalid_argument("A man with such a number is already added to the phone book!"));
                 }
            }
        }

        if (ui->nameLine->text().length() < 2) {
            addValidFlag = false;
            ui->nameLine->setStyleSheet("QLineEdit{background:red;}");
            if (ui->nameLine->text().length() == 1) {
                throw (std::invalid_argument("One letter name, Srsly?"));
            } else {
                throw (std::invalid_argument("Enter Name"));
            }
        }

        //т.к. отчества может не быть проеряется только то что оно не из одной буквы
        if (ui->secondNameLine->text().length() == 1 && ui->secondNameLine->text() != "-") {
            addValidFlag = false;
            ui->secondNameLine->setStyleSheet("QLineEdit{background:red;}");
            throw (std::invalid_argument("One letter Second Name? Srsly?"));
        }

        if (ui->surnameLine->text().length() < 2) {
            addValidFlag = false;
            ui->surnameLine->setStyleSheet("QLineEdit{background:red;}");
            if (ui->surnameLine->text().length() == 1) {
                throw (std::invalid_argument("One letter surname, Srsly?"));
            } else {
                throw (std::invalid_argument("Enter Surname"));
            }
        }

        if (ui->addressLine->text().length() == 0) {
            addValidFlag = false;
            ui->addressLine->setStyleSheet("QLineEdit{background:red;}");
            throw (std::invalid_argument("Enter Address"));
        }

        {   int cur_year, cur_month, cur_day;
            QDate::currentDate().getDate(&cur_year,&cur_month,&cur_day);

            int read_year, read_month, read_day;
            ui->dateEdit->date().getDate(&read_year, &read_month, &read_day);

            QString exept = "Choose normal date";
            if (read_year > cur_year) {
                addValidFlag = false;
                throw (std::invalid_argument(exept.toStdString()));
            } else if (cur_year == read_year && read_month > cur_month) {
                addValidFlag = false;
                throw (std::invalid_argument(exept.toStdString()));
            } else if (cur_year == read_year && read_month == cur_month && read_day > cur_day) {
                addValidFlag = false;
                throw (std::invalid_argument(exept.toStdString()));
            }
        }

    } catch(std::exception& e) {
        QMessageBox::critical(this,"Error",e.what());
       // qDebug() << e.what();
    }

    if (addValidFlag) {
        record.number = ui->numberLine->text();
        record.name = ui->nameLine->text();
        if (ui->secondNameLine->text().length() == 0) {
            ui->secondNameLine->setText("-");
            record.second_name = "-";
        } else
            record.second_name = ui->secondNameLine->text();
        record.surname = ui->surnameLine->text();
        record.address = ui->addressLine->text();
        if (ui->maleRadioButton->isChecked())
            record.gender = "Male";
        else
            record.gender = "Female";
        record.date = ui->dateEdit->text();

        QString strF = "UPDATE phone_book SET number = '%1', name = '%2', surname = '%3' "
                      "WHERE id = %4;";
        QString str = strF.arg(record.number)
                          .arg(record.name)
                          .arg(record.surname)
                          .arg(current_id);
        //qDebug() << str;

        if (!query.exec(str)) {
            qDebug() << "can't update book";
        }
        strF = "UPDATE phone_book_info SET secondName = '%1', Address = '%2', Gender = '%3', Date = '%4' "
               "WHERE id = %5;";
        str = strF.arg(record.second_name)
                  .arg(record.address)
                  .arg(record.gender)
                  .arg(record.date)
                  .arg(current_id);
      //  qDebug() << str;

        if (!query.exec(str)) {
            qDebug() << "can't update book's info";

        }
        if (!query.exec("SELECT * FROM phone_book;")) {
            qDebug() << "can't select";
        }
        queryModel.setQuery(query);
        tableView->setModel(&queryModel);
        close();
    }

}

void changeInfoButton_window::closeEvent(QCloseEvent* )
{
    ui->numberLine->clear();
    ui->nameLine->clear();
    ui->secondNameLine->clear();
    ui->surnameLine->clear();
    ui->addressLine->clear();
    ui->dateEdit->clear();

    changeInfoButton_window::close();
}


void changeInfoButton_window::change_numberLine_background()
{
    ui->numberLine->setStyleSheet("QlineEdit{Background:white;}");
}

void changeInfoButton_window::change_nameLine_background()
{
    ui->nameLine->setStyleSheet("QlineEdit{Background:white;}");
}

void changeInfoButton_window::change_secondNameLine_background()
{
    ui->secondNameLine->setStyleSheet("QlineEdit{Background:white;}");
}

void changeInfoButton_window::change_surnameLine_background()
{
    ui->surnameLine->setStyleSheet("QlineEdit{Background:white;}");
}

void changeInfoButton_window::change_addressLine_background()
{
    ui->addressLine->setStyleSheet("QLineEdit{Background:white;}");
}
