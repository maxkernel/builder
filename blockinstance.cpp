#include "blockinstance.h"

BlockInstance::BlockInstance(const QString &title, model_linkable_t *linkable, const QPixmap &icon, const QColor &color) :
    QGraphicsItem(), title(title), icon(icon), color(color), linkable(linkable)
{
}

QRectF BlockInstance::boundingRect() const
{
    return QRectF(0,0,0,0);
}

void BlockInstance::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
