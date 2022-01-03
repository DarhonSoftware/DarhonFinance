#include <QtGui>
#include <QtWidgets>
#include "purgetrans.h"
#include "ui_purgetrans.h"
#include "calendar.h"
#include "global.h"
#include "dr_profile.h"

CPurgeTrans::CPurgeTrans(QWidget *pWidget) :
    QDialog(pWidget),
    ui(new Ui::CPurgeTrans)
{
  ui->setupUi(this);

  //Setup children
  if (g_dateFormat().left(2)=="dd")
    m_sDateFormat="dd.MM.yyyy";
  else
    m_sDateFormat="MM.dd.yyyy";

  //Resize the window to optimaize the platform style
  adjustSize();

  //Set Draw buttons to be mutual exclusive
  QButtonGroup *m_pButtonGroup=new QButtonGroup(this);
  m_pButtonGroup->addButton(ui->pRBOpt1,0);
  m_pButtonGroup->addButton(ui->pRBOpt2,1);
  m_pButtonGroup->addButton(ui->pRBOpt3,2);

  //Initiate view
  QDate Date=QDate::currentDate();
  ui->pPBDate->setText(Date.toString(m_sDateFormat));
  this->setWindowTitle(tr("Purge transactions"));

  //Connect signals
  connect(m_pButtonGroup,SIGNAL(buttonClicked(int)),this,SLOT(setChoice(int)));
  ui->pRBOpt1->click();
}

CPurgeTrans::~CPurgeTrans()
{
  delete ui;
}

int CPurgeTrans::choice()
{
  return m_iChoice;
}

QDate CPurgeTrans::date()
{
  return QDate::fromString(ui->pPBDate->text(),m_sDateFormat);
}

void	CPurgeTrans::setChoice(int iChoice)
{
  m_iChoice=iChoice;
  if (m_iChoice==0)
    ui->pPBDate->setEnabled(true);
  else
    ui->pPBDate->setEnabled(false);
}

void CPurgeTrans::on_pPBDate_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBDate->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBDate->setText(Date.toString(m_sDateFormat));
  }
}
