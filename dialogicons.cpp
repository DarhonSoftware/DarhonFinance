#include <QtGui>
#include <QtWidgets>
#include "dialogicons.h"
#include "ui_dialogicons.h"
#include "dr_profile.h"

#define DIR_ICONS ":/user/icons-user"

CDialogIcons::CDialogIcons(QWidget *pWidget) :
    QDialog(pWidget),
    ui(new Ui::CDialogIcons)
{
  ui->setupUi(this);

  //Load the icons in the ListView
  QDir Dir(DIR_ICONS);
  QStringList lstIcons=Dir.entryList(QDir::Files);

  for (int i=0; i<lstIcons.count(); i++)
  {
    QListWidgetItem *pItem=new QListWidgetItem(ui->pListWidget);
    QString sFile=DIR_ICONS+QString("/")+lstIcons.at(i);
    pItem->setIcon(QIcon(sFile));
    pItem->setData(Qt::UserRole,sFile);
  }
  m_sIcon.clear();

  //Resize the window to optimaize the platform style
  adjustSize();
}

CDialogIcons::~CDialogIcons()
{
  delete ui;
}

void CDialogIcons::on_pListWidget_itemClicked(QListWidgetItem * /*item*/)
{
  m_sIcon=ui->pListWidget->currentItem()->data(Qt::UserRole).toString();
  accept();
}


void CDialogIcons::on_pButtonBox_clicked(QAbstractButton* pButton)
{
  if (pButton==ui->pButtonBox->button(QDialogButtonBox::Reset))
  {
    m_sIcon.clear();
    accept();
  }
}
