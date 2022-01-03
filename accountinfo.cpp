#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "accountinfo.h"
#include "ui_accountinfo.h"
#include "dr_profile.h"
#include "global.h"
#include "mainwindow0.h"

CAccountInfo::CAccountInfo(const QVariant& Id,QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CAccountInfo)
{
  ui->setupUi(this);

  //Setup children
#ifdef MOBILE_PLATFORM
  ui->pWidgetMemo->setVisible(false);
#else
  ui->pPBMemo->hide();
#endif

  m_pModel=new QSqlTableModel(this);
  m_pModel->setTable("account");
  m_pModel->setFilter("idx="+Id.toString());

  m_pModelBank=new QSqlTableModel(this);
  m_pModelBank->setTable("bank");
  m_pModelType=new QSqlTableModel(this);
  m_pModelType->setTable("type");
  m_pModelCurrency=new QSqlTableModel(this);
  m_pModelCurrency->setTable("currency");
}

CAccountInfo::~CAccountInfo()
{
  delete ui;
}

void CAccountInfo::showEvent( QShowEvent */*pEvent*/ )
{
  m_pModel->select();

  //Filter referenced tables with the content of 'account'
  m_pModelType->setFilter("idx="+m_pModel->record(0).value(2).toString());
  m_pModelType->select();
  m_pModelBank->setFilter("idx="+m_pModel->record(0).value(4).toString());
  m_pModelBank->select();
  m_pModelCurrency->setFilter("idx="+m_pModel->record(0).value(5).toString());
  m_pModelCurrency->select();

  //Format initial balance
  double d=m_pModel->record(0).value(8).toDouble();
  QLocale Locale;
  if (d<0)
    ui->pLInBalance->setText("("+Locale.toString(d*-1.0,'f',2)+")");
  else
    ui->pLInBalance->setText(Locale.toString(d,'f',2));

  //Fill out controls from "account" table
  ui->pLIcon->setPixmap(QPixmap(m_pModel->record(0).value(1).toString()));
  ui->pLAccount->setText("<b>"+m_pModel->record(0).value(3).toString()+"</b>");
  ui->pLNumber->setText(m_pModel->record(0).value(6).toString());
  ui->pLHolder->setText(m_pModel->record(0).value(7).toString());
  ui->pTENoteA->setHtml(m_pModel->record(0).value(9).toString());
  ui->pLAccStatus->setText(m_pModel->record(0).value(11).toInt()==0 ? tr("Inactive") : tr("Active"));

  //Fill out controls from "bank" table
  ui->pLBank->setText(m_pModelBank->record(0).value(1).toString());
  ui->pLAddress->setText(m_pModelBank->record(0).value(2).toString());
  ui->pLOther->setText(m_pModelBank->record(0).value(3).toString());
  ui->pTENoteB->setHtml(m_pModelBank->record(0).value(4).toString());

  //Fill out controls from "type" table
  ui->pLType->setText("<i>"+m_pModelType->record(0).value(1).toString()+"</i>");

  //Fill out controls from "currency" table
  QString s=m_pModelCurrency->record(0).value(1).toString();
  s+=" ("+m_pModelCurrency->record(0).value(3).toString()+")";
  ui->pLCurrency->setText(s);
}

void CAccountInfo::on_pPBClose_clicked()
{
  close();
}

void CAccountInfo::on_pPBMemo_toggled(bool bChecked)
{
  ui->pPBClose->setVisible(!bChecked);
  ui->pWidgetControls->setVisible(!bChecked);
  ui->pWidgetMemo->setVisible(bChecked);
}
