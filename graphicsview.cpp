#include "graphicsview.h"

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    setEnabled(false);
}

void GraphicsView::openScript(model_script_t *script)
{
    if (this->script != NULL)
    {
        closeScript();
    }

    this->script = script;
    setEnabled(true);
}

void GraphicsView::closeScript()
{
    this->script = NULL;
    setEnabled(false);
}

void GraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}
