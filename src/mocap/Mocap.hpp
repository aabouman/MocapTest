#pragma once

#include "string"
#include "functional"
#include "vector"

#include <NatNetTypes.h>
#include <NatNetCAPI.h>
#include <NatNetClient.h>
#include <pthread.h>

namespace rexquad {

class Mocap {
   public:
    using CallbackFunction = std::function<void(const sRigidBodyData&)>;

    Mocap();
    ~Mocap();
    void SetServerAddress(const std::string& addr);
    void SetLocalAddress(const std::string& addr);
    int ConnectClient();
    void SendTestRequest();
    void ResetClient();
    NatNetClient* GetClient() { return _pClient; };
    const sServerDescription& GetServerDescription() const;
    void AddCallback(const CallbackFunction& callback);
    void RunCallbacks(const sRigidBodyData&);
    int Run();
    void SetVerbose(bool verbose) { verbose_ = verbose; }
    bool IsVerbose() const { return verbose_; }
    void PrintVersion() const ;
    float GetFramerate() const; 
    bool DoesConvertToZUp() const;
    void ConvertToZUp(bool flag);

   private:
    bool verbose_ = true;
    bool do_convert_to_z_up_ = false;
    std::string _local_address;
    std::string _server_address;
    NatNetClient* _pClient;  
    sNatNetClientConnectParams _connectParams;
    sServerDescription _serverDescription;
    std::vector<CallbackFunction> _callbacks;
    int _analogSamplesPerMocapFrame = 0;

};

}  // namespace rexquad