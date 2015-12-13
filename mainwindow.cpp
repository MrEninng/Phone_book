#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRegExpValidator> //  Регулярные выражения использовались для поля numberLine/ заменено на inputmask
                            // Сейчас используется для валидатора навзаний
#include <QRegExp>
#include <stdexcept>
#include <QDate>  //  Для валидатора даты
//#include <QDialog>
//#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{

    ui->setupUi(this);

    ui->changeInfoButton->setToolTip(tr("Click on any row and press that button"));

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

    //ИЗМЕНЕНИЕ ПЛЕЙСХОЛДЕРА В parametrLine
    connect(ui->nameRadioButton,SIGNAL(clicked(bool)),this,SLOT(is_findBox_clicked()));
    connect(ui->surnameRadioButton,SIGNAL(clicked(bool)),this,SLOT(is_findBox_clicked()));
    connect(ui->dateRadioButton,SIGNAL(clicked(bool)),this,SLOT(is_findBox_clicked()));

    //Блок Меню
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(is_actionExit_triggered()));
    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(is_actionAbout_triggered()));
    connect(ui->actionAboutQt,SIGNAL(triggered(bool)),this,SLOT(is_actionAboutQt_triggered()));
    connect(ui->actionSaveAsTXT,SIGNAL(triggered(bool)),this,SLOT(is_actionSaveAsTXT_triggered()));

    connect(ui->actionOpenTXT,SIGNAL(triggered(bool)),this,SLOT(is_actionOpenAsTXT_triggered()));

    connect(ui->actionGender_diagram, SIGNAL(triggered(bool)), this, SLOT(is_actionGenderDiagram_triggered()));



    {
    // БАЗА ДАННЫХ!

    //соеденяемся с менеджером баз данных
        if (!(create_connection())) {
            qDebug() << "no connection";
        }
        // создаем базу

       QSqlQuery query;
//       query.exec("DROP TABLE phone_book");
//       query.exec("DROP TABLE phone_book_info");

       // ИСПОЛЬЗУЕТСЯ две таблицы в базе данных, т.к. это ебаный костыль т.к. я не смог
       // разобраться с тем, как нормально выводить информацию из одной таблицы

       QString str = "CREATE TABLE phone_book ( "
                    "id INTEGER,"
                    "Number VARCHAR(20), "
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


    //Кнопки


    connect(ui->addButton,SIGNAL(clicked(bool)),this,SLOT(is_addButton_clicked()));
    connect(ui->deleteButton,SIGNAL(clicked(bool)),this,SLOT(is_deleteButton_clicked()));
    connect(ui->changeInfoButton,SIGNAL(clicked(bool)),this,SLOT(is_changeInfoButton_clicked()));
    connect(ui->infoButton,SIGNAL(clicked(bool)),this,SLOT(is_infoButton_clicked()));
    connect(ui->findButton,SIGNAL(clicked(bool)),this,SLOT(is_findButton_clicked()));

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
    QSqlQuery query;
    query.exec("DROP TABLE phone_book");
    query.exec("DROP TABLE phone_book_info");
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

void MainWindow::is_actionSaveAsTXT_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Phone book as TXT"), "", tr("*.txt"));
    QFile saveTxtFile(fileName);
    if (!saveTxtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::information(this, tr("Unable to open file"),
//                                 saveTxtFile.errorString());
        qDebug() << "can't save file";
    } else {
        QTextStream out(&saveTxtFile);
        // ВОЗМОЖНО СТОИТ ЗАМЕНИТЬ ПОИСК ВО ВТОРОЙ ТАБЛИЦЕ НО ПРИ СОРТИРОВКЕ ТАК БУДЕТ ЛУЧШЕ!
        QSqlQuery query, queryInfo;
        person_information record;
        int size = queryModel.rowCount();
       // qDebug() << "SIZE:" << size;
        out << "Number | Name | SecondName | Surname | Gender | Date | Address\0" << endl;
        QString str = "SELECT id, number, name, surname FROM phone_book";
        QString strF = "SELECT secondName, Address, Gender, Date FROM phone_book_info WHERE id = %1";
        query.exec(str);
        for (int i = 0; i < size; i++) {
            query.next();
            int id = query.value(0).toInt();
            record.number = query.value(1).toString();
            record.name = query.value(2).toString();
            record.surname = query.value(3).toString();

            QString strI = strF.arg(id);
            queryInfo.exec(strI);
            while (queryInfo.next()) {
                record.second_name = queryInfo.value(0).toString();
                record.address = queryInfo.value(1).toString();
                record.gender = queryInfo.value(2).toString();
                record.date = queryInfo.value(3).toString();
            }
            // qDebug() << id << record.number << record.name << record.second_name << record.surname << record.address << record.gender << record.date;

            // КОнечную строку лучше подгонять под размер!
            QString stringForAdd = record.number + " | " + record.name +
                " | " + record.second_name + " | " + record.surname + " | " + record.gender
                + " | " + record.date + " | " + record.address + "\0";
            out << stringForAdd << endl;
           // qDebug() << stringForAdd;

        }
    }// Конец else
}



void MainWindow::is_actionOpenAsTXT_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Phone book as TXT"), "", tr("*.txt"));
    QFile openAsTXTFile(fileName);
    if (!openAsTXTFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
       // QMessageBox::information(this, tr("Unable to open file"),
         //                        op   enAsTXTFile.errorString());
        qDebug() << "can't open file";
    } else {
        QSqlQuery query;
        QString line  = openAsTXTFile.readLine(); // Считывается и пропускается первая строка содержащая шапку файла.
        QStringList readedPartsOfString;
        QString str, strF;

        // ОЧИСТКА БАЗЫ ДАННЫХ И ТАБЛИЦЫ
        str = "DELETE FROM phone_book;";
        query.exec(str);
        str = "DELETE FROM phone_book_info;";
        query.exec(str);
        queryModel.setQuery(query);
        id = 0;

        queryModel.insertColumns(0,4);
        queryModel.setHeaderData(0,Qt::Horizontal,QObject::tr("id"));
        queryModel.setHeaderData(1,Qt::Horizontal,QObject::tr("Number"));
        queryModel.setHeaderData(2,Qt::Horizontal,QObject::tr("Name"));
        queryModel.setHeaderData(3,Qt::Horizontal,QObject::tr("Surname"));
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->setModel(&queryModel);

        while (!openAsTXTFile.atEnd()) {
            line = openAsTXTFile.readLine();
            readedPartsOfString = line.split('|');
            // ЦИКЛ ДЛЯ УДАЛЕНИЯ ЛИШНИХ ПРОБЕЛОВ
            for (int i = 0; i < 7; ++i) {
                readedPartsOfString[i].replace(" ","");
//                qDebug() << readedPartsOfString[i];
            }
            readedPartsOfString[6].replace('\n',""); //  Удалениe \n


            strF = "INSERT INTO phone_book_info (id, SecondName, Address, Gender, Date)"
                   "VALUES (%1,'%2','%3','%4','%5')";
            str = strF.arg(id)
                      .arg(readedPartsOfString[2])
                      .arg(readedPartsOfString[6])
                      .arg(readedPartsOfString[4])
                      .arg(readedPartsOfString[5]);
            if (!query.exec(str)) {
                qDebug() << "Unable to make insert Operation into IFNO";
            }

            strF = "INSERT INTO phone_book (id, number, name, surname)"
                    "VALUES (%1, '%2', '%3', '%4')";
            str = strF.arg(id)
                      .arg(readedPartsOfString[0])
                      .arg(readedPartsOfString[1])
                      .arg(readedPartsOfString[3]);
            if (!query.exec(str)) {
                qDebug() << "Unable to make insert Operation into Main";
            }
            ++id;
        }
        str = "SELECT * FROM phone_book;";
        query.exec(str);
        queryModel.setQuery(query);
    }
}




//Круговая диаграмма (Genger_diagram)
void MainWindow::is_actionGenderDiagram_triggered()
{
    try {
        QSqlQuery query;
        int men = 0;
        int women = 0;
        // поиск из базы данных мужчин и подсчет их количества
        query.exec("SELECT id FROM phone_book_info WHERE gender = 'Male'");
        while(query.next())
            men++;
        women = queryModel.rowCount() - men;
        //qDebug() << men << " " << women;
        if (men == 0 and women == 0)
            throw (std::invalid_argument("Fill in the phone book, please."));
        else {
            gender_diagram_widget = new gender_diagram(this);
            gender_diagram_widget->set_data(men, women);
            gender_diagram_widget->show();
        }
    }
    catch (std::exception& e) {
        QMessageBox::critical(this,"Error",e.what());
    }
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
        ui->parametrLine->setPlaceholderText("id");

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
           // qDebug() << cur_year << cur_month << cur_day;
            int read_year, read_month, read_day;
            ui->dateEdit->date().getDate(&read_year, &read_month, &read_day);
            //  `qDebug() << read_year << read_month << read_day;
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

// ДОБАВЛЕНИЕ В ТАБЛИЦУ
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

//        QString strF =
//                "INSERT INTO phone_book (number, name, secondName, surname, address, gender, date) "
//                "VALUES (%1, '%2', '%3', '%4', '%5', '%6', '%7')";

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
                  .arg(record.address)
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

        id++;


        queryModel.setQuery(query);
        ui->tableView->setModel(&queryModel);


        // ПРОВЕРЕНО ЧТО ДАННЫЕ В ТАБЛИЦУ УСПЕШНО ЗАНОСЯТСЯ И ОТ ТУДА БЕРУТСЯ!
        //        QSqlRecord rec = query.record();
        //        QString exmpl;
        //        while (query.next()) {

        //            exmpl = query.value(rec.indexOf("name")).toString();
        //            qDebug() << exmpl;
        //        }
        //        qDebug() << "";
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
    query.exec("SELECT * FROM phone_book;");
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
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    try {
        if (indexes.count() == 0 || indexes.count() > 1)
            throw (std::invalid_argument("Choose just 1 row!"));
        else {
            QString f = indexes[0].data().toString();
            change_widget = new changeInfoButton_window(f.toInt(),ui->tableView);
            change_widget->setModal(true);
            change_widget->show();
//            query.exec("SELECT * FROM phone_book");
//            queryModel.setQuery(query);
        }
    } catch (std::exception &e) {
        QMessageBox::critical(this,"Error!",e.what());
    }
}

void MainWindow::is_infoButton_clicked()
{
    QSqlQuery query;
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    try {
        if (indexes.count() == 0 || indexes.count() > 1)
            throw (std::invalid_argument("Choose just 1 row!"));
        else {
            int current_id = indexes[0].data().toString().toInt();
            QString strF = "SELECT number, name, surname FROM phone_book WHERE id = %1";
            QString str = strF.arg(current_id);
            //qDebug() << str;
            query.exec(str);
            QString name, surname, number;
            while (query.next()) {
                number = query.value(0).toString();
                name = query.value(1).toString();
                surname = query.value(2).toString();
            }

            strF = "SELECT secondName, address, gender, date FROM phone_book_info WHERE id = %1";
            str = strF.arg(current_id);
            query.exec(str);
            QString secondName, address, gender, date;
            while (query.next()) {
                secondName = query.value(0).toString();
                address = query.value(1).toString();
                gender = query.value(2).toString();
                date = query.value(3).toString();
            }
            QString information = "  id =" + QString::number(current_id) +
                                  "\n  Name: " + name + "\n  Second Name: " + secondName +
                                  "\n  Surname: " + surname + "\n  Number: " + number +
                                  "\n  Address: " + address + "\n  Gender: " + gender +
                    "\n  Date of Birth: " + date;
            QMessageBox::information(this,"Personal information", information);

        }
    } catch (std::exception &e) {
        QMessageBox::critical(this, "Error!", e.what());
    }
}

void MainWindow::is_findButton_clicked()
{
    QString value = ui->parametrLine->text();
    QSqlQuery query;
//  НИЖЕ ПРИМЕР ДЛЯ ПОИСКА  ФАМИЛИИ ПО ИМЕНИ!
    QString strF = "SELECT Surname FROM phone_book WHERE name LIKE '%%1%'";
    QString str = strF.arg(value);
//    qDebug() << str;
    query.exec(str);
//    QTableView *table = new QTableView;
//    QSqlQueryModel model;
//    model.setQuery(query);
//    table->setModel(&model);
//    table->show();
//    while (query.next()) {
//        qDebug() << query.value(0).toString();
//    }

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


bool MainWindow::create_connection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("phone_book.db");
    db.setUserName("User");
    db.setHostName("user's_computer");
    db.setPassword("Password");
    if (!db.open()) {
        qDebug() << "cannot open database " << db.lastError().text();
        return false;
    } else {
       // qDebug() << "connected";
    }

    return true;
}

void MainWindow::closeEvent(QCloseEvent*)
{
    QSqlQuery query;
    query.exec("DROP TABLE phone_book");
    query.exec("DROP TABLE phone_book_info");
    close();
}


//// TODO Подумать на структурой флага(да это зашквар) | составной флаг для коннекта смены цвета   -FORGET
// TODO Попробовать переделать рекурентки имени отчества и фамилии на поднятие в заглавную букву
// TODO Разобраться с QMouseEvent для проверки изменения цвета поля!  Пока на костыле висит
//// TODO Разобраться как чистануть базы данных!  - SOLVED!
//// TODO Разобраться как перенести базу данных в функцию и создать указатель на нее. - SOLVED
//// TODO Подумать над добавлением кнопки Change Information -  SOLVED!
//// TODO Oбдумать костыль для поля Даты при добавление в базу данных SOLVED (Deleted)
//// TODO Подумать над добавлением строк через QAction SOLVED / ЗАменено на костыль|не костыль с двумя базами
// TODO окно для поиска ( добавить в главный хэдэр хээдэр окна поиска)!
// TODO обрабоать замену айдишников после удаления!// возможен вектор с айдишниками, в который будут записаны оставшиеся айдишники, но это убийство памяти
// TODO сортировка с помощью проксимодели http://www.qtforum.org/article/18679/qsqlquerymodel-qtableview-and-sorting.html
// TODO Доделать what's this в выскакивающих окнах и главном окне
//// TODO Сделать Валидатор для даты SOLVED
//// TODO Сделать чтобы при нажатии change info  в полях выплевшего окна были данные SOLVED
// TODO Открылся вопрос о полноценности окна появляющегося после нажатия кнопки Find,  Должна ли быть там возможность удалить или редактировать данные?
// TODO СДЕЛАТЬ В ПЕРВУЮ ОЧЕРЕДЬ СОРТИРОВКУ!
// TODO Доделать формат для сохранения .TXT( определенное количество символов между разделителями
//// TODO Убрать QMessagebox-ы из сохранения и открытия файла, т.к. эт противоречит религии приложений - SOLVED
// TODO Окно спрашиващее "уверены ли вы что хотите удалить данные"
// TODO Документацию
//// TODO Убрать пробелы при считывании и разобарться почему 2 строки. SOLVED
// TODO Добавить поток для считывания данных и удаления большого количества строк, иначе прога зависает

// CHANGES AFTER FIRST COMMIT:
/* Working buttons: Change info, Personal info!
 * Added changes for closeEvent (clearing database);
 * Working Save AS TXT menuButton;
 */

/*Changes after second and third commit:
 * Working database on linux;
 * Working dayaedit on linux;
 */

/* Changes after 4th commit:
 * added OpenAsTxt button
 */
