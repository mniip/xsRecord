#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>

class AppWindow: public QWidget
{
	Q_OBJECT
private:
	QLabel *Preview;
	QPushButton *StartStop;
	QLineEdit *Output;
	QSpinBox *Fps;
	int recording;
	std::vector<int> selection;
public:
	AppWindow(QWidget *parent = 0);
public slots:
	void startStop();
	void selectRange();
	void selectWindow();
	void screenshot();
	void browse();
	void update();
};
