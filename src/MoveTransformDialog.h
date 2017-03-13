#ifndef MOVETRANSFORMDIALOG_H
#define MOVETRANSFORMDIALOG_H

#include <QDialog>

namespace Ui {
class MoveTransformDialog;
}

class MoveTransformDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::MoveTransformDialog *ui;

public:
    explicit MoveTransformDialog(QWidget *parent = 0);
    ~MoveTransformDialog();

public slots:
    void Accept();

};

#endif // MOVETRANSFORMDIALOG_H
