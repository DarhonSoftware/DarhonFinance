#ifndef CLOGIN_H
#define CLOGIN_H

#include <QDialog>
#include <QByteArray>

class QWidget;

namespace Ui
{
  class CLogin;
}

class CLogin : public QDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(CLogin)
public:
  enum ETypes {TypePass,TypeLogin};
  explicit CLogin(const QByteArray& BAPass,ETypes iType,QWidget *pWidget = 0);
  virtual ~CLogin();

  QByteArray pass();

private:
  Ui::CLogin *ui;
  QByteArray m_BAPass, m_BAInput,m_BAInputConfirm;
  int m_iType;

private slots:
  void on_pPBCancel_clicked();
  void on_pPBOk_clicked();
  void on_pPBSave_clicked();
  void on_pPBClear_clicked();
  void clickDigit();
};

#endif // CLOGIN_H
