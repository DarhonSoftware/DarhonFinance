#ifndef REPORTDRAWING_H
#define REPORTDRAWING_H

#include <QLabel>
#include <QPoint>

class QStandardItemModel;
class QWidget;

class CReportDrawing : public QLabel
{
	Q_OBJECT
public:
	enum Draw {DrawNone,DrawPie,DrawHistogram};

	CReportDrawing(QWidget *pWidget = 0);
	~CReportDrawing();
	void updateModel(QStandardItemModel *pModel,Draw iDraw);
	void updatePixmap(Draw iDraw);

private:
	void drawPie();
	void drawHistogram();

private:
	QStandardItemModel *m_pModel;
  QPoint m_Point;
  int m_iFontSize;
  int m_iFontLabelSize;
  int m_iRectPie;
  int m_iRadiusLabelA;
  int m_iRadiusLabelB;
  int m_iVMarginHis;
  int m_iHMarginHis;
  int m_iMarginPie;
  int m_iBarWidth;
  int m_iTitleMargin;

protected:
  virtual void	mousePressEvent ( QMouseEvent * pEvent );
  virtual void	mouseReleaseEvent ( QMouseEvent * pEvent );

signals:
  void hideDrawing();
};

#endif // REPORTDRAWING_H
