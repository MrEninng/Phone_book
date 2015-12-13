#ifndef GENDER_DIAGRAM_H
#define GENDER_DIAGRAM_H

#include <QMainWindow>

namespace Ui {
class gender_diagram;
}

class gender_diagram : public QMainWindow
{
    Q_OBJECT

public:
    explicit gender_diagram(QWidget *parent = 0);
    void set_data(int men, int women);

    void paintEvent(QPaintEvent *);
    ~gender_diagram();

private:
    Ui::gender_diagram *ui;
    int _men;
    int _women;
};

#endif // GENDER_DIAGRAM_H
