#include <glog/logging.h>

#include "faz_client.h"

bool FazClient::Hook(const int event_type, const std::string &funcstr) {
  grpc::ClientContext context;
  faz::HookRequest request;
  faz::HookReply reply;
  request.set_event_type(event_type);
  request.set_event_function(funcstr);
  grpc::Status status = stub_->Hook(&context, &request, &reply);
  if (!status.ok()) {
    LOG(INFO) << "status error: " << status.error_message();
    return false;
  }
  return true;
}

bool FazClient::Unhook(const int event_type, const std::string &funcstr) {
  grpc::ClientContext context;
  faz::UnhookRequest request;
  faz::UnhookReply reply;
  request.set_event_type(event_type);
  request.set_event_function(funcstr);
  grpc::Status status = stub_->Unhook(&context, &request, &reply);
  if (!status.ok()) {
    LOG(INFO) << "status error: " << status.error_message();
    return false;
  }
  return true;
}

faz::EventReply* Event(const faz::EventRequest* request) {
  grpc::ClientContext context;
  faz::EventReply *reply;
  grpc::Status status = stub_->Event(&context, request, reply);
  return reply;
}

int main() {
  return false;
}