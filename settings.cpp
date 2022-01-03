#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "settings.h"
#include "ui_settings.h"
#include "global.h"
#include "mainwindow0.h"

CSettings::CSettings(QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CSettings)
{
  ui->setupUi(this);

  m_pModel=new QSqlTableModel(this);
  m_pModel->setTable("settings");
  m_pModel->select();

  //Initiating FontSize combobox
  ui->pCBFontSize->addItem(tr("Normal"));
  ui->pCBFontSize->addItem(tr("Medium"));
  ui->pCBFontSize->addItem(tr("Small"));
  ui->pCBFontSize->addItem(tr("XSmall"));
  ui->pCBFontSize->setCurrentIndex(m_pModel->record(0).value(19).toInt());

  //Initiate DateFormat comboBox
  ui->pCBDateFormat->addItem(tr("dd.mm"),"dd.MM");
  ui->pCBDateFormat->addItem(tr("dd.mm.yy"),"dd.MM.yy");
  ui->pCBDateFormat->addItem(tr("mm.dd"),"MM.dd");
  ui->pCBDateFormat->addItem(tr("mm.dd.yy"),"MM.dd.yy");
  int iIdx=ui->pCBDateFormat->findData(m_pModel->record(0).value(21));
  ui->pCBDateFormat->setCurrentIndex(iIdx);
}

CSettings::~CSettings()
{
  delete ui;
}

void CSettings::on_pPBApply_clicked()
{
  QSqlRecord Record=m_pModel->record(0);

  //DateFormat
  Record.setValue(19,ui->pCBFontSize->currentIndex());

  //FontSize
  QString s=ui->pCBDateFormat->itemData(ui->pCBDateFormat->currentIndex()).toString();
  Record.setValue(21,s);

  m_pModel->setRecord(0,Record);
  m_pModel->submitAll();
  close();
}

void CSettings::showEvent( QShowEvent */*pEvent*/ )
{
}

void CSettings::on_pPBCancel_clicked()
{
  close();
}

void CSettings::on_pPBReset_clicked()
{
  QSqlQuery Query;
  Query.setForwardOnly(true);

  Query.exec("UPDATE settings SET acc_list=NULL,trans_lista=NULL,trans_listp=NULL,trans_listc=NULL,sch_list=NULL WHERE idx=0;");

  g_pMainWindow->databaseChanged();
}
