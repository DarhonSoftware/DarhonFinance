#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "itemedit.h"
#include "ui_itemedit.h"
#include "global.h"
#include "mainwindow0.h"

CItemEdit::CItemEdit(Action iAction, const QVariant& Id, QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CItemEdit)
{
  ui->setupUi(this);

  //Setup children
  ui->pLEField1->setInputMethodHints(Qt::ImhNoAutoUppercase);
  ui->pLEField2->setInputMethodHints(Qt::ImhNoAutoUppercase);
  ui->pLEField3->setInputMethodHints(Qt::ImhNoAutoUppercase);
  ui->pLEField4->setInputMethodHints(Qt::ImhNoAutoUppercase);
  ui->pLEField5->setInputMethodHints(Qt::ImhNoAutoUppercase);
  ui->pLEField6->setInputMethodHints(Qt::ImhNoAutoUppercase);
  ui->pLEField7->setInputMethodHints(Qt::ImhNoAutoUppercase);
  ui->pLEField8->setInputMethodHints(Qt::ImhNoAutoUppercase);

  //Fill out the form for ActionEdit
  //If ActionEdit -> Id for table "sbitem"
  //If ActionAdd -> Id for table "sbgroup"
  m_iAction=iAction;
  if (iAction==ActionEdit)
  {
    QSqlTableModel Model;
    Model.setTable("sbitem");
    Model.setFilter("idx="+Id.toString());
    Model.select();

    //Set member variables
    m_IdItem=Id;
    m_IdGroup=Model.record(0).value(1);

    //Set controls
    ui->pLEField1->setText(Model.record(0).value(2).toString());
    ui->pLEField2->setText(Model.record(0).value(3).toString());
    ui->pLEField3->setText(Model.record(0).value(4).toString());
    ui->pLEField4->setText(Model.record(0).value(5).toString());
    ui->pLEField5->setText(Model.record(0).value(6).toString());
    ui->pLEField6->setText(Model.record(0).value(7).toString());
    ui->pLEField7->setText(Model.record(0).value(8).toString());
    ui->pLEField8->setText(Model.record(0).value(9).toString());
  }
  if (iAction==ActionAdd)
  {
    //Set member variables
    m_IdItem.clear();
    m_IdGroup=Id;
  }
}

CItemEdit::~CItemEdit()
{
  delete ui;
}

void CItemEdit::on_pPBApply_clicked()
{
  //Set model to apply changes
  QSqlTableModel ModelItem;
  ModelItem.setTable("sbitem");
  ModelItem.select();
  QSqlRecord Record;

  if (m_iAction==ActionEdit)
  {
    ModelItem.setFilter("idx="+m_IdItem.toString());
  }
  if (m_iAction==ActionAdd)
  {
    int iId=g_generateId("sbitem");
    ModelItem.insertRow(0);
    ModelItem.setData(ModelItem.index(0,0),iId);
    ModelItem.setData(ModelItem.index(0,1),m_IdGroup);
    ModelItem.submitAll();
    ModelItem.setFilter("idx="+QString::number(iId));
  }

  Record=ModelItem.record(0);

  //Set fields for item
  Record.setValue(2,ui->pLEField1->text().simplified());
  Record.setValue(3,ui->pLEField2->text().simplified());
  Record.setValue(4,ui->pLEField3->text().simplified());
  Record.setValue(5,ui->pLEField4->text().simplified());
  Record.setValue(6,ui->pLEField5->text().simplified());
  Record.setValue(7,ui->pLEField6->text().simplified());
  Record.setValue(8,ui->pLEField7->text().simplified());
  Record.setValue(9,ui->pLEField8->text().simplified());
  ModelItem.setRecord(0,Record);
  ModelItem.submitAll();

  close();
}

void CItemEdit::on_pPBCancel_clicked()
{
  close();
}

void CItemEdit::showEvent( QShowEvent */*pEvent*/ )
{
  //Write the Title from Group
  QSqlTableModel ModelGroup;
  ModelGroup.setTable("sbgroup");
  ModelGroup.setFilter("idx="+m_IdGroup.toString());
  ModelGroup.select();
  ui->pLGroup->setText("<b>"+ModelGroup.record(0).value(1).toString()+"</b>");

  //Write the name for the fields
  ui->pLField1->setText(ModelGroup.record(0).value(2).toString());
  if (ui->pLField1->text().isEmpty())
    ui->pLEField1->hide();
  else
    ui->pLEField1->show();

  ui->pLField2->setText(ModelGroup.record(0).value(3).toString());
  if (ui->pLField2->text().isEmpty())
    ui->pLEField2->hide();
  else
    ui->pLEField2->show();

  ui->pLField3->setText(ModelGroup.record(0).value(4).toString());
  if (ui->pLField3->text().isEmpty())
    ui->pLEField3->hide();
  else
    ui->pLEField3->show();

  ui->pLField4->setText(ModelGroup.record(0).value(5).toString());
  if (ui->pLField4->text().isEmpty())
    ui->pLEField4->hide();
  else
    ui->pLEField4->show();

  ui->pLField5->setText(ModelGroup.record(0).value(6).toString());
  if (ui->pLField5->text().isEmpty())
    ui->pLEField5->hide();
  else
    ui->pLEField5->show();

  ui->pLField6->setText(ModelGroup.record(0).value(7).toString());
  if (ui->pLField6->text().isEmpty())
    ui->pLEField6->hide();
  else
    ui->pLEField6->show();

  ui->pLField7->setText(ModelGroup.record(0).value(8).toString());
  if (ui->pLField7->text().isEmpty())
    ui->pLEField7->hide();
  else
    ui->pLEField7->show();

  ui->pLField8->setText(ModelGroup.record(0).value(9).toString());
  if (ui->pLField8->text().isEmpty())
    ui->pLEField8->hide();
  else
    ui->pLEField8->show();
}

