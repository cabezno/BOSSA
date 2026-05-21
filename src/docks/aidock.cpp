#include "aidock.h"
#include "mainwindow.h"
#include "controllers/bossamissioncontrol.h"
#include "qmltypes/qmlutilities.h"
#include <QQmlContext>
#include <QVBoxLayout>

AIDock::AIDock(QWidget *parent) : QDockWidget(tr("AI Mission Control"), parent)
{
    setObjectName("AIDock");
    
    QQuickWidget *view = new QQuickWidget(QmlUtilities::sharedEngine(), this);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    
    // Get the controller from MainWindow (or we could create it here)
    // For simplicity, let's assume it's already registered or accessible.
    
    view->setSource(QUrl("qrc:/qml/views/ai/AIPanel.qml"));
    
    setWidget(view);
}
