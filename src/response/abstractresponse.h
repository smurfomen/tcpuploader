#ifndef ABSTRACTRESPONSE_H
#define ABSTRACTRESPONSE_H

class AbstractResponse
{
public:
    virtual ~AbstractResponse() = default;
    virtual class QByteArray Serialize() = 0;
    virtual void Deserialize(class QDataStream& stream) = 0;
};

#endif // ABSTRACTRESPONSE_H
