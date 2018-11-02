import tensorflow as tf
import numpy as np
import os

ROOT_PATH = os.path.dirname(os.path.abspath(__file__))

with tf.Session() as sess:
    a = tf.Variable(0.0, dtype=tf.float32, name="var1")
    init_all_vars = tf.global_variables_initializer()

    tf.train.write_graph(
        sess.graph_def,
        ROOT_PATH,
        'graph.pb',
        as_text=False
    )