#ifndef BLOCKINSTANCE_H
#define BLOCKINSTANCE_H

#include <QtGui>
#include <maxmodel/model.h>
#include "ioentry.h"

class BlockInstance : public QGraphicsItem
{
public:
    BlockInstance(const QString &title, const model_linkable_t *linkable, const QPixmap &icon = QPixmap(":/images/block.png"), const QColor &color = QColor(0xCCCCCC));
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    const model_linkable_t *getLinkable() { return linkable; }
    QPair<QPointF, QList<IOEntry *> > *getData() { return data; }

private:
    QString title;
    QPixmap icon;
    QColor color;
    QRectF bounding;

    const model_linkable_t *linkable;
    QPair<QPointF, QList<IOEntry *> > *data;
};

#endif // BLOCKINSTANCE_H
