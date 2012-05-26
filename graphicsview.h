#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QtGui>
#include <maxmodel/model.h>
#include "ioentry.h"
#include "link.h"
#include "blockinstance.h"

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QGraphicsScene *scene, QWidget *parent = 0);

signals:

public slots:
    void openScript(const model_t *model, const model_script_t *script);
    void closeScript();

    void addLinkable(const model_linkable_t *linkable);
    void updateBacking();

protected:
    void drawForeground(QPainter *painter, const QRectF &rect);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    IOEntry::Handle *getHandle(IOEntry *entry);

    const model_script_t *script;

    IOEntry::Handle *dragio;
    QList<Link *> links;
};

#endif // GRAPHICSVIEW_H
