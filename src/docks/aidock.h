#ifndef AIDOCK_H
#define AIDOCK_H

#include <QDockWidget>
#include <QQuickWidget>

class AIDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit AIDock(QWidget *parent = nullptr);
};

#endif // AIDOCK_H
