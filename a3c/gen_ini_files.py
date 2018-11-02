""" Helper function that generates the *.ini files at a desired location. """

import os
import shutil
import argparse

ROOT_PATH = os.path.dirname(os.path.abspath(__file__))  

CONFIG_FILE_PATH = [
    ROOT_PATH + '/python/server/server.ini',
    ROOT_PATH + '/python/server/session_manager.ini',
    ROOT_PATH + '/python/server/cluster.json',
    ROOT_PATH + '/cc/predictor/predictor.ini',
    ROOT_PATH + '/cc/broker/broker.ini'
]


def gen_ini_files(path_to_root):

    """
    This function create a dictionary at the specified location.
    All init files will be put into this directory. Don't change the name of those files
    nor the parameter names.
    Args:
        path_to_root(str): At the location this path points to a directory will be created containing all the ini files.

    Returns:
        path(str): Path to the directory
    """
    # check if slash is at the end of the path
    if path_to_root[-1] == '/':
        # remove trailing slash
        path_to_root = path_to_root[:-1]

    # if the dir already exists throw exception
    if os.path.isdir(path_to_root + '/configurations'):
        raise Exception('Directory <{}> already exists!'.format(path_to_root + '/configurations'))
    else:
        # create the dir
        os.mkdir(path_to_root + '/configurations')

    for file_path in CONFIG_FILE_PATH:
        shutil.copy2(src=file_path, dst=path_to_root + '/configurations')

    return path_to_root + '/configurations'


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Path')
    parser.add_argument("-path", help="Path to the directory where the files should be created")
    args = parser.parse_args()
    path = args.path
    gen_ini_files(path)