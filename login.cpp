#include <QtGui>
#include <QtWidgets>
#include "login.h"
#include "ui_login.h"
#include "dr_profile.h"
#include "global.h"

#define SETPASS_STEP1   QT_TRANSLATE_NOOP("Global","Type your current password and click 'Ok'")
#define SETPASS_STEP2   QT_TRANSLATE_NOOP("Global","Type a new password and click 'Save'")
#define SETPASS_STEP3   QT_TRANSLATE_NOOP("Global","Type again to confirm and click 'Save'")

CLogin::CLogin(const QByteArray& BAPass, ETypes iType, QWidget *pWidget) :
    QDialog(pWidget),
    ui(new Ui::CLogin)
{
  ui->setupUi(this);

  //Resize the window to optimaize the platform style
  adjustSize();

  m_BAPass=BAPass;
  m_iType=iType;
  m_BAInput.clear();
  m_BAInputConfirm.clear();
  ui->pPBClear->setEnabled(false);
  ui->pPBSave->setEnabled(false);

  if (iType==TypePass)
  {
    setWindowTitle(tr("Set Password"));
    ui->pWidgetControls->setEnabled(false);
    ui->pWidgetControls->show();
    ui->pLInfo->setText(QCoreApplication::translate("Global",SETPASS_STEP1));
  }

  if (iType==TypeLogin)
  {
    setWindowTitle(tr("Login"));
    ui->pWidgetControls->hide();
  }

  //Create connections
  connect(ui->pPB0,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB1,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB2,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB3,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB4,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB5,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB6,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB7,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB8,SIGNAL(clicked()),this,SLOT(clickDigit()));
  connect(ui->pPB9,SIGNAL(clicked()),this,SLOT(clickDigit()));
}

CLogin::~CLogin()
{
  delete ui;
}

void CLogin::on_pPBClear_clicked()
{
  m_BAInput.clear();
  ui->pPBClear->setEnabled(false);
  ui->pPBSave->setEnabled(false);
}

void CLogin::on_pPBSave_clicked()
{
  //In case nothing was typed
  if (m_BAInput.isEmpty()) return;

  //In case the first try was typed
  if (m_BAInputConfirm.isEmpty())
  {
    m_BAInputConfirm=m_BAInput;
    m_BAInput.clear();
    ui->pLInfo->setText(QCoreApplication::translate("Global",SETPASS_STEP3));
    ui->pPBClear->setEnabled(false);
    ui->pPBSave->setEnabled(false);
    return;
  }

  //In case the second try was typed and it is wrong
  if (m_BAInput!=m_BAInputConfirm)
  {
    m_BAInput.clear();
    m_BAInputConfirm.clear();
    ui->pLInfo->setText(QCoreApplication::translate("Global",SETPASS_STEP2));
    ui->pPBClear->setEnabled(false);
    ui->pPBSave->setEnabled(false);
    return;
  }

  //In case the second try was typed and it is right
  m_BAPass=m_BAInput;
  close();
}

void CLogin::clickDigit()
{
  //Record the digits typed by the user
  m_BAInput+=sender()->property("digit").toByteArray();

  //Set controls dirty if user is in STEP 2 (set pass)
  if (ui->pWidgetControls->isEnabled())
  {
    ui->pPBClear->setEnabled(true);
    ui->pPBSave->setEnabled(true);
  }

  //Clear message "wrongpass" (login)
  if (m_iType==TypeLogin)
  {
    ui->pLInfo->setText("");
  }
}


QByteArray CLogin::pass()
{
  return m_BAPass;
}

void CLogin::on_pPBOk_clicked()
{
  if (m_iType==TypeLogin)
  {
    if(m_BAInput.isEmpty()) return;
    m_BAPass=m_BAInput;
    close();
  }

  if (m_iType==TypePass)
  {
    if (m_BAPass==m_BAInput)
    {
      ui->pWidgetControls->setEnabled(true);
      ui->pLInfo->setText(QCoreApplication::translate("Global",SETPASS_STEP2));
      ui->pPBOk->setEnabled(false);
    }
    m_BAInput.clear();
  }
}

void CLogin::on_pPBCancel_clicked()
{
  m_BAPass.clear();
  close();
}

