#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "transinfo.h"
#include "ui_transinfo.h"
#include "global.h"
#include "dr_profile.h"
#include "mainwindow0.h"

CTransInfo::CTransInfo(const QVariant& Id,QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CTransInfo)
{
  ui->setupUi(this);

  //Setup children
  ui->pStatusWidget->setFrameStyle(CStatusWidget::FrameNo);
#ifdef MOBILE_PLATFORM
  ui->pTENote->setVisible(false);
#else
  ui->pPBMemo->hide();
#endif

  //Set Model
  m_pModelSplit=new QStandardItemModel(this);

  m_pModel=new QSqlTableModel(this);
  m_pModel->setTable("trans");
  m_pModel->setFilter("idx="+Id.toString());

  m_pModelAccount=new QSqlTableModel(this);
  m_pModelAccount->setTable("account");
  m_pModelCategory=new QSqlTableModel(this);
  m_pModelCategory->setTable("category");
  m_pModelPayee=new QSqlTableModel(this);
  m_pModelPayee->setTable("payee");
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
}

CTransInfo::~CTransInfo()
{
  delete ui;
}

void CTransInfo::showEvent( QShowEvent */*pEvent*/ )
{
  m_pModel->select();

  //Filter referenced tables with the content of 'trans'
  m_pModelAccount->setFilter("idx="+m_pModel->record(0).value(6).toString());
  m_pModelAccount->select();
  m_pModelCategory->setFilter("idx="+m_pModel->record(0).value(7).toString());
  m_pModelCategory->select();
  m_pModelPayee->setFilter("idx="+m_pModel->record(0).value(5).toString());
  m_pModelPayee->select();
  m_pModelClass->setFilter("idx="+m_pModel->record(0).value(4).toString());
  m_pModelClass->select();

  //Fill out controls from "trans" table
  ui->pStatusWidget->setState(m_pModel->record(0).value(2).toInt());
  QDate Date=QDate::fromJulianDay(m_pModel->record(0).value(3).toInt());
  ui->pLDate->setText(QLocale::system().toString(Date));
  ui->pLCheque->setText(m_pModel->record(0).value(8).toString());
  ui->pTENote->setHtml(m_pModel->record(0).value(10).toString());

  //Format amount
  double d=m_pModel->record(0).value(9).toDouble();
  QLocale Locale;
  if (d<0)
    ui->pLAmount->setText("("+Locale.toString(d*-1.0,'f',2)+")");
  else
    ui->pLAmount->setText(Locale.toString(d,'f',2));

  //Set controls in case this is a split
  if (m_pModel->record(0).value(7).toInt()==ID_SPLIT)
  {
    //Load childdren transactions
    QSqlTableModel ModelTrans;
    ModelTrans.setTable("trans");
    ModelTrans.setFilter("split="+QString::number(SPLIT+(m_pModel->record(0).value(14).toInt())));
    ModelTrans.select();

    m_pModelSplit->clear();
    int iRowCount=g_rowCount(ModelTrans);
    for (int iRow=0;iRow<iRowCount;iRow++)
    {
      m_pModelSplit->setItem(iRow,0,new QStandardItem(ModelTrans.record(iRow).value(7).toString()));  //Category
      m_pModelSplit->setItem(iRow,1,new QStandardItem(ModelTrans.record(iRow).value(4).toString()));  //Class
      m_pModelSplit->setItem(iRow,2,new QStandardItem(ModelTrans.record(iRow).value(9).toString()));  //Amount
      m_pModelSplit->setItem(iRow,3,new QStandardItem(ModelTrans.record(iRow).value(10).toString())); //Note
      m_pModelSplit->setItem(iRow,4,new QStandardItem(ModelTrans.record(iRow).value(13).toString())); //Aux
    }
    ui->pPBSplit->setEnabled(true);
  }
  else
    ui->pPBSplit->setEnabled(false);

  //Fill out controls from "account" table
  ui->pLAccount->setText("<b>"+m_pModelAccount->record(0).value(3).toString()+"</b>");
  ui->pLIconAccount->setPixmap(QPixmap(m_pModelAccount->record(0).value(1).toString()));

  //Write the Currency symbol from Currency
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+m_pModelAccount->record(0).value(5).toString());
  ModelCurr.select();
  ui->pLCurrency->setText(ModelCurr.record(0).value(3).toString());

  //Fill out controls from "category" table
  int iCategory=m_pModel->record(0).value(7).toInt();
  if ((iCategory>=0)||(iCategory==ID_SPLIT))
  {
    ui->pLCategory->setText(m_pModelCategory->record(0).value(1).toString());
    ui->pLIconCategory->setPixmap(QPixmap(m_pModelCategory->record(0).value(2).toString()));
  }
  else
  {
    m_pModelAccount->setFilter("idx="+QString::number(iCategory*-1));
    m_pModelAccount->select();
    ui->pLCategory->setText("["+m_pModelAccount->record(0).value(3).toString()+"]");
    ui->pLIconCategory->clear();
  }

  //Fill out controls from "payee" table
  ui->pLPayee->setText(m_pModelPayee->record(0).value(1).toString());

  //Fill out controls from "class" table
  ui->pLClass->setText(m_pModelClass->record(0).value(1).toString());
}

void CTransInfo::on_pPBClose_clicked()
{
  close();
}

void CTransInfo::on_pPBMemo_toggled(bool bChecked)
{
  ui->pPBClose->setVisible(!bChecked);
  ui->pWidgetControls->setVisible(!bChecked);
  ui->pTENote->setVisible(bChecked);
}

void CTransInfo::on_pPBSplit_clicked()
{
  g_pMainWindow->addFormSplit(m_pModelSplit,m_pModel->record(0).value(6),m_pModel->record(0).value(9).toDouble(),false,true);
}
