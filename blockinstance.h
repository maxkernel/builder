#ifndef BLOCKINSTANCE_H
#define BLOCKINSTANCE_H

#include <QtGui>
#include <maxmodel/model.h>

class BlockInstance : public QGraphicsItem
{
public:
    BlockInstance(const QString &title, model_linkable_t *linkable, const QPixmap &icon = QPixmap(":/images/block.png"), const QColor &color = QColor(0x555555));
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    QString title;
    QPixmap icon;
    QColor color;

    model_linkable_t *linkable;
};

#endif // BLOCKINSTANCE_H
