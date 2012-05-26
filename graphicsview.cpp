#include "graphicsview.h"

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent), script(NULL), dragio(NULL)
{
    closeScript();
}

void GraphicsView::openScript(const model_t *model, const model_script_t *script)
{
    if (this->script != NULL)
    {
        closeScript();
    }

    // Sanity check
    {
        if (model == NULL || script == NULL)
        {
            return;
        }
    }

    this->model = model;
    this->script = script;

    iterator_t litr = model_linkableitr(model, script);
    {
        const model_linkable_t *linkable = NULL;
        while (model_linkablenext(litr, &linkable, NULL))
        {
            addLinkable(linkable);
        }
    }
    iterator_free(litr);

    setEnabled(true);
}

void GraphicsView::closeScript()
{
    updateBacking();

    // Free all the scene blocks
    {
        QList<QGraphicsItem *> items = scene()->items();
        foreach (QGraphicsItem *item, items)
        {
            scene()->removeItem(item);
            delete item;
        }
    }

    // Free all the links
    {
        foreach (Link *l, links)
        {
            delete l;
        }
        links.clear();
    }

    this->script = NULL;
    setEnabled(false);
}

void GraphicsView::addLinkable(const model_linkable_t *linkable)
{
    modeltype_t type = model_type(model_object(linkable));
    switch (type)
    {
    case model_blockinst:
    {
        const meta_t *meta = NULL;
        const model_module_t *module = NULL;
        const char *m_name = NULL, *b_name = NULL;
        model_getblockinst(linkable, &b_name, &module, NULL, NULL, NULL);
        model_getmodule(module, NULL, &meta);
        meta_getinfo(meta, NULL, &m_name, NULL, NULL, NULL);

        scene()->addItem(new BlockInstance(QString("%1.%2").arg(m_name, b_name), linkable));
        break;
    }

    case model_syscall:
    {

        break;
    }

    case model_rategroup:
    {

        break;
    }

    default: break;
    }
}

void GraphicsView::updateBacking()
{
    foreach (QGraphicsItem *item, scene()->items())
    {
        BlockInstance *bi = dynamic_cast<BlockInstance *>(item);

        if (bi != NULL)
        {
            bi->getData()->first = bi->pos();
        }
    }
}

Entry::Handle *GraphicsView::getEntryHandle(Entry *entry)
{
    foreach (QGraphicsItem *item, scene()->items())
    {
        Entry::Handle *h = dynamic_cast<Entry::Handle *>(item);
        if (h != NULL && *h->getEntry() == *entry)
        {
            return h;
        }
    }

    return NULL;
}

Link::Handle *GraphicsView::getLinkHandle(Link *link)
{
    foreach (QGraphicsItem *item, scene()->items())
    {
        Link::Handle *h = dynamic_cast<Link::Handle *>(item);
        if (h != NULL && *h->getLink() == *link)
        {
            return h;
        }
    }

    return NULL;
}

void GraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    painter->save();
    {
        painter->setRenderHints(QPainter::Antialiasing);

        painter->setPen(QPen(Qt::black, 2));
        foreach (Link *l, links)
        {
            QLineF line(l->getOutput()->mapToScene(0,0), l->getInput()->mapToScene(0,0));

            Link::Handle *h = getLinkHandle(l);
            h->setPos(line.p1() + QPointF(line.dx()/2, line.dy()/2));

            painter->drawLine(line);
        }

        if (dragio != NULL)
        {
            painter->save();
            {
                painter->setPen(QPen(Qt::blue, 3));
                painter->drawLine(getEntryHandle(dragio->getEntry())->mapToScene(0,0), dragio->pos());

                painter->translate(dragio->pos());
                dragio->paint(painter, NULL);
            }
            painter->restore();

            if (targetio != NULL)
            {
                painter->save();
                {
                    painter->setPen(QPen(Qt::black, 3));
                    painter->translate(targetio->mapToScene(0,0));
                    painter->drawRect(targetio->boundingRect());
                }
                painter->restore();
            }
        }

    }
    painter->restore();
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (script == NULL)
    {
        return;
    }

    foreach (QGraphicsItem *item, items(event->pos()))
    {
        Entry::Handle *handle = dynamic_cast<Entry::Handle *>(item);
        if (handle != NULL)
        {
            Entry *entry = handle->getEntry();
            dragio = entry->makeHandle();
            return;
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (script == NULL)
    {
        return;
    }

    viewport()->update();

    if (dragio != NULL)
    {
        targetio = NULL;

        // See if we are hovering over a valid Entry::Handle
        foreach (QGraphicsItem *item, items(event->pos()))
        {
            Entry::Handle *handle = dynamic_cast<Entry::Handle *>(item);
            if (handle != NULL)
            {
                if (*handle->getEntry() != *dragio->getEntry())
                {
                    targetio = handle;
                    break;
                }
            }
        }

        dragio->setPos(mapToScene(event->pos()));
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (script == NULL)
    {
        return;
    }

    viewport()->update();

    if (dragio != NULL)
    {
        if (targetio != NULL)
        {
            Entry *o1 = dragio->getEntry(), *o2 = targetio->getEntry();
            Entry *out = (o1->getType() == Entry::Output)? o1 : (o2->getType() == Entry::Output)? o2 : NULL;
            Entry *in = (o1->getType() == Entry::Input)? o1 : (o2->getType() == Entry::Input)? o2 : NULL;

            if (out == NULL || in == NULL)
            {
                // Can't route I->I or O->O
                goto out;
            }

            // TODO - type check & array index'd shit here!

            exception_t *e = NULL;
            model_link_t *link = model_newlink((model_t *)model, (model_script_t *)script, (model_linkable_t *)out->getLinkable(), out->getName().toAscii().constData(), (model_linkable_t *)in->getLinkable(), in->getName().toAscii().constData(), &e);
            if (link == NULL || exception_check(&e))
            {
                QMessageBox::critical(this, "Link creation failed", QString("Could not create link.\nReason: %1").arg(e == NULL? "Unknown error" : e->message));
                goto out;
            }

            QPair<Entry *, Entry *> *data = new QPair<Entry *, Entry *>(out, in);
            model_userdata(model_object(link)) = data;

            Link *l = new Link(link, getEntryHandle(out), getEntryHandle(in));
            scene()->addItem(l->makeHandle());

            links.append(l);
        }

out:

        delete dragio;
        dragio = NULL;
        targetio = NULL;

        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}
