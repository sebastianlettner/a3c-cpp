# Tests for the worker.

from worker import Worker
import unittest
import zmq


class TestWorker(unittest.TestCase):

    """ Testing the worker."""

    def test_array_serialization(self):

        """
        Test the serialization of numpy arrays
        Returns:

        """
        worker = Worker()
