import requests
import time
import subprocess
import tinydb
from tinydb import where
import util
import sys

last_access = None

def get_problem_id_list():
    # 未提出でsolution_sizeが最も小さいやつを解く
    db = tinydb.TinyDB('icfpc.json')
    submission_table = db.table('submission')
    problem_table = db.table('problem')
    problems = problem_table.all()

    problems.sort(key=lambda x: x['solution_size'], reverse=True)
    # {"ok": false} なsubmissionをした場合はproblem_idが付与されないので
    submissions = set(map(lambda x: x['problem_id'], submission_table.all()))
    for problem in problems:
        if problem['problem_id'] not in submissions:
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


def submit_solution(problem_id, solution):
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

    if rj['ok']:
        db = tinydb.TinyDB('icfpc.json')
        submission_table = db.table('submission')
        submission_table.insert(rj)
        print('resemblance =', rj['resemblance'])
    else:
        print(rj)

def main():
    for problem_id in get_problem_id_list():
        print('next problem_id =', problem_id)
        solution = create_solution(problem_id)
        if solution is None:
            continue
        submit_solution(problem_id, solution)

if __name__ == '__main__':
    main()
