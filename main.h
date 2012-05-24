#ifndef MAIN_H
#define MAIN_H

#include <QtCore>
#include <maxmodel/model.h>

namespace Global
{

const char *getType(char t);

model_t *open(const QString &filepath);
bool save(model_t *model, const QString &filepath);

}

#endif // MAIN_H
