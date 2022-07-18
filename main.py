from executor import Executor

if __name__ == '__main__':
    executor = Executor(connector='boardconnector', tests_file_path='tests.json',
                        c_file_path='bigint', tty='/dev/ttyUSB4')
    # executor.run()  # run all the commands
    executor.run_random(N=50)  # run random tests
