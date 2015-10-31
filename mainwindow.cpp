#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRegExpValidator> //  Регулярные выражения использовались для поля numberLine/ заменено на inputmask
                            // Сейчас используется для валидатора навзаний
#include <QRegExp>
#include <QChar>
#include <stdexcept>
//#include "change_information_widget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{

    ui->setupUi(this);

    //УСТАНОВКА ИКОНКИ ПРИЛОЖЕНИЯ
    icon = new QIcon();
    icon->addFile(":/imgs/icon3.png");
    setWindowIcon(*icon);

    //Маска для числовой формы. Убраны регулярные выражения!
    ui->numberLine->setInputMask("8(000)000-00-00");
    //QRegExp rx("8\\(\\d{3}\\)\\d{3}\\-\\d{2}\\-\\d{2}");// УЗНАТЬ ОБ АВТО ЗАПОЛНЕНИИ дефисов
    //QValidator *numberValidator = new QRegExpValidator(rx,this);

    // В данном блоке задается максимальная длина для полей заполнения
    // И валидаторы для всех кроме Адреса! для него он не нужен(вероятно)
    ui->nameLine->setMaxLength(10);
    ui->secondNameLine->setMaxLength(10);
    ui->surnameLine->setMaxLength(10);
    ui->addressLine->setMaxLength(10);

    QRegExp regExpWordInput("[A-Z|А-Я][a-z | а-я]*");//Первый символ обзательно с заглавной буквы дальше с маленькой
    QRegExpValidator *validatorWordInput = new QRegExpValidator(regExpWordInput,this);

    ui->nameLine->setValidator(validatorWordInput);
    ui->secondNameLine->setValidator(validatorWordInput);
    ui->surnameLine->setValidator(validatorWordInput);

    //ИЗМЕНЕНИЕ ПЛЕЙСХОЛДЕРА В parametrLine
    connect(ui->nameRadioButton,SIGNAL(clicked(bool)),this,SLOT(is_findBox_clicked()));
    connect(ui->surnameRadioButton,SIGNAL(clicked(bool)),this,SLOT(is_findBox_clicked()));
    connect(ui->dateRadioButton,SIGNAL(clicked(bool)),this,SLOT(is_findBox_clicked()));

    //Блок Меню
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(is_actionExit_triggered()));
    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(is_actionAbout_triggered()));
    connect(ui->actionAboutQt,SIGNAL(triggered(bool)),this,SLOT(is_actionAboutQt_triggered()));

    {
    // БАЗА ДАННЫХ!

    //соеденяемся с менеджером баз данных
        if (!(createConnection())) {
            qDebug() << "no connection";
        }
        // создаем базу

       QSqlQuery query;
       query.exec("DROP TABLE phone_book");
       query.exec("DROP TABLE phone_book_info");

       // ИСПОЛЬЗУЕТСЯ две таблицы в базе данных, т.к. это ебаный костыль т.к. я не смог
       // разобраться с тем, как нормально выводить информацию из одной таблицы


       QString str = "CREATE TABLE phone_book ( "
                    "id INTEGER,"
                    "Number VARCHAR(20), " // пределать на строки::?
                    "Name VARCHAR(15), "
                    "Surname VARCHAR(15) "        // ТУТ УБРАНА ЗАПЯТАЯ!
                    ");";
        if (!(query.exec(str))) {
            qDebug() << "Unable to create main table" << query.lastError().text();
        }

        str = "CREATE TABLE phone_book_info ( "
                     "id INTEGER,"
                     "SecondName VARCHAR(15), "
                     "Address VARCHAR(30), "
                     "Gender VARCHAR(10), "
                     "Date VARCHAR(20) "   // можно сделать меньше но с запасом!
                     ");";
        if (!(query.exec(str))) {
            qDebug() << "Unable to create informational table" << query.lastError().text();
        }
        // изначальная отрисовка
        queryModel.insertColumns(0,4);
        queryModel.setHeaderData(0,Qt::Horizontal,QObject::tr("id"));
        queryModel.setHeaderData(1,Qt::Horizontal,QObject::tr("Number"));
        queryModel.setHeaderData(2,Qt::Horizontal,QObject::tr("Name"));
        queryModel.setHeaderData(3,Qt::Horizontal,QObject::tr("Surname"));
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->setModel(&queryModel);
    }


    // добавляем данные в таблцу!!


    //Кнопки


    connect(ui->addButton,SIGNAL(clicked(bool)),this,SLOT(is_addButton_clicked()));
    connect(ui->deleteButton,SIGNAL(clicked(bool)),this,SLOT(is_deleteButton_clicked()));
    connect(ui->changeInfoButton,SIGNAL(clicked(bool)),this,SLOT(is_changeInfoButton_clicked()));


    // коннекты смена цвета! или заменить на наследование или переделать на составной флаг, или забить
    connect(ui->numberLine,SIGNAL(textChanged(QString)),this,SLOT(change_numberLine_background()));
    connect(ui->nameLine,SIGNAL(textChanged(QString)),this,SLOT(change_nameLine_background()));
    connect(ui->secondNameLine,SIGNAL(textChanged(QString)),this,SLOT(change_secondNameLine_background()));
    connect(ui->surnameLine,SIGNAL(textChanged(QString)),this,SLOT(change_surnameLine_background()));
    connect(ui->addressLine,SIGNAL(textChanged(QString)),this,SLOT(change_addressLine_background()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 *  БЛОК МЕНЮ
 */

void MainWindow::is_actionExit_triggered()
{
    MainWindow::close();
}

void MainWindow::is_actionAbout_triggered()
{

    QMessageBox::information(this,"About","Our Info\nKoKOKO\nKEKEKEK");
}

void MainWindow::is_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

/*
 * КОНЕЦ БЛОКА МЕНЮ
 */


/*
 *  БЛОК СИГНАЛОВ ОКНА
 */

void MainWindow::is_findBox_clicked()
{
    if (ui->nameRadioButton->isChecked())
        ui->parametrLine->setPlaceholderText("Name");
    else if (ui->surnameRadioButton->isChecked())
        ui->parametrLine->setPlaceholderText("Surname");
    else if (ui->dateRadioButton->isChecked())
        ui->parametrLine->setPlaceholderText("Date");

}

/*
 *  КОНЕЦ БЛОКА СИГНАЛОВ ОКНА
 */

/*
 * БЛОК СИГНАЛОВ КНОПОК
 */

void MainWindow::is_addButton_clicked()
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
                 if (ui->numberLine->text() == exmpl) {
                     no_repeats_flag = false;
                     addValidFlag = false;
                     throw (std::invalid_argument("A man with such a number is already added to the phone book!"));
                 }
            }


            // ПРОВЕРКА НА НАЛИЧИЕ В БАЗЕ ДАННЫХ!
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
        record.adress = ui->addressLine->text();
        if (ui->maleRadioButton->isChecked())
            record.gender = "Male";
        else
            record.gender = "Female";
        record.date = ui->dateEdit->text();

//        QString strF =
//                "INSERT INTO phone_book (number, name, secondName, surname, address, gender, date) "
//                "VALUES (%1, '%2', '%3', '%4', '%5', '%6', '%7' )";

//        QString str = strF.arg(record.number)
//                          .arg(record.name)
//                          .arg(record.second_name)
//                          .arg(record.surname)
//                          .arg(record.adress)
//                          .arg(record.gender)
//                          .arg(record.date);
        QString str,strF;
        strF = "INSERT INTO phone_book_info (id, SecondName, Address, Gender, Date)"
               "VALUES (%1,'%2','%3','%4','%5')";
        str = strF.arg(id)
                  .arg(record.second_name)
                  .arg(record.adress)
                  .arg(record.gender)
                  .arg(record.date);
        //qDebug() << str;
        if (!query.exec(str)) {
            qDebug() << "Unable to make insert Operation into IFNO";
        }
        if (!query.exec("SELECT * FROM phone_book_info;")) {
            qDebug() << "Unable to  execute INFO query - exiting;";
        }
        strF = "INSERT INTO phone_book (id, number, name, surname)"
                "VALUES (%1, '%2', '%3', '%4')";
        str = strF.arg(id)
                          .arg(record.number)
                          .arg(record.name)
                          .arg(record.surname);
      //  qDebug() << str;
        if (!query.exec(str)) {
            qDebug() << "Unable to make insert Operation";
        }
        if (!query.exec("SELECT * FROM phone_book;")) {
            qDebug() << "Unable to  execute query - exiting;";
        }
        //queryModel.sort(); подумать над сортом!
        id++;


        queryModel.setQuery(query);
        //queryModel.insertRow(15);
        ui->tableView->setModel(&queryModel);



//        model.setQuery("SELECT name, surname, number FROM phone_book WHERE name = 'Sasha';");
//        if (model.lastError().isValid()) {
//            qDebug() << model.lastError();
//        }
//        ui->tableView->setModel(&model);
//        ui->tableView->show();



        // ПРОВЕРЕНО ЧТО ДАННЫЕ В ТАБЛИЦУ УСПЕШНО ЗАНОСЯТСЯ И ОТ ТУДА БЕРУТСЯ!
        //        QSqlRecord rec = query.record();
        //        QString exmpl;
        //        while (query.next()) {

        //            exmpl = query.value(rec.indexOf("name")).toString();
        //            qDebug() << exmpl;
        //        }
        //        qDebug() << "";


        // qDebug() << str;
    }
}


void MainWindow::is_deleteButton_clicked()
{

    QSqlQuery query;
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    for (int i = 0; i < indexes.count(); i++) {
        QString f = indexes[i].data().toString();
        QString str = "DELETE FROM phone_book WHERE id = " + f;
        query.exec(str);
        str = "DELETE FROM phone_book_info WHERE id = " + f;
        query.exec(str);
    }
    query.exec("SELECT * FROM phone_book");
    queryModel.setQuery(query);


    //РАБОЧИЙ КОД! для удаления 1й строки

//    QString cur_id;
//    row = ui->tableView->currentIndex().row();
//   // qDebug() << ui->tableView->currentIndex().row() << row;
//    cur_id = ui->tableView->model()->data(ui->tableView->model()->index(row/*ui->tableView->currentIndex().row()*/,0)).toString();
//   // qDebug() << cur_id;
//    QString str = "DELETE FROM phone_book WHERE id = " + cur_id;
//    QSqlQuery query;
//    query.exec(str);
//    QString str = "DELETE FROM phone_book_info WHERE id = " + cur_id;
//    query.exec(str);
//    query.exec("SELECT * FROM phone_book");
//    queryModel.setQuery(query);
}

void MainWindow::is_changeInfoButton_clicked()
{
    change_widget.setModal(true);
    change_widget.show();

}



/*
 * КОНЕЦ БЛОКА СИГНАЛОВ КНОПОК
 */

void MainWindow::change_numberLine_background()
{
    ui->numberLine->setStyleSheet("QlineEdit{Background:white;}");
}

void MainWindow::change_nameLine_background()
{
    ui->nameLine->setStyleSheet("QlineEdit{Background:white;}");
}

void MainWindow::change_secondNameLine_background()
{
    ui->secondNameLine->setStyleSheet("QlineEdit{Background:white;}");
}

void MainWindow::change_surnameLine_background()
{
    ui->surnameLine->setStyleSheet("QlineEdit{Background:white;}");
}
void MainWindow::change_addressLine_background()
{
    ui->addressLine->setStyleSheet("QLineEdit{Background:white;}");
}

//-----

bool MainWindow::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("phone_book");
    db.setUserName("User");
    db.setHostName("user's_computer");
    db.setPassword("Password");

    if (!db.open()) {
        qDebug() << "cannot open database " << db.lastError().text();
        return false;
    } else {
        //qDebug() << "connected";
    }

    return true;
}


// TODO Подумать на структурой флага(да это зашквар) | составной флаг для коннекта смены цвета
// TODO Попробовать переделать рекурентки имени отчества и фамилии на поднятие в заглавную букву
// TODO Разобраться с QMouseEvent для проверки изменения цвета поля!  Пока на костыле висит
// TODO Разобраться как чистануть базы данных!  - SOLVED!
// TODO Разобраться как перенести базу данных в функцию и создать указатель на нее. - SOLVED
// TODO Подумать над добавлением кнопки Change Information
// TODO Oбдумать костыль для поля Даты при добавление в базу данных SOLVED (Deleted)
// TODO Подумать над добавлением строк через QAction SOLVED / ЗАменено на костыль|не костыль с двумя базами
// TODO окно для поиска ( добавить в главный хэдэр хээдэр окна поиска)!
// TODO обрабоать замену айдишников после удаления!
// TODO сортировка с помощью проксимодели http://www.qtforum.org/article/18679/qsqlquerymodel-qtableview-and-sorting.html
// TODO Доделать what's this в выскакивающих окнах и главном окне
