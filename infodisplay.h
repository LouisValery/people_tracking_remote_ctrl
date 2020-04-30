#ifndef INFODISPLAY_H
#define INFODISPLAY_H

#include <QObject>

class InfoDisplay : public QObject {
    Q_OBJECT
public:
    InfoDisplay();
    ~InfoDisplay();
public slots:
    void process();
signals:
    void finished();
    void error(QString err);
private:
    // add your variables here
};

#endif // INFODISPLAY_H
