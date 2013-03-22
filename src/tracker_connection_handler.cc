#include "tracker_connection_handler.h"
#include "netpack.h"
#include "tracker.h"
#include "payload_type.h"
#include <Poco/Util/Application.h>

using Poco::Util::Application;

TrackerConnectionHandler::TrackerConnectionHandler(const StreamSocket& sock, SocketReactor& reactor):
    sock_(sock),
    reactor_(reactor),
    logger_( Application::instance().logger() ),
    app_( dynamic_cast<Tracker&>(Application::instance()) ),
    session_( app_.session() )
{
    poco_information_f1(logger_, "Connection from %s", sock_.peerAddress().toString() );

    reactor_.addEventHandler(sock_, 
                        NObserver<TrackerConnectionHandler, ReadableNotification>(*this, &TrackerConnectionHandler::onReadable));

    reactor_.addEventHandler(sock_, 
                        NObserver<TrackerConnectionHandler, ShutdownNotification>(*this, &TrackerConnectionHandler::onShutdown));
}

TrackerConnectionHandler::~TrackerConnectionHandler()
{
    poco_information_f1(logger_, "Disconnect from %s", sock_.peerAddress().toString() );

    reactor_.removeEventHandler(sock_, 
                        NObserver<TrackerConnectionHandler, ReadableNotification>(*this, &TrackerConnectionHandler::onReadable));

    reactor_.removeEventHandler(sock_, 
                        NObserver<TrackerConnectionHandler, ShutdownNotification>(*this, &TrackerConnectionHandler::onShutdown));
}

void TrackerConnectionHandler::onReadable(const AutoPtr<ReadableNotification>& pNotification)
{

    NetPack pack;
    NetPack retMsg(1, "msg from server.");

    int ret = pack.receiveFrom( sock_ );

    if( ret ){
        poco_warning_f2(logger_, "pack.receiveFrom error! ret : %d, remote addr : %s.", 
                ret, sock_.peerAddress().toString());
        goto err;
    }

    poco_notice_f1(logger_, "header : \n%s", pack.debug_string() );

    this->Process(pack, &retMsg);

    ret = retMsg.sendBy( sock_ );
    if( ret ){
        poco_warning_f2( logger_, "pack.sendBy error ! ret : %d, remote addr : %s.", 
                ret, sock_.peerAddress().toString() );
        goto err;
    }

err:
    delete this;
    return;
}

void TrackerConnectionHandler::onShutdown(const AutoPtr<ShutdownNotification>& pNotification)
{
    poco_information_f1(logger_, "shutdown connection from %s.", sock_.peerAddress().toString());
    delete this;
}

void TrackerConnectionHandler::Process(const NetPack& in, NetPack* out){
    switch( in.payloadtype() ){
        case PAYLOAD_LOGIN: 
             break;
        case PAYLOAD_LOGOUT: 
             break;
        case PAYLOAD_REQUEST_PEER:
             break;
    case PAYLOAD_REPORT_PROGRESS:
             break;
        case PAYLOAD_PUBLISH_RESOURCE:
             break;
        default:
             poco_warning_f2(logger_, "Unknown PayloadType : %d , remote addr : %s.", in.payloadtype(), sock_.peerAddress().toString() );
             break;
    }
}

retcode_t TrackerConnectionHandler::HandleLogin(const string& in, NetPack* out){
    const static string PROTONAME("TrackerProto.Login");
    Message* pMessage = NULL;
    retcode_t ret = ERROR_OK;
    ret = ParseProto(PROTONAME, pMessage);
    if( ERROR_OK != ret ){
        return ret;
    }
    if( !pMessage ){
        return ERROR_PROTO_PARSE_ERROR;
    }

    Login* loginProto = dynamic_cast<Login*>( pMessage );
    if( !loginProto ){
        return ERROR_PROTO_PARSE_ERROR;
    }

    SharedPtr<Login> msg( loginProto );
   
    string ip = msg->has_loginip() ? msg->loginip() : sock_.peerAddress().toString();
    Tracker::ClientPtr client( new ClientInfo(msg->clientid(), ip, msg->messageport()) );
    ret = app_.AddOnlineUser(msg->ClientId(), client);
    if( ret != ERROR_OK ){
        poco_warning_f1("Cannot add online user, client id : %s, client ip : %s, retCode : %d.", 
                msg->clientid(), ip, ret);
        return ret;
    }
    return ret;
}
retcode_t TrackerConnectionHandler::HandleLogOut(const string& in, NetPack* out);
retcode_t TrackerConnectionHandler::HandleRequestPeer(const string& in, NetPack* out);
retcode_t TrackerConnectionHandler::HandleReportProgress(const string& in, NetPack* out);
retcode_t TrackerConnectionHandler::HandlePublishResource(const string& in, NetPack* out);

retcode_t TrackerConnectionHandler::ParseProto(const string& name, Message* &proto){
    message = NULL;
    const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(name);
    if (descriptor)
    {
        const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype)
        {
          message = prototype->New();
        }
    }
    return message;
}
