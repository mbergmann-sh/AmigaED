#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
    // Remove Close-, help-, size gadgets
    this->setWindowFlags(Qt::Dialog | Qt::Desktop | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

aboutDialog::~aboutDialog()
{
    delete ui;
}
