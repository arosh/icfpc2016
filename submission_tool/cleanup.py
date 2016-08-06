import tinydb

if __name__ == '__main__':
    db = tinydb.TinyDB('icfpc.json')

    # 最新でないcontestを消す
    contest_table = db.table('submission')
    contests = contest_table.all()
    max_snapshot = max(x['snapshot_time'] for x in contests)
    contest_table.remove(where('snapshot_time') != max_snapshot)

    # 最大でないresemblanceを消す
    problem_table = db.table('problem')
    problems = problem_table.all()
    for problem in problems:
        pass
