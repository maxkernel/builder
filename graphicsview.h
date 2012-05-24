#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QtGui>
#include <maxmodel/model.h>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QGraphicsScene *scene, QWidget *parent = 0);

signals:

public slots:
    void openScript(model_script_t *script);
    void closeScript();

protected:
    void drawForeground(QPainter *painter, const QRectF &rect);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    model_script_t *script;
};

#endif // GRAPHICSVIEW_H
