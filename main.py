from executor import Executor

if __name__ == '__main__':
    executor = Executor(connector='subprocess', tests_file_path='tests.json',
                        c_file_path='c_code/calculator.exe')
    executor.run()  # run all the commands