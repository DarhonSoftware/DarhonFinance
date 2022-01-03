#include <QtGui>
#include "calendar.h"
#include "ui_calendar.h"
#include "dr_profile.h"

CCalendar::CCalendar(QWidget *pWidget) :
    QDialog(pWidget),
    ui(new Ui::CCalendar)
{
  ui->setupUi(this);

  //Resize the window to optimaize the platform style
  adjustSize();

  QTextCharFormat CharFormat;
  CharFormat.setFontWeight(QFont::Bold);
  CharFormat.setFontUnderline(true);
  ui->pCalendarWidget->setDateTextFormat(QDate::currentDate(),CharFormat);
}

CCalendar::~CCalendar()
{
  delete ui;
}

QDate CCalendar::date()
{
  return m_Date;
}

void CCalendar::setDate(const QDate &Date)
{
  m_Date=Date;
  ui->pCalendarWidget->setSelectedDate(Date);
}

void CCalendar::on_pCalendarWidget_clicked(QDate Date)
{
  m_Date=Date;
  accept();
}

void CCalendar::on_pPBTomorrow_clicked()
{
  m_Date=QDate::currentDate().addDays(1);
  accept();
}

void CCalendar::on_pPBYesterday_clicked()
{
  m_Date=QDate::currentDate().addDays(-1);
  accept();
}

void CCalendar::on_pPBToday_clicked()
{
  m_Date=QDate::currentDate();
  accept();
}
