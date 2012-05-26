#include "blockinstance.h"

#define BORDER_MARGIN       12
#define ICON_OFFSET         25
#define HEADER_HEIGHT       50
#define ENTRY_HEIGHT        25
#define FOOTER_HEIGHT       15


BlockInstance::BlockInstance(const QString &title, const model_linkable_t *linkable, const QPixmap &icon, const QColor &color) :
    QGraphicsItem(), title(title), icon(icon), color(color), linkable(linkable)
{
    data = (QPair<QPointF, QList<Entry *> > *)model_userdata(model_object(linkable));

    QGraphicsSimpleTextItem *name = new QGraphicsSimpleTextItem(title, this);
    name->setFont(QFont("sans-serif", 11, QFont::Bold));
    name->setPos(ICON_OFFSET, 0);
    int width = QFontMetrics(name->font()).width(title) + ICON_OFFSET * 2;

    QSet<QString> ios;
    foreach (Entry *e, data->second)
    {
        ios.insert(e->getName());
    }

    QList<QString> ios_list = ios.toList();
    qSort(ios_list);

    int offset = HEADER_HEIGHT;
    foreach (QString n, ios_list)
    {
        QGraphicsSimpleTextItem *entry = new QGraphicsSimpleTextItem(n, this);
        entry->setPos( width / 2 - entry->boundingRect().width() / 2, offset + ENTRY_HEIGHT / 2 - entry->boundingRect().height() / 2);
        foreach (Entry *e, data->second)
        {
            if (e->getName() == n)
            {
                Entry::Handle *handle = e->makeHandle();
                handle->setParentItem(this);
                handle->setPos(e->getType() == Entry::Input? 0 : width, offset + ENTRY_HEIGHT / 2);
            }
        }

        offset += ENTRY_HEIGHT;
    }

    bounding = QRectF(0, 0, width, offset + FOOTER_HEIGHT);
    setPos(data->first);

    setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

QRectF BlockInstance::boundingRect() const
{
    return QRectF(bounding.x() - BORDER_MARGIN, bounding.y() - BORDER_MARGIN, bounding.width() + BORDER_MARGIN * 2, bounding.height() + BORDER_MARGIN * 2);
}

void BlockInstance::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, hasFocus()? 3 : 1));

    QPainterPath bg;
    bg.addRoundedRect(bounding, 4, 4);
    painter->fillPath(bg, QBrush(color));
    painter->drawPath(bg);

    painter->drawPixmap(3, 3, icon);

    painter->restore();
}
