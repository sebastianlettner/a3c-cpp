""" Class, that manages the neural network. """

import os
import sys
import time
import datetime
import tensorflow as tf
from Queue import Queue as qQueue  # rename to avoid clash with mp
from multiprocessing import Process, Queue, Value
import configparser


class SessionManager(Process):

    """

    """

    def __init__(self, builder, config_file_dir, cluster_spec):
        """
        Args:
            builder(GraphBuilder):
            config_file_dir(str): Directory to log the model variables as well as the tensorboard values.
            cluster_spec(dict):
        """
        super(SessionManager, self).__init__()
        self.start_time = 0  # will be set when the process starts.

        self.path_to_config_file = config_file_dir + '/session_manager.ini'
        self.session_config = configparser.ConfigParser()
        self.session_config.read(self.path_to_config_file)

        self.use_tensorboard = self.session_config['TENSORFLOW'].getboolean('USE_TENSORBOARD')
        self.load_model = self.session_config['LOADMODEL'].getboolean('LOAD_CHECKPOINT')
        self.save_model = self.session_config['SAVEMODEL'].getboolean('SAVE_MODEL')

        self.cluster_spec = cluster_spec
        self.device = "/job:session_management/task:0"

        self.log_dir = self.prepare_log_dir()

        # set everything for tensorboard and save/restore of variables if specified in the session_manager.ini file
        self.file_path = self.log_dir + '/results.txt'
        self.graph_builder = builder

        # for communication with the server
        self.episode_log_q = Queue(maxsize=self.session_config['STATS']['MAX_QUEUE_SIZE'])

        self.episode_count = Value('i', 0)  # counts the episodes.
        self.update_count = Value('i', 0)  # counts the updates.
        self.worker_count = Value('i', 0)  # count number of workers
        self.predictor_count = Value('i', 0)  # count the number of predictors
        self.agent_count = Value('i', 0)  # count the number of agents
        self.should_stop = Value('i', 0)

    def build_tf_graph(self):
        """
        Set up the tensorboard.
        Returns:

        """

        with tf.device(self.device):
            return self.graph_builder.build_tf_graph(self.graph_builder.define_graph())

    def get_tensorboard(self, compute_graph):
        """

        Args:
            compute_graph:

        Returns:

        """
        with tf.device(self.device):
            with compute_graph.tf_graph.as_default():
                summaries = tf.get_collection(tf.GraphKeys.SUMMARIES)
                summary_op = tf.summary.merge(summaries)
                log_writer = tf.summary.FileWriter(
                    logdir=self.log_dir + '/tensorboard',
                    graph=compute_graph.session.graph
                )
        return summary_op, log_writer

    def get_saver(self):
        """

        Returns:

        """
        with tf.device(self.device):
            variables = tf.global_variables()
            saver = tf.train.Saver({var.name: var for var in variables}, max_to_keep=0)
        return saver

    def prepare_log_dir(self):
        """
        Crete two folders in the log dir
        Returns:

        """
        log_dir = self.session_config['STATS']['PATH_TO_DIR']

        # create the directories for tensorboard and variables
        if self.save_model and not os.path.isdir(log_dir + '/checkpoint'):
            os.mkdir(log_dir + '/checkpoint')
        if self.use_tensorboard and not os.path.isdir(log_dir + '/tensorboard'):
            os.mkdir(log_dir + '/tensorboard')

        return log_dir

    @staticmethod
    def get_global_step(compute_graph):
        """
        The global_step key must be provided in the output dictionary of the model.

        Args:
            compute_graph(ComputeGraph): The compute graph from the tf-graph-tool.
        Returns:
            global_step(int): Number of training steps performed.
        """
        return compute_graph.session.run(
            compute_graph.outputs['global_step']
        )

    @staticmethod
    def log(compute_graph, summary_op, log_writer, inputs):
        """
        Log the current values to the tensorboard.
        Args:
            compute_graph(ComputeGraph): The compute graph from the tf-graph-tool
            summary_op(tf.operation): All tensorboard ops.
            log_writer(tf.summary.FileWriter): Writes the summary to files.
            inputs(dict): Input dict.

        Returns:

        """
        # prepare feed dict
        feed_dict = compute_graph.get_feed_dict(inputs=inputs)

        # run the tensorboard (summary op)
        step, summary = compute_graph.session.run(
            [compute_graph.outputs['global_step'], summary_op],
            feed_dict=feed_dict
        )

        # write
        log_writer.add_summary(summary=summary, global_step=step)

    def _checkpoint_filename(self, episode):
        """
        Returns the path to the checkpoint file with the specified episode.

        Args:
            episode(int): The episode number.

        Returns:
            Path for checkpoint file.
        """
        return self.log_dir + "/checkpoint/episode_{}".format(episode)

    def save(self, compute_graph, saver, episode):
        """
        Save the model parameters.
        Args:
            episode(int): The current episode.
            compute_graph(ComputeGraph): The compute graph from the tf-graph-tool
            saver(tf.train.Saver): Interface for saving and loading models.

        Returns:

        """
        saver.save(compute_graph.session, self._checkpoint_filename(episode))

    def load(self, compute_graph, saver):
        """
        Load a model.
        Returns:
            episode_count(int): The episode after loading.
        """
        filename = tf.train.latest_checkpoint(os.path.dirname(self._checkpoint_filename(episode=0)))
        if self.session_config['LOADMODEL'].getint('LOAD_EPISODE') > 0:
            filename = self._checkpoint_filename(self.session_config['LOADMODEL'].getint('LOAD_EPISODE'))
        saver.restore(compute_graph.session, filename)
        # TODO Change retrieval of epsiode. Maybe additional tf variable.
        self.episode_count.value = int(filename.split('_')[1])

    def ups(self):
        """
        Gives an average of the training episodes per sec.

        Returns:

        """

        return self.update_count.value / (time.time() - self.start_time)

    @staticmethod
    def t_print(msg):
        """
        Like 'print' but won't get new lines confused with multiple processes.
        Args:
            msg(str): The message you want to print.

        Returns:

        """
        sys.stdout.write(msg + '\n')
        sys.stdout.flush()

    def run(self):

        """
        Main function of the process. Will be called when the process is started.

        Returns:

        """
        summary_op = None
        saver = None
        log_writer = None

        server = tf.train.Server(
            self.cluster_spec,
            job_name="session_management",
            task_index=0
        )
        compute_graph = self.build_tf_graph()
        compute_graph.create_session(target=server.target)
        print "Session Manager: Cluster ready. Initializing Variables"
        compute_graph.session.run(tf.global_variables_initializer())

        if self.use_tensorboard:
            summary_op, log_writer = self.get_tensorboard(compute_graph)
        if self.load_model or self.save_model:
            saver = self.get_saver()
        if self.load_model:
            self.load(compute_graph, saver)

        # 'a' Open for appending at the end of the file without truncating it. Creates a new file if it does not exist.
        with open(self.file_path, 'a') as results_logger:

            rolling_reward = 0
            results_q = qQueue(maxsize=self.session_config['STATS'].getint('ROLLING_MEAN_WINDOW'))

            self.start_time = time.time()

            while not self.should_stop:

                inputs = self.episode_log_q.get()
                # TODO: Improve. Check with cpp
                episode_time = inputs['episode_time']
                reward = inputs['episode_reward']
                episode_length = inputs['episode_length']
                del inputs['episode_time']
                del inputs['episode_reward']
                del inputs['episode_length']

                results_logger.write('Time: {}\tReward: {}\tLength: {}\n'.format(
                        episode_time.strftime("%Y-%m-%d %H:%M:%S"),
                        reward,
                        episode_length
                    )
                )
                results_logger.flush()

                # increase the episode count (cumulative over all agents)
                self.episode_count.value += 1

                rolling_reward += reward

                if results_q.full():
                    old_episode_time, old_reward, old_length = results_q.get()
                    rolling_reward -= old_reward

                results_q.put((episode_time, reward, episode_length))

                if self.save_model and \
                        self.episode_count.value % self.session_config['SAVEMODEL'].getint('SAVE_MODEL_T') == 0:
                    self.save(
                        compute_graph=compute_graph,
                        saver=saver,
                        episode=self.episode_count.value
                    )

                if self.use_tensorboard and \
                        self.episode_count.value % self.session_config['TENSORFLOW'].getint('TENSORBOARD_T'):
                    SessionManager.log(
                        compute_graph=compute_graph,
                        summary_op=summary_op,
                        log_writer=log_writer,
                        inputs=inputs
                    )

                if self.session_config['STATS']['PRINT_STATS'] and\
                        self.episode_count.value % self.session_config['STATS']['PRINT_STATS_T'] == 0:
                    msg = "[Time: {:8d}]\t" \
                          "[Steps: {:8d}]\t" \
                          "[Episode: {:8d} Score: {:.4f}]\t" \
                          "[TPS: {:.2f}]\t" \
                          "[NT: {:d} NP: {:d} NA: {:d}]".format(
                              int(time.time() - self.start_time),
                              episode_length,
                              self.episode_count.value, reward,
                              rolling_reward / results_q.qsize(),
                              self.ups(),
                              self.update_count.value, self.predictor_count.value, self.agent_count.value
                          )
                    SessionManager.t_print(msg=msg)

            results_logger.write('Stopped at {}'.format(datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")))
            results_logger.flush()
