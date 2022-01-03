#ifndef CCALENDAR_H
#define CCALENDAR_H

#include <QDialog>
#include <QDate>

class QWidget;

namespace Ui
{
  class CCalendar;
}

class CCalendar : public QDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(CCalendar)

public:
  explicit CCalendar(QWidget *pWidget = 0);
  virtual ~CCalendar();
  QDate date();
  void setDate(const QDate &Date);

private:
  Ui::CCalendar *ui;
  QDate m_Date;

private slots:
  void on_pPBToday_clicked();
  void on_pPBYesterday_clicked();
  void on_pPBTomorrow_clicked();
  void on_pCalendarWidget_clicked(QDate date);
};

#endif // CCALENDAR_H
