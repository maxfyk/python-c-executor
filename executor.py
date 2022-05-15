import os
import json

from connectors.subconnector import SubConnector


class Executor:
    def __init__(self, connector, commands_file_path, **kwargs):
        '''c_file_path: required for subprocess connector'''
        self._connector = self._establish_connection(connector, kwargs)
        self._commands_list = self._read_commands_file(commands_file_path)

    def run(self):
        '''Execute the commands list'''
        for index, command in enumerate(self._commands_list):  # loop tough all commands from json file
            print(f'Executing command {index}: {str(command["input"])[:20]}')
            self._connector.execute_cmd(command['input'])

    def verify_results(self):
        '''Compare the outputs of the commands list with the expected results'''
        results = self._connector.get_results()
        predicted_results = [command['output'] for command in self._commands_list if
                             command['output']]  # only leave commands that have an expected result

        if len(results) != len(predicted_results):
            raise ValueError('Number of predicted results and real results does not match')

        for predicted_result, real_result in zip(predicted_results, results):  # loop tough all results and compare them
            pred_res, pred_type = predicted_result['value'], predicted_result['type']
            if pred_type == 'float':  # convert to float if needed
                pred_res, real_result = float(pred_res), float(real_result)

            assert pred_res == real_result, f'Output is not as expected; {pred_res} == {real_result}'
            print(f'Output is as expected; {pred_res} == {real_result}')
        self._connector.disconnect()  # close connection
        print(f'Done!')

    @staticmethod
    def _read_commands_file(commands_file_path):
        '''Validate and read the commands file
        :return: list of commands'''
        if os.path.isfile(commands_file_path):  # check if file exists
            try:
                with open(commands_file_path, 'r', encoding='utf8') as f:  # read json and return list of commands
                    commands_list = json.load(f).get('commands', [])
                    if commands_list:
                        return commands_list

                raise ValueError('Commands list is empty')
            except ValueError as e:
                raise ValueError('Input file is not a valid JSON file')

        raise ValueError('File does not exist')

    @staticmethod
    def _establish_connection(connector, kwargs):
        '''Establish connection depending on the selected connector'''
        if connector == 'subprocess':
            return SubConnector(kwargs['c_file_path'])
        # elif connector == 'ssh':
        #     return SSHConnector()
        else:
            raise ValueError('Unknown connector')
