#include "searchdialog.h"
#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::Desktop | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    this->setModal(false);
    ui->lineEdit_Search->setFocus();

}

SearchDialog::~SearchDialog()
{
    delete ui;
}
