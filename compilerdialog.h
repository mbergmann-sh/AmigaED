#ifndef COMPILERDIALOG_H
#define COMPILERDIALOG_H

#include <QDialog>

namespace Ui {
class CompilerDialog;
}

class QAction;
class QActionGroup;
class QMenu;

class CompilerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompilerDialog(QWidget *parent = 0);
    ~CompilerDialog();

private slots:
    void on_comboBoxCompilerSelection_currentIndexChanged(int index);

private:
    Ui::CompilerDialog *ui;
};

#endif // COMPILERDIALOG_H
