#include "brieffileinfo.h"

// конструктор
BriefFileInfo::BriefFileInfo (QFileInfo& fi)
{
    fill(fi);
}

void BriefFileInfo::fill(QFileInfo& fi)
{
    _name           = fi.fileName();
    _lastChanged    = fi.lastModified();
    _length         = fi.size();
    _created        = fi.created();
    _version        = "0";
    _attrib         = "";
    if (fi.isReadable()) _attrib += "R";
    if (fi.isWritable()) _attrib += "W";
    if (fi.isHidden())    _attrib += "H";
    if (fi.isExecutable()) _attrib += "E";
}

