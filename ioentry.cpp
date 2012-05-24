#include "ioentry.h"

IOEntry::IOEntry(const Type &type, const QString &name, char sig) :
    QGraphicsItem(), type(type), name(name), sig(sig)
{
}

QRectF IOEntry::boundingRect() const
{
    return QRectF(-10, -10, 20, 20);
}

void IOEntry::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(QColor(0xFF0000)));

        QPainterPath bg;
        bg.addRect(boundingRect());
        painter->fillPath(bg, QBrush(QColor(0x00FF00)));

        painter->drawText(boundingRect(), QString(sig));
    }
    painter->restore();
}
