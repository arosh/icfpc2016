import sys

import tinydb
from tinydb import where
from tinydb.middlewares import CachingMiddleware
from tinydb.storages import JSONStorage

import util

if __name__ == '__main__':
    with tinydb.TinyDB(util.FILENAME, storage=CachingMiddleware(JSONStorage)) as db:
        # 最新でないcontestを消す
        contest_table = db.table('contest')
        contests = contest_table.all()
        max_snapshot = max(x['snapshot_time'] for x in contests)
        contest_table.remove(where('snapshot_time') != max_snapshot)

        # 最大でないresemblanceを消す (contest_table)
        latest_contest = contest_table.all()[0]
        problems = latest_contest['problems']
        for problem in problems:
            ranking = problem['ranking']
            if ranking:
                top = max(ranking, key=lambda x: (x['resemblance'], -x['solution_size']))
                ranking = [top]
                problem['ranking'] = ranking
        latest_contest['problems'] = problems
        contest_table.update(latest_contest, eids=[latest_contest.eid])

        # 大きすぎる問題を消す
        latest_contest = contest_table.all()[0]
        problems = latest_contest['problems']
        nx = []
        for problem in problems:
            if problem['problem_size'] <= 500:
                nx.append(problem)
        contest_table.update({'problems': nx}, eids=[latest_contest.eid])

        problem_ids = set(x['problem_id'] for x in nx)
        problem_table = db.table('problem')
        for problem in problem_table.all():
            if problem['problem_id'] not in problem_ids:
                problem_table.remove(eids=[problem.eid])
