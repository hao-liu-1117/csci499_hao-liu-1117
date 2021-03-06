#ifndef FAZ_SERVER_H_
#define FAZ_SERVER_H_

#include <grpcpp/grpcpp.h>
#include <grpcpp/client_context.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "faz.grpc.pb.h"

// FazClient is called by command-line client
// and sends reqeusts to FazServer.
class FazClient {
 public:
  // explicit FazClient(std::shared_ptr<grpc::Channel> channel)
  //                       : stub_(faz::FazService::NewStub(channel)) {}
  explicit FazClient(std::shared_ptr<grpc::Channel> channel) 
                        : stub_(faz::FazService::NewStub(channel)){}

  bool Hook(const int event_type, const std::string &funcstr);

  bool Unhook(const int event_type);
  // If function is registered, return event_type. Otherwise return -1.
  // event_type == -1 stands for invalid event_type.
  int IsRegistered(const std::string &funcstr);

  faz::EventReply Event(const faz::EventRequest &request);

  // subscribe request to FazServer
  grpc::Status Subscribe(const faz::EventRequest &request);
private:
  // Client sends request using stub and waits for response.
  std::unique_ptr<faz::FazService::Stub> stub_;
};

#endif
