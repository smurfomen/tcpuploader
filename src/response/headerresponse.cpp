#include "headerresponse.h"
#include <QtEndian>
HeaderResponse::HeaderResponse(RemoteRq& req, bool success/*=true*/)
{
    rq = req.rq;
    src = req.dst;
    dst = req.src;
    fullPath = req.fullPath;
    _success = success;
}

void HeaderResponse::Serialize  (QDataStream &stream)
{
    stream << RemoteRq::streamHeader;
    stream << RemoteRq::paServerVersion;
    stream << (int)rq;      // его переворачивать НЕ НУЖНО, т.к. он уже перевернут при десериализации
    stream << _success;
    stream << src;
    stream << dst;
    stream << fullPath;
    stream << reserv1;
    stream << reserv2;
    stream << reserv3;
}
