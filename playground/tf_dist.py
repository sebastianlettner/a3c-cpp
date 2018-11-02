import tensorflow as tf 
import time

cluster = tf.train.ClusterSpec({
    "worker": [
        "localhost:5555",
        "localhost:5554",
    ]
})

if __name__ == '__main__':

    with tf.device("/job:worker/task:0"):
        var = tf.Variable(0.0, name='var1')

    server = tf.train.Server(cluster,
                             job_name="worker",
                             task_index=0)
    sess = tf.Session(target=server.target)
        
    print("Parameter server: initializing variables...")
    sess.run(tf.global_variables_initializer())
    print("Parameter server: variables initialized")
    
    for i in range(20):
        op = tf.assign(var, i+0.0)
        sess.run(op)
        time.sleep(1.0)
        print sess.run(var)


