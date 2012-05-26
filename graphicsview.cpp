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

IOEntry::Handle *GraphicsView::getHandle(IOEntry *entry)
{
    foreach (QGraphicsItem *item, scene()->items())
    {
        IOEntry::Handle *h = dynamic_cast<IOEntry::Handle *>(item);
        if (h != NULL && *h->getEntry() == *entry)
        {
            return h;
        }
    }

    return NULL;
}

void GraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    if (dragio != NULL)
    {
        painter->save();
        {
            painter->setRenderHints(QPainter::Antialiasing);

            painter->setPen(QPen(Qt::blue, 3));
            painter->drawLine(getHandle(dragio->getEntry())->mapToScene(0,0), dragio->pos());

            painter->translate(dragio->pos());
            dragio->paint(painter, NULL);
        }
        painter->restore();
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    foreach (QGraphicsItem *item, items(event->pos()))
    {
        IOEntry::Handle *handle = dynamic_cast<IOEntry::Handle *>(item);
        if (handle != NULL)
        {
            IOEntry *entry = handle->getEntry();
            dragio = entry->makeHandle();
            return;
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (dragio != NULL)
    {
        dragio->setPos(mapToScene(event->pos()));

        viewport()->update();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (dragio != NULL)
    {
        foreach (QGraphicsItem *item, items(event->pos()))
        {
            IOEntry::Handle *handle = dynamic_cast<IOEntry::Handle *>(item);
            if (handle != NULL)
            {
                IOEntry *o1 = dragio->getEntry(), *o2 = handle->getEntry();


                IOEntry *out = (o1->getType() == IOEntry::Output)? o1 : (o2->getType() == IOEntry::Output)? o2 : NULL;
                IOEntry *in = (o1->getType() == IOEntry::Input)? o1 : (o2->getType() == IOEntry::Input)? o2 : NULL;

                if (out == NULL || in == NULL)
                {
                    // Can't route I->I or O->O
                    break;
                }

                // TODO - type check & array index'd shit here!

                exception_t *e = NULL;
                model_link_t *link = model_newlink(NULL, (model_script_t *)script, (model_linkable_t *)out->getLinkable(), out->getName().toAscii().constData(), (model_linkable_t *)in->getLinkable(), in->getName().toAscii().constData(), &e);
                if (link == NULL || exception_check(&e))
                {
                    QMessageBox::critical(this, "Link creation failed", QString("Could not create link.\nReason: %1").arg(e == NULL? "Unknown error" : e->message));
                    break;
                }

                QPair<IOEntry *, IOEntry *> *data = new QPair<IOEntry *, IOEntry *>(out, in);
                model_userdata(model_object(link)) = data;

                links.append(new Link(link, getHandle(out), getHandle(in)));
            }
        }

        delete dragio;
        dragio = NULL;

        viewport()->update();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}
