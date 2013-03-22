#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H

#include <string>
#include <Poco/Types.h>

using std::string;
using Poco::UInt16;

class ClientInfo{
    public:
        ClientInfo(const string& clientId, const string& ip, UInt16 messagePort); 
        ~ClientInfo();

        string ClientId() const{
            return clientId_;
        }

        string Ip() const{
            return ip_;
        }

        UInt16 MessagePort() const{
            return messagePort_;
        }

    private:
        string clientId_;
        string ip_;
        UInt16 messagePort_;
};

#endif
