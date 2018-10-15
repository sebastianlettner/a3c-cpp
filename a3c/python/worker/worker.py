# The Worker. Its called as a subprocess from the server.
# Performs updates on the tensorflow graph.

from multiprocessing import Process
import tensorflow as tf
import numpy as np
import configparser
import time
import json
import zmq
import pickle


class Worker(Process):

    """

    """
    def __init__(self, worker_id, builder, config_file_dir, time_max):

        """
        Initializes object.
        Args:
            worker_id(int): Identifier.
            builder(GraphBuilder):
            config_file_dir(str): Path to the directory where the config file are placed.
            time_max(int): Batch size. After every time_max steps an update is made.
                           The batch is possibly smaller than time_max.
        """
        super(Worker, self).__init__()

        self.start_time = 0

        self.worker_id = worker_id

        self.graph_builder = builder

        with open(config_file_dir + '/cluster.json', 'r') as fh:
            self.cluster_spec = json.load(fh)

        self.device = "/job:worker/task{}".format(worker_id)

        self.path_to_config_file = config_file_dir + '/worker.ini'
        self.worker_config = configparser.ConfigParser()
        self.worker_config.read(self.path_to_config_file)

        self.time_max = time_max

        self.should_stop = False

    def build_tf_graph(self):
        """

        Returns:

        """
        with tf.device(self.device):
            return self.graph_builder.build_tf_graph(self.graph_builder.define_graph())

    @staticmethod
    def get_recurrent_keys(compute_graph):
        """

        Args:
            compute_graph:

        Returns:

        """
        recurrent_cells = compute_graph.recurrent_cells
        recurrent_state_keys = []
        for cell in recurrent_cells:
            recurrent_state_keys.append(cell.interface.get_state_names()[0])
            recurrent_state_keys.append(cell.interface.get_state_names()[1])

        return recurrent_state_keys

    def connect_to_server(self, context):

        """
        First, subscribe to the servers publish socket. This is mainly for receiving the kill signal.
        Second, connect to the socket that send the experience data.
        Returns:

        """
        socket_sub = context.socket(zmq.SUB)
        socket_rep = context.socket(zmq.REP)
        socket_sub.connect(self.worker_config['ADDRESSES']['SERVER_INTERFACE_SUB'])
        socket_rep.connect(self.worker_config['ADDRESSES']['SERVER_INTERFACE_REP'])
        return socket_rep, socket_sub

    def dynamic_pad(self, experience_batch, recurrent_keys):

        """
        Dynamic pad batches of size < time max to time max.
        Args:
            experience_batch(dict): The batches for every input.
            recurrent_keys(list): List of strings. Contains the names of the recurrent cells
                                  hidden state as defined in the graph configuration (tf-graph-tool)
        Returns:
            padded_exp_batch(dict): The padded batched
        """
        required_size = self.time_max
        for key, batch in experience_batch.iteritems():
            if key not in recurrent_keys:
                z = np.zeros((required_size - len(batch),) + batch.shape[1:])
                experience_batch[key] = np.append(batch, z, axis=0)
        return experience_batch

    @staticmethod
    def deserialize_exp_batch(msg):
        """
        Deserialize the message coming form the server containing the experience.
        The basic structure is a dictionary where the values are serialized with pickle.
        All values are expected to be of type np.ndarray.
        Args:
            msg(dict):

        Returns:
            msg(dict): Same dict with deserialized arrays
        """
        for key, value in msg.iteritems():
            msg[key] = pickle.loads(msg[key])

        return msg

    def run(self):
        """

        Returns:

        """
        server = tf.train.Server(
            self.cluster_spec,
            job_name="worker",
            task_index=self.worker_id
        )
        compute_graph = self.build_tf_graph()
        compute_graph.create_session(target=server.target)
        while len(tf.report_uninitialized_variables()) > 0:
            print "Worker {}: Waiting for variable initialization".format(self.worker_id)
            time.sleep(1)

        context = zmq.Context()

        socket_rep, socket_pub = self.connect_to_server(context=context)

        msg_in = zmq.Poller()
        msg_in.register(socket_rep)
        msg_in.register(socket_pub)

        while not self.should_stop:

            socks = dict(msg_in.poll())

            # data
            if socket_rep in socks and socks[socket_rep] == zmq.POLLIN:
                msg = socket_rep.recv_json()
                exp_batch = Worker.deserialize_exp_batch(msg)
                # compute_graph.run_multiple_operations(ops?, msg)

            if socket_pub in socks and socks[socket_pub]:
                msg = socket_pub.recv()
                if msg == 'KILL':
                    self.should_stop = True

