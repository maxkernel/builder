#ifndef LINK_H
#define LINK_H

#include <QtGui>
#include <maxmodel/model.h>
#include "ioentry.h"

class Link
{
public:
    Link(const model_link_t *link, IOEntry::Handle *out, IOEntry::Handle *in) :
        link(link), out(out), in(in)
    {
    }

private:
    const model_link_t *link;
    IOEntry::Handle *out, *in;
};

#endif // LINK_H
