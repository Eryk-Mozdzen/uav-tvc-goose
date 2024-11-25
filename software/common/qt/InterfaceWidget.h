#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>

namespace common {

class InterfaceWidget : public QGroupBox {
    Q_OBJECT

	QComboBox *listComboBox;
	QPushButton *scanButton;
	QPushButton *saveButton;
	QSettings settings;
	QLabel *uiLabels[4];

public slots:
	void status(const QString st);
	void stats(const int download, const int upload, const int errors);
	void scanFinished(const QStringList &list);

signals:
	void scan();
	void change(const QString input);

public:
	InterfaceWidget(const QString name, QWidget *parent = nullptr);
	void forceScan();
};

}
