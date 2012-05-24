#ifndef MODULE_H
#define MODULE_H

#include <QtGui>
#include <maxmodel/model.h>

class Module : public QTreeWidgetItem
{
public:
    Module(QTreeWidgetItem *parent, const QIcon &icon, const QString &name, const model_module_t *module) :
        QTreeWidgetItem(parent), name(name), module(module)
    {
        setText(0, name);
        setIcon(0, icon);
    }

    const model_module_t *getModule()
    {
        return module;
    }

private:
    QString name;
    const model_module_t *module;
};

#endif // MODULE_H
