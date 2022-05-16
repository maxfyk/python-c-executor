import os
import json

from connectors.subconnector import SubConnector


class Executor:
    def __init__(self, connector, tests_file_path, **kwargs):
        '''c_file_path: required for subprocess connector type'''
        self._connector = self._establish_connection(connector, kwargs)
        self._tests_list = self._read_tests_file(tests_file_path)

    def run(self, verify_results=True):
        '''Execute the tests list'''
        for test in self._tests_list:
            print(f'Executing test "{test["name"]}": {len(test["commands"])} commands')

            [self._connector.execute_cmd(command['input']) for index, command in
             enumerate(test['commands'])]  # loop through all cmds in current test + execute them

            if verify_results:  # compare the returned results with the expected results if needed
                self.verify_results(test)
            self._connector.restart()  # restart program
            print('__________________________________________________')
        self._connector.disconnect()  # close connection
        print(f'Done!')

    def verify_results(self, test):
        '''Compare the outputs of the subprocess with the expected test results'''
        results = self._connector.get_results()  # get results returned by the subprocess
        test_name, commands = test['name'], test['commands']
        predicted_results = [command['output'] for command in commands if
                             command['output']]  # only leave tests that have an expected result (output)

        if len(results) != len(predicted_results):
            print(f'Returned results: {results}')
            print(f'Expected results: {predicted_results}')
            raise ValueError('Number of predicted results and returned results does not match')

        for predicted_result, real_result in zip(predicted_results, results):  # loop trough all results + compare them
            pred_res, pred_type = predicted_result['value'], predicted_result['type']
            if pred_type == 'float':  # convert to float if needed
                pred_res, real_result = float(pred_res), float(real_result)

            assert pred_res == real_result, f'Output is not as expected; {pred_res} == {real_result}'
            print(f'Output is as expected; {pred_res} == {real_result}')
        print(f'Test "{test_name}" passed!')

    @staticmethod
    def _read_tests_file(tests_file_path):
        '''Validate and read the tests file
        :return: list of tests'''
        if os.path.isfile(tests_file_path):  # check if file exists
            try:
                with open(tests_file_path, 'r', encoding='utf8') as f:  # read json and return list of tests
                    tests_list = json.load(f).get('tests', [])
                    if tests_list:
                        return tests_list

                raise ValueError('tests list is empty')
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
