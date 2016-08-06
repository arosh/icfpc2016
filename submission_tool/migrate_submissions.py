import tinydb

if __name__ == '__main__':
    db = tinydb.TinyDB('icfpc.json')
    submission_table = db.table('submission')
    submission_table.update({'solver_name': 'yazaten_v1'}, tinydb.Query().problem_id != '')
