#include <QtGui>
#include <QtWidgets>
#include "help0.h"
#include "ui_help0.h"

CHelp::CHelp(QWidget *pWidget) :
  CFormBase(pWidget),
  ui(new Ui::CHelp)
{
  ui->setupUi(this);
  QString s="<p align='center'><b>"+tr("Quick start")+"</b></p>" \
            "<p align='justify'>"+tr("Click on")+" <b><i>"+tr("New")+"</i></b> "+tr("from File menu to create your first database.")+"</p>"\
            "<p align='justify'>"+tr("Click on")+" <b><i>"+tr("General")+"</i></b> "+tr("from Data menu to create your list of Banks and Types to be used as basic information once you create your Accounts. Then create your list of Payees, Categories and Classes to be used as basic information once you create your Transactions.")+"</p>"\
            "<p align='justify'>"+tr("Click on")+" <b><i>"+tr("Accts")+"</i></b> "+tr("button to enter into the Account's maintainer to create your first account.")+"</p>"\
            "<p align='justify'>"+tr("To start adding your transactions, select the new account from Accounts view to open Transactions view and click on")+" <b><i>"+tr("Add")+"</i></b> "+tr("from Actions menu.")+"</p>"\
            "<p align='justify'>"+tr("Enter a date, payee, amount, category and if needed a class from the lists and click")+" <b><i>"+tr("Apply")+"</i></b> "+tr("to close the form. You'll return to the register and the transaction you entered will be displayed and your ending balance will be updated.")+"</p>"\
            "<p align='justify'></p>"\
            "<p align='justify'>"+tr("Download the user's manual for further and more detailed help from the official website:")+" <a href='http://www.darhon.com'>www.darhon.com</a></p>";

  ui->pTextEdit->setHtml(s);
}

CHelp::~CHelp()
{
  delete ui;
}

void CHelp::showEvent( QShowEvent */*pEvent*/ )
{
  ui->pTextEdit->setFocus();
}

void CHelp::on_pPBClose_clicked()
{
  close();
}


void CHelp::on_pPBScrollUp_clicked()
{
  ui->pTextEdit->verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
}

void CHelp::on_pPBScrollDown_clicked()
{
  ui->pTextEdit->verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
}
