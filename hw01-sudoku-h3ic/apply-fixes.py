import argparse
import colorama
import os
import subprocess
import termcolor


colorama.init()


def get_cwd():
    return os.path.dirname(os.path.abspath(__file__))


def log(msg, color='yellow'):
    termcolor.cprint(msg + '... ', color, attrs=['bold'], end='', flush=True)


def ok(msg='', color='green'):
    termcolor.cprint('OK', color, attrs=['bold', 'blink'], end='')
    termcolor.cprint(' ' + msg, 'magenta', attrs=['bold'], flush=True)


def print_error_and_exit(msg):
    termcolor.cprint('ERROR!!!', 'red', attrs=['bold', 'blink'])
    termcolor.cprint(msg, 'white', attrs=['bold'], flush=True)
    exit(1)


def call(args, on_error='', cwd=get_cwd()):
    result = subprocess.run(args, capture_output=True, cwd=cwd, text=True, universal_newlines=True)
    if result.returncode != 0:
        print_error_and_exit(on_error + ' failed with return code: ' + str(result.returncode) + '\n\t' + ' '.join(args) +
                             '\n\n\n' + str(result.stdout) + '\n\n\n' + str(result.stderr))
    return str(result.stdout)


def get_git_status():
    return call(['git', 'status', '-s'], 'Check uncommitted changes').rstrip()


parser = argparse.ArgumentParser(description='Apply fixes')
parser.add_argument('-n', default=10000, type=int, dest='fixes_count', help='Number of fixes to apply')
parser.add_argument('--on-github', action='store_true', dest='on_github', help='Use to vary behavior on github')
cmd_args = parser.parse_args()
fixes_count = cmd_args.fixes_count
on_github = cmd_args.on_github


if not on_github:
    log('Check uncommitted changes')
    output = get_git_status()
    if len(output) != 0:
        print_error_and_exit('You have uncommitted changes on repository:\n' + output +
                             '\n\nTo continue use commands:\n\n\tgit add .\n\tgit commit -m \"<commit message>\"' +
                             '\n\nAfter that repeat script')
    ok()


log('Find out current repository name')
output = call(['git', 'config', '--get', 'remote.origin.url'], 'Find out remote url').rstrip()
user_repo_name = call(['basename', '-s', '.git', output], 'Parse repo name').rstrip()
task_id = user_repo_name.split('-')[0]
ok(user_repo_name + ' with id ' + task_id)


fixes_dir = get_cwd() + '/.fixes-internal'
fixes_repo_dir = fixes_dir + '/fixes'

if not on_github:
    log('Remove old fixes')
    call(['rm', '-rf', fixes_dir], 'Remove old fixes')
    ok()

    os.makedirs(fixes_dir, exist_ok=False)
    assert not os.path.exists(fixes_repo_dir)
    log('Clone fixes repository')
    call(['git', 'clone', 'git@github.com:ITMO-CPP-IB/fixes.git'], 'Clone fixes repository', fixes_dir)
    ok()

log('Find out initial repository name')
assert os.path.exists(fixes_repo_dir)
fixes = os.listdir(fixes_repo_dir)
cur_fix_dir = None
for fix_name in fixes:
    if os.path.isdir(fixes_repo_dir + '/' + fix_name) and fix_name.startswith(task_id):
        cur_fix_dir = fixes_repo_dir + '/' + fix_name
        break

if cur_fix_dir is None:
    ok()
    termcolor.cprint('No fixes for repo ' + user_repo_name + '. Stop.', 'magenta', attrs=['bold'])
    exit(0)

assert os.path.exists(cur_fix_dir)
repo_name = os.path.basename(cur_fix_dir)
ok(repo_name)


log('Apply fix ' + repo_name)
res = subprocess.run(['cp', '-vrf', cur_fix_dir + '/.', get_cwd() + '/'], capture_output=True, cwd=get_cwd(), text=True, universal_newlines=True)
if res.returncode != 0:
    print('')
    termcolor.cprint('Cannot apply fix for repo ' + repo_name + '!!!', 'red', attrs=['bold', 'blink'])
    termcolor.cprint('Text to @burakov28 about this problem (return code: ' + str(res.returncode) + ')', 'magenta', attrs=['bold'])
    termcolor.cprint(res.stderr, 'red', attrs=['bold'])
    termcolor.cprint('Discard changes for fix ' + repo_name + ':', 'magenta', attrs=['bold'])

    if on_github:
        exit(1)

    if len(get_git_status()) == 0:
        termcolor.cprint('\tNothing to discard', 'magenta', attrs=['bold'])
        exit(1)

    log('\tStash fix changes', 'magenta')
    call(['git', 'stash', 'save', '--keep-index', '--include-untracked'], 'Stash fix changes')
    ok('', 'magenta')

    log('\tDrop stashed changes', 'magenta')
    call(['git', 'stash', 'drop'], 'Drop stashed changes')
    ok('', 'magenta')
    exit(1)
ok()
output = str(res.stdout)
changed_files = output.rstrip().splitlines()
for line in changed_files:
    dst = line.split(' -> ')[1][1:-1]
    assert dst.startswith(get_cwd() + '/./')
    dst = dst[len(get_cwd() + '/./'):]
    path = get_cwd() + '/' + dst
    assert os.path.exists(path)
    if os.path.isdir(path):
        continue
    termcolor.cprint('\tRewrite file \'' + dst + '\'', 'cyan', attrs=['bold'])

if on_github:
    exit(0)


if len(get_git_status()) == 0:
    termcolor.cprint('No changes in repo after fixes application', 'magenta', attrs=['bold'])
    exit(0)

log('Add fixes changes')
call(['git', 'add', '.'], 'Add fix changes')
ok()
log('Commit fixes')
call(['git', 'commit', '-m', 'Apply fixes'])
ok()

termcolor.cprint('Successfully applied fix ' + repo_name, 'magenta', attrs=['bold'])
