#include "compilerdialog.h"
#include "ui_compilerdialog.h"

CompilerDialog::CompilerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompilerDialog)
{
    ui->setupUi(this);
}

CompilerDialog::~CompilerDialog()
{
    delete ui;
}

void CompilerDialog::on_comboBoxCompilerSelection_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
    }
}
