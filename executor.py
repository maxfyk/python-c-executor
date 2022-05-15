import os
import json


class Executor:
    def __init__(self, connector, commands_file_path):
        self._connector = self._establish_connection(connector)
        self.commands_list = self._read_commands_file(commands_file_path)

    def run(self):
        for index, command in enumerate(self.commands_list):
            self._connector.run(command['input'])
            if command['output']:
                out = self._connector.get_output()
                assert out == command['output'], f'Command {index}: Output is not as expected'

    @staticmethod
    def _read_commands_file(commands_file_path):
        '''Validates and reads the commands file
        :returns: list of commands'''
        if os.path.isfile(commands_file_path):
            try:
                commands_list = json.loads(commands_file_path).get('commands', [])
                if commands_list:
                    return commands_list

                raise ValueError('Commands list is empty')
            except ValueError as e:
                raise ValueError('Input file is not a valid JSON file')

        raise ValueError('File does not exist')

    @staticmethod
    def _establish_connection(connector):
        '''Establishes connection depending on the selected connector'''
        if connector == 'subprocess':
            return SubConnector()
        # elif connector == 'ssh':
            # return SSHConnector()
        else:
            raise ValueError('Unknown connector')
