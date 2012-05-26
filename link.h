#ifndef LINK_H
#define LINK_H

#include <QtGui>
#include <maxmodel/model.h>
#include "entry.h"

class Link
{
public:
    Link(const model_link_t *link, Entry::Handle *out, Entry::Handle *in) :
        link(link), out(out), in(in)
    {
    }

    const model_link_t *getLink() const { return link; }
    Entry::Handle *getOutput() const { return out; }
    Entry::Handle *getInput() const { return in; }

    class Handle : public QGraphicsItem
    {
    public:
        Handle(Link *link) :
            QGraphicsItem(), link(link)
        {
            setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable);
        }

        Link *getLink() const { return link; }
        QRectF boundingRect() const { return QRectF(-7, 0, 14, 20); }
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
        {
            Q_UNUSED(option);
            Q_UNUSED(widget);

            QPainterPath bg;
            bg.addRect(boundingRect());

            painter->save();
            {
                painter->setRenderHints(QPainter::Antialiasing);

                painter->fillPath(bg, QBrush(hasFocus()? Qt::black : QColor(0x8370FA)));

                //painter->setPen(QPen(Qt::black));
                //painter->setFont(QFont("sans-serif", 10, QFont::Normal));
                //painter->drawText(boundingRect(), Qt::AlignCenter, QString(entry->getSig()));

            }
            painter->restore();
        }

    private:
        Link *link;
    };

    Handle *makeHandle() { return new Handle(this); }

    bool operator ==(const Link &other) const
    {
        return getLink() == other.getLink();
    }

    bool operator !=(const Link &other) const
    {
        return !(*this == other);
    }

private:
    const model_link_t *link;
    Entry::Handle *out, *in;
};

#endif // LINK_H
