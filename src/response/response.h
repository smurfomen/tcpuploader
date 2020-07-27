#ifndef RESPONSE_H
#define RESPONSE_H
#include "abstractresponse.h"
#include "responseabout.h"
#include "responsedirs.h"
#include "responsefileinfo.h"
#include "responsefiles.h"
#include "responsetempfile.h"
#include "responseread.h"
#include "responsedrives.h"
#include "responseerror.h"
#include "responseprntscr.h"
#include "responseprocesses.h"
#include "responseprocessesinfo.h"
#include "remoterq.h"
#include <stack>
#include <memory>
///\brief Фабрика для классов-responses.
///\details Управляет выделением памяти под реализации AbstractResponse.
/// В деструкторе память высвобождается, поэтому время жизни реализаций определяется временем жизни фабрики.
class FactoryResponse
{
public:
    ~FactoryResponse(){
        while(responses.size())
        {
            delete responses.top();
            responses.pop();
        }
    }

    AbstractResponse * fromRequest(std::shared_ptr<RemoteRq> rq, std::shared_ptr<Logger> logger = nullptr)
    {
        AbstractResponse * response = nullptr;
        switch (rq->Rq())
        {
            case rqAbout:
            {
                response = new ResponseAbout(*rq, logger.get());
                break;
            }
            case rqDirs:
            {
                response = new ResponseDirs(*rq, logger.get());
                break;
            }
            case rqFileInfo:
            {
                response = new ResponseFileInfo(*rq, logger.get());
                break;
            }
            case rqFilesInfo:
            {
                response = new ResponseFiles(*rq, logger.get());
                break;
            }
            case rqDrives:
            {
                response = new ResponseDrives(*rq, logger.get());
                break;
            }
            case rqProcesses:
            {
                response = new ResponseProcesses(*rq, logger.get());
                break;
            }
            case rqProcesseInfo:
            {
                response = new ResponseProcessesInfo(*rq, logger.get());
                break;
            }
            case rqTempFile:
            {
                response = new ResponseTempFile(*rq, logger.get());
                break;
            }
            case rqScreenshot:
            {
                response = new ResponsePrntScr(*rq, logger.get());
                break;
            }
            case rqRead:
            {
                response = new ResponseRead(*rq, logger.get());
                break;
            }
            default:
                Logger::LogStr(QString("Некорректный тип запроса #%1. Завершаемся.").arg(rq->Rq()));
                throw;
        }
        responses.push(response);
        return responses.top();
    }

private:
    std::stack<AbstractResponse*> responses;
};

#endif // RESPONSE_H
