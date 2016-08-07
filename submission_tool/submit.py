import argparse
import subprocess
import sys
import time

import requests
import tinydb
from tinydb import where
from tqdm import tqdm

import util

last_access = None

def get_problem_id_list(solver_name, db):
    # 未提出でsolution_sizeが小さい順
    submission_table = db.table('submission')
    contest_table = db.table('contest')

    latest_contest = max(contest_table.all(), key=lambda x: x['snapshot_time'])
    problems = latest_contest['problems']
    problems.sort(key=lambda x: x['solution_size'], reverse=True)

    submission_set = set((x['problem_id'], x['solver_name']) for x in submission_table.all())
    for problem in problems:
        if (problem['problem_id'], solver_name) not in submission_set:
            yield problem['problem_id']


def create_solution(problem_id, db):
    problem_table = db.table('problem')
    problem = problem_table.get(where('problem_id') == problem_id)
    print('create_solution: problem_id =', problem_id)
    if problem is None:
        print('cannot find content (problem_id = {})'.format(problem_id))
        return None
    try:
        content = problem['content'].encode('UTF-8')
        solution = subprocess.check_output(['./solver'], input=content)
        return solution.decode('UTF-8')
    except subprocess.CalledProcessError as e:
        print(e, file=sys.stderr)
        return None


def submit_solution(problem_id, solution, solver_name, db):
    global last_access
    if last_access is not None:
        time.sleep(max(0, last_access + util.SLEEP - time.time()))

    headers = {
        'Expect': '',
        'X-API-Key': util.API_KEY,
    }
    payload = {
        'problem_id': problem_id,
        'solution_spec': solution,
    }
    r = requests.post('http://2016sv.icfpcontest.org/api/solution/submit', headers=headers, data=payload)
    last_access = time.time()
    rj = r.json()
    rj['solver_name'] = solver_name

    if rj['ok']:
        submission_table = db.table('submission')
        submission_table.insert(rj)
        print('resemblance =', rj['resemblance'])
    else:
        print(rj)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('solver_name')
    args = parser.parse_args()
    solver_name = args.solver_name

    with tinydb.TinyDB(util.FILENAME) as db:
        for problem_id in tqdm(list(get_problem_id_list(solver_name, db))):
            print('next problem_id =', problem_id)
            solution = create_solution(problem_id, db)
            if solution is None:
                continue
            submit_solution(problem_id, solution, solver_name, db)

if __name__ == '__main__':
    main()
