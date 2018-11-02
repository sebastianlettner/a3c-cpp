#include "tensorflow/core/distributed_runtime/rpc/grpc_server_lib.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_session.h"

#include "grpcpp/grpcpp.h"
#include "grpcpp/security/credentials.h"
#include "grpcpp/server_builder.h"

#include "tensorflow/core/distributed_runtime/server_lib.h"

#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/core/lib/strings/strcat.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/protobuf/cluster.pb.h"
#include "tensorflow/core/protobuf/tensorflow_server.pb.h"
#include "tensorflow/core/public/session_options.h"
#include "tensorflow/core/util/command_line_flags.h"
#include "tensorflow/core/public/session.h"
#include <unistd.h>

namespace tensorflow {
namespace {

Status FillServerDef(const string& cluster_spec, const string& job_name,
                     int task_index, ServerDef* options) {
  options->set_protocol("grpc");
  options->set_job_name(job_name);
  options->set_task_index(task_index);

  size_t my_num_tasks = 0;

  ClusterDef* const cluster = options->mutable_cluster();

  for (const string& job_str : str_util::Split(cluster_spec, ',')) {
    JobDef* const job_def = cluster->add_job();
    // Split each entry in the flag into 2 pieces, separated by "|".
    const std::vector<string> job_pieces = str_util::Split(job_str, '|');
    CHECK_EQ(2, job_pieces.size()) << job_str;
    const string& job_name = job_pieces[0];
    job_def->set_name(job_name);
    // Does a bit more validation of the tasks_per_replica.
    const StringPiece spec = job_pieces[1];
    // job_str is of form <job_name>|<host_ports>.
    const std::vector<string> host_ports = str_util::Split(spec, ';');
    for (size_t i = 0; i < host_ports.size(); ++i) {
      (*job_def->mutable_tasks())[i] = host_ports[i];
    }
    size_t num_tasks = host_ports.size();
    if (job_name == options->job_name()) {
      my_num_tasks = host_ports.size();
    }
    LOG(INFO) << "Peer " << job_name << " " << num_tasks << " {"
              << str_util::Join(host_ports, ", ") << "}";
  }
  if (my_num_tasks == 0) {
    return errors::InvalidArgument("Job name \"", options->job_name(),
                                   "\" does not appear in the cluster spec");
  }
  if (options->task_index() >= my_num_tasks) {
    return errors::InvalidArgument("Task index ", options->task_index(),
                                   " is invalid (job \"", options->job_name(),
                                   "\" contains ", my_num_tasks, " tasks");
  }
  return Status::OK();
}

}  // namespace
}  // namespace tensorflow

int main() {

    const std::string cluster_spec = "worker|192.168.0.2:5555;localhost:5554";

    tensorflow::ServerDef server_def;
    tensorflow::string job_name = "worker";
    tensorflow::Status s = tensorflow::FillServerDef(cluster_spec, job_name,
                                                    1, &server_def);

    std::unique_ptr<tensorflow::ServerInterface> server;
    TF_QCHECK_OK(NewServer(server_def, &server));
    TF_QCHECK_OK(server->Start());
    std::vector<tensorflow::Tensor> outputs; // Store outputs

    tensorflow::Session* session;
    std::string graph_definition = "/Users/sebastianlettner/GoogleDrive/MachineLearning/a3c-framework/playground/graph.pb";
    tensorflow::GraphDef graph_def;
    tensorflow::SessionOptions opts;
    opts.target = server->target();
    TF_CHECK_OK(ReadBinaryProto(tensorflow::Env::Default(), graph_definition, &graph_def));
    TF_CHECK_OK(NewSession(opts, &session));
    TF_CHECK_OK(session->Create(graph_def));
    TF_CHECK_OK(session->Run({}, {}, {"init"}, nullptr));
    int i = 0;
    while(i<20) {
        TF_CHECK_OK(session->Run({}, {"var1"}, {}, &outputs));
        auto output = outputs[0].scalar<float>();
        std::cout << "Output: " << output << std::endl;
        i++;
        sleep(1);
    }
    
    // Load graph into session
    //TF_QCHECK_OK(server->Join());

}