""" Helper function that generates the *.ini files at a desired location. """

import os
import shutil

ROOT_PATH = os.path.dirname(os.path.abspath(__file__))  # the python folder


CONFIG_FILE_PATH = [
    ROOT_PATH + '/server/server.ini',
    ROOT_PATH + '/server/session_manager.ini',
    ROOT_PATH + '/server/cluster.json'
]


def gen_ini_files(path_to_root):

    """
    This function create a dictionary at the specified location.
    All init files will be put into this directory. Don't change the name of those files.
    When creating the server use this directory to tell it where the *.ini files are.
    Args:
        path_to_root(str): At the location this path points to a directory will be created containing all
        the init files.

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
