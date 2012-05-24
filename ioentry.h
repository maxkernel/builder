#ifndef IOENTRY_H
#define IOENTRY_H

#include <QtGui>

class IOEntry : public QGraphicsItem
{
public:
    enum Type { Input, Output };
    IOEntry(const Type &type, const QString &name, char sig);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    Type type;
    QString name;
    char sig;
};

#endif // IOENTRY_H
