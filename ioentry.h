#ifndef IOENTRY_H
#define IOENTRY_H

#include <QtGui>
#include <maxmodel/model.h>

class IOEntry
{
public:
    enum Type { Unknown, Input, Output };
    IOEntry(const model_linkable_t *linkable, const Type &type, const QString &name, char sig) :
        linkable(linkable), type(type), name(name), sig(sig)
    {
    }

    const model_linkable_t *getLinkable() const { return linkable; }
    Type getType() const { return type; }
    QString getName() const { return name; }
    char getSig() const { return sig; }

    class Handle : public QGraphicsItem
    {
    public:
        Handle(IOEntry *entry) :
            QGraphicsItem(), entry(entry)
        {
        }

        IOEntry *getEntry() const { return entry; }
        QRectF boundingRect() const { return QRectF(-10, -10, 20, 20); }
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
        {
            Q_UNUSED(option);
            Q_UNUSED(widget);

            QPainterPath bg;
            bg.addRect(boundingRect());

            painter->save();
            {
                painter->setRenderHints(QPainter::Antialiasing);

                painter->fillPath(bg, QBrush(QColor(0x8370FA)));

                painter->setPen(QPen(Qt::black));
                painter->setFont(QFont("sans-serif", 10, QFont::Normal));
                painter->drawText(boundingRect(), Qt::AlignCenter, QString(entry->getSig()));
            }
            painter->restore();
        }

        /*
        bool operator ==(const Handle &other) const
        {
            return *getEntry() == *other.getEntry();
        }

        bool operator !=(const Handle &other) const
        {
            return !(*this == other);
        }
        */

    private:
        IOEntry *entry;
    };

    Handle *makeHandle() { return new Handle(this); }

    bool operator ==(const IOEntry &other) const
    {
        return getLinkable() == other.getLinkable() && getName() == other.getName() && getType() == other.getType();
    }

    bool operator !=(const IOEntry &other) const
    {
        return !(*this == other);
    }

private:
    const model_linkable_t *linkable;
    Type type;
    QString name;
    char sig;
};

#endif // IOENTRY_H
