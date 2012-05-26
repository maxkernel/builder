#ifndef BLOCK_H
#define BLOCK_H

#include <QtGui>
#include "blockinstance.h"

typedef model_linkable_t *(*create_f)(model_t *model, model_script_t *script, const QString &name, const void *userdata);

class Block : public QTreeWidgetItem
{
public:
    Block(QTreeWidgetItem *parent, const QIcon &icon, const QString &name, create_f cb, const void *userdata) :
        QTreeWidgetItem(parent), name(name), callback(cb), userdata(userdata)
    {
        setText(0, name);
        setIcon(0, icon);
    }

    model_linkable_t *create(model_t *model, model_script_t *script)
    {
        return this->callback(model, script, name, userdata);
    }

private:
    QString name;
    create_f callback;
    const void *userdata;
};

#endif // BLOCK_H
