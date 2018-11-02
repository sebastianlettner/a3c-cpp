
#include "a3c/cc/helper/fill_server_def.h"


Status tf_helper::FillServerDef(const string& cluster_spec, const string& job_name,
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