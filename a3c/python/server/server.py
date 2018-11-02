# The Server.
# It manages the complete program flow and is its central unit.
# From the server the c++ processes are started and terminated.

import multiprocessing as mp
import zmq
import time
import configparser
import session_manager as session
import json


class Server(mp.Process):

    """

    """

    def __init__(self, tf_graph_builder, config_file_dir):
        """
        Initializes object
        Args:
            tf_graph_builder(GraphBuilder): Graph builder from the tf-graph-tool. The graph is not built yet.
            config_file_dir(str): Path to the config files directory.
        """
        super(Server, self).__init__()

        self.path_to_config_file = config_file_dir + '/server.ini'
        self.server_config = configparser.ConfigParser()
        self.server_config.read(self.path_to_config_file)

        with open(config_file_dir + '/cluster.json', 'r') as fh:
            self.cluster_spec = json.load(fh)

        self.num_workers = len(self.cluster_spec['worker'])

        # create the session manager.
        self.session_manager = session.SessionManager(
            builder=tf_graph_builder,
            config_file_dir=self.server_config['TF_GRAPH_MANAGEMENT']['LOG_DIR'],
            cluster_spec=self.cluster_spec
        )
        self.episode_log_q = self.session_manager.episode_log_q

    def add_worker(self):
        """
        Boot new worker.
        Returns:

        """
        pass

    def remove_worker(self):
        """
        Remove worker. Only possible if more than one worker is online.
        Returns:

        """

    def signal_worker_shutdown(self):
        """
        Signal the workers to shut down.
        Returns:

        """
        pass
        

    def run(self):

        """
        Main function of the server.
        In this function batches from the cpp agents are received.
        They are processed, sent to the session manager for logging and
        sent to one of the workers to perform a parameter update.
        Returns:

        """
        self.session_manager.start()
        context = zmq.Context()

        # the socket receiving the data from agents.
        frontend_sock = context.socket(zmq.PULL)
        frontend_sock.bind(self.server_config['ADDRESSES']['AGENT_INTERFACE'])

        # the socket sending the tasks data to the worker.
        backend_sock = context.socket(zmq.DEALER)
        backend_sock.bind(self.server_config['ADDRESSES']['WORKER_INTERFACE_DEALER'])

        # the socket for sending basic signal like 'shut-down'.
        signal_socket = context.socket(zmq.PUB)
        signal_socket.bind(self.server_config['ADDRESSES']['INTERFACE_PUB'])

        # for handling multiple sockets.
        msg_in = zmq.Poller()
        msg_in.register(frontend_sock)
        msg_in.register(backend_sock)

        while self.session_manager.episode_count < self.server_config['A3C']['MAX_NUM_STEPS']:

            socks = dict(msg_in.poll())

            # message from the agent
            if frontend_sock in socks and socks[frontend_sock] == zmq.POLLIN:
                msg = frontend_sock.recv()

            # message from a worker.
            if backend_sock in socks and socks[backend_sock] == zmq.POLLIN:
                msg = backend_sock.recv()

        print "Shut-Down"
        signal_socket.send('KILL')
        self.session_manager.should_stop = True
        # unclean
        time.sleep(3)
        signal_socket.close()
        frontend_sock.close()
        backend_sock.close()




