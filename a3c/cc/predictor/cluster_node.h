
#include "tensorflow/core/distributed_runtime/rpc/grpc_server_lib.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_session.h"

#include <vector>
#include <sstream>

struct spec_t {

    const std::string cluster_spec;
    const std::string job_name;
    const std::string run_operations;
    const std::string graph_definition;
    int task_index;

} NodeSpecification;


class ClusterNode {

public:

    ClusterNode::ClusterNode(NodeSpecification specs);
    ClusterNode::~ClusterNode();

    void run_variable_initialization();

    void run_operation(std::vector<std::pair<string, tensorflow::Tensor>>& inputs, std::vector<tensorflow::Tensor>& outputs);

private:

    NodeSpecification* _specifications;

    std::istringstream _iss;
    std::vector<std::string> _run_operations;

    tensorflow::ServerDef _server_def;
    std::unique_ptr<tensorflow::ServerInterface> _server;

    tensorflow::Session* _session;

    tensorflow::GraphDef _graph_def;

    tensorflow::SessionOptions _opts;
    
}