#ifndef SCRIPT_H
#define SCRIPT_H

#include <QtGui>
#include <maxmodel/model.h>
#include "block.h"

class Script : public QListWidgetItem
{
public:
    Script(const QString &name, const model_script_t *script) :
        QListWidgetItem(), name(name), script(script)
    {
        setText(name);
        setIcon(QIcon(":/images/script.png"));
    }

    const model_script_t *getScript() const { return script; }
    const QString &getName() { return name; }

private:
    QString name;
    const model_script_t *script;
};

#endif // SCRIPT_H
