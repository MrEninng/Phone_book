#include "changeinfobutton_window.h"
#include "ui_changeinfobutton_window.h"

changeInfoButton_window::changeInfoButton_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changeInfoButton_window)
{
    ui->setupUi(this);
    //УСТАНОВКА ИКОНКИ ПРИЛОЖЕНИЯ
    icon = new QIcon();
    icon->addFile(":/imgs/icon3.png");
    setWindowIcon(*icon);
    ui->numberLine->setInputMask("8(000)000-00-00");
    connect(ui->submitButton,SIGNAL(clicked(bool)),this,SLOT(is_submitButton_clicked()));
}

changeInfoButton_window::~changeInfoButton_window()
{
    delete ui;
}

void changeInfoButton_window::is_submitButton_clicked()
{

}
