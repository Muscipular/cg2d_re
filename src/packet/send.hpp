#ifndef CG_ENGINE_SEND_H
#define CG_ENGINE_SEND_H
#include <string>

namespace cg::packet::send
{
    struct Packet
    {
        std::string header;
    };

    /**
     * RECV_ClientLogin 账号密码 JFVf {clientHash:es} {password:es} {account:es}
     */
    struct ClientLogin : public Packet
    {
        std::string clientHash;
        std::string password;
        std::string account;

        ClientLogin(const std::string& client_hash, const std::string& account, const std::string& password)
            : Packet("JFVf"), clientHash(client_hash),
              password(password),
              account(account)
        {
        }


        std::string Encode()
        {
            //TODO Encode
            return {};
        }
    };
}

#endif //CG_ENGINE_SEND_H
