#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "iteminfo.h"
#include "ui_iteminfo.h"
#include "global.h"
#include "mainwindow0.h"

CItemInfo::CItemInfo(const QVariant& Id,QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CItemInfo)
{
  ui->setupUi(this);

  m_pModelItem=new QSqlTableModel(this);
  m_pModelItem->setTable("sbitem");
  m_pModelItem->setFilter("idx="+Id.toString());
  m_pModelItem->select();

  m_pModelGroup=new QSqlTableModel(this);
  m_pModelGroup->setTable("sbgroup");
  m_pModelGroup->setFilter("idx="+m_pModelItem->record(0).value(1).toString());
}

CItemInfo::~CItemInfo()
{
  delete ui;
}

void CItemInfo::showEvent( QShowEvent */*pEvent*/ )
{
  m_pModelGroup->select();
  m_pModelItem->select();

  //Fill out Field Names
  ui->pLField1->setText("<b>"+m_pModelGroup->record(0).value(2).toString()+"</b>");
  ui->pLField2->setText("<b>"+m_pModelGroup->record(0).value(3).toString()+"</b>");
  ui->pLField3->setText("<b>"+m_pModelGroup->record(0).value(4).toString()+"</b>");
  ui->pLField4->setText("<b>"+m_pModelGroup->record(0).value(5).toString()+"</b>");
  ui->pLField5->setText("<b>"+m_pModelGroup->record(0).value(6).toString()+"</b>");
  ui->pLField6->setText("<b>"+m_pModelGroup->record(0).value(7).toString()+"</b>");
  ui->pLField7->setText("<b>"+m_pModelGroup->record(0).value(8).toString()+"</b>");
  ui->pLField8->setText("<b>"+m_pModelGroup->record(0).value(9).toString()+"</b>");
  ui->pLGroup->setText("<b>"+m_pModelGroup->record(0).value(1).toString()+"</b>");

  //Fill out Field Data
  ui->pLF1->setText(m_pModelItem->record(0).value(2).toString());
  ui->pLF2->setText(m_pModelItem->record(0).value(3).toString());
  ui->pLF3->setText(m_pModelItem->record(0).value(4).toString());
  ui->pLF4->setText(m_pModelItem->record(0).value(5).toString());
  ui->pLF5->setText(m_pModelItem->record(0).value(6).toString());
  ui->pLF6->setText(m_pModelItem->record(0).value(7).toString());
  ui->pLF7->setText(m_pModelItem->record(0).value(8).toString());
  ui->pLF8->setText(m_pModelItem->record(0).value(9).toString());
}

void CItemInfo::on_pPBClose_clicked()
{
  close();
}
