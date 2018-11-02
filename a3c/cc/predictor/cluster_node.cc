 
#include "a3c/cc/predictor/tf_node.h"

ClusterNode::ClusterNode(NodeSpecification specs):
    _specification(&specs),
    _iss(_specifications->run_operations),
    _run_operations((std::istream_iterator<std::string>(_iss)), std::istream_iterator<std::string>())
    {
        TF_CHECK_OK(
            tensorflow::FillServerDef(
                _specification->cluster_spec,
                _specification->job_name,
                _specification->task_index,
                &_server_def
            )
        );
        TF_CHECK_OK(NewServer(server_def, &_server));
        TF_CHECK_OK(server->Start());
        _opts.target = _server->target();

        TF_CHECK_OK(ReadBinaryProto(tensorflow::Env::Default(), _graph_definition, &_graph_def));
        TF_CHECK_OK(NewSession(_opts, &_session));
        TF_CHECK_OK(session->Create(_graph_def));
    }

void ClusterNode::run_variable_initialization() {
    // Initialize all variables. 
    // The graph must hold an op called init which implements tf.global_variables_initializer()
    TF_CHECK_OK(ClusterNode::_session->Run({}, {}, {"init"}, nullptr));
}
void v::run_operation(std::vector<std::pair<string, tensorflow::Tensor>>& inputs, std::vector<tensorflow::Tensor>& outputs) {

    //
    // Run the specified operations 
    //
    // Args:
    //     inputs: The feed dict. A mapping of the placeholder name to the value
    //     outputs: A vector to store the graph output in.
    //
    // Return:
    //     
    TF_CHECK_OK(ClusterNode::_session->Run(inputs, _run_operations, {}, outputs));
}