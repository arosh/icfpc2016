import requests
import time
import subprocess
import tinydb
from tinydb import where
import util
import sys
import argparse

last_access = None

def get_problem_id_list(solver_name):
    # 未提出でsolution_sizeが最も小さいやつを解く
    db = tinydb.TinyDB('icfpc.json')
    submission_table = db.table('submission')
    problem_table = db.table('problem')
    problems = problem_table.all()

    problems.sort(key=lambda x: x['solution_size'], reverse=True)
    for problem in problems:
        query = (where('problem_id') == problem['problem_id']) & (where('solver_name') == solver_name)
        if not submission_table.search(query):
            yield problem['problem_id']


def create_solution(problem_id):
    db = tinydb.TinyDB('icfpc.json')
    problem_table = db.table('problem')
    problem = problem_table.search(where('problem_id') == problem_id)[0]
    print('create_solution: problem_id =', problem_id)
    try:
        solution = subprocess.check_output(['./solver'], input=problem['content'].encode('UTF-8'))
        return solution.decode('UTF-8')
    except subprocess.CalledProcessError as e:
        print(e, file=sys.stderr)
        return None


def submit_solution(problem_id, solution, solver_name):
    global last_access
    while last_access is not None and time.time() < last_access + 1:
        time.sleep(0.01)

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
        db = tinydb.TinyDB('icfpc.json')
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

    for problem_id in get_problem_id_list(solver_name):
        print('next problem_id =', problem_id)
        solution = create_solution(problem_id)
        if solution is None:
            continue
        submit_solution(problem_id, solution, solver_name)

if __name__ == '__main__':
    main()
