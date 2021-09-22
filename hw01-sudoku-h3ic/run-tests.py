import argparse
import colorama
import subprocess
import termcolor
import timeit
import os

colorama.init()


def is_windows():
    return os.name == 'nt'


def get_os_binary(name):
    if is_windows():
        return name + '.exe'
    return name


ONE_TEST_TIMEOUT_S = 0.5


def check_file_exists(path):
    return os.path.exists(path) and os.path.isfile(path)


parser = argparse.ArgumentParser(description='Run tests')
parser.add_argument('--type', '-t', default='public', choices=['public', 'private'], dest='folder')
parser.add_argument('--config', '-c', default='Debug', choices=['Debug', 'Release', 'RelWithDebInfo'], dest='config')

args = parser.parse_args()

termcolor.cprint('Run ' + args.folder + ' tests for ' + args.config + ' config:', 'magenta', attrs=['bold'])

if is_windows() and args.config == 'RelWithDebInfo':
    termcolor.cprint('Cannot run valgrind under Windows -- setup Linux. ', 'red', end='')
    termcolor.cprint('BE BRAVE!!!', 'red', attrs=['bold', 'blink'])
    exit(0)

folder = args.folder + '-tests'
files_num = len(os.listdir(folder))

assert files_num % 2 == 0, "Inconsistent tests"

for i in range(1, files_num // 2 + 1):
    test_name = folder + '/' + str(i)
    if not check_file_exists(test_name + '.in'):
        termcolor.cprint('Cannot find \'' + test_name + '.in\'', 'red', attrs=['bold'])
        exit(1)
    if not check_file_exists(test_name + '.ans'):
        termcolor.cprint('Cannot find \'' + test_name + '.ans\'', 'red', attrs=['bold'])
        exit(1)

    termcolor.cprint('Run ' + args.folder + ' test #' + str(i) + '...  ', 'green', end='')
    start_time = timeit.default_timer()
    try:
        timeout_s = 1000.0
        if args.config == 'Release':
            timeout = ONE_TEST_TIMEOUT_S
        binary = [get_os_binary('cmake-build-' + args.config + '/checker')]
        if args.config == 'RelWithDebInfo':
            binary = ['valgrind', '--tool=memcheck', '--gen-suppressions=all', '--leak-check=full', '--leak-resolution=med', '--track-origins=yes', '--vgdb=no', '--error-exitcode=1', 'cmake-build-RelWithDebInfo/checker']

        result = subprocess.run(binary + [test_name + '.in', test_name + '.ans'],
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=os.getcwd(), timeout=timeout_s,
                                check=True, text=True, universal_newlines=True)
        duration = (timeit.default_timer() - start_time) * 1000.0
        termcolor.cprint('OK', 'green', attrs=['bold', 'blink'], end='')
        termcolor.cprint('  (' + str(duration) + 'ms)', 'white', attrs=['bold'])
        print('')

    except subprocess.TimeoutExpired:
        termcolor.cprint('FAILED!', 'red', attrs=['bold', 'blink'], end='')
        termcolor.cprint('  Timeout ' + str(ONE_TEST_TIMEOUT_S) + 's on test #' + str(i), 'white', attrs=['bold'])
        exit(1)
    except subprocess.CalledProcessError as e:
        termcolor.cprint('FAILED!', 'red', attrs=['bold', 'blink'], end='')
        termcolor.cprint('  Test #' + str(i) + ' returns non-zero exit code: ' + str(e.returncode), 'white', attrs=['bold'])
        termcolor.cprint(e.stdout, 'white')
        exit(1)
termcolor.cprint('ALL TESTS ARE PASSED!!!', 'green', attrs=['bold', 'blink'])
print('')
print('')
