#ifndef CDIALOGICONS_H
#define CDIALOGICONS_H

#include <QDialog>
#include <QString>

class QListWidgetItem;
class QAbstractButton;

namespace Ui
{
  class CDialogIcons;
}

class CDialogIcons : public QDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(CDialogIcons)
public:
  explicit CDialogIcons(QWidget *parent = 0);
  virtual ~CDialogIcons();
  QString icon() {return m_sIcon;}

private:
  Ui::CDialogIcons *ui;
  QString m_sIcon;

private slots:
 void on_pButtonBox_clicked(QAbstractButton* pButton);
 void on_pListWidget_itemClicked(QListWidgetItem * item);
};

#endif // CDIALOGICONS_H
