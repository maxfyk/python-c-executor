from executor import Executor

if __name__ == '__main__':
    executor = Executor(connector='subprocess', commands_file_path='test_data.json',
                        c_file_path='c_code/calculator.exe')
    executor.run()  # run all the commands
