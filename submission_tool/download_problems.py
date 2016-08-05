import dataset
import requests
import util
import time
import tinydb
from tinydb import where
import json

last_access = None


def download_snapshot():
    global last_access
    while last_access is not None and time.time() < last_access + 1:
        time.sleep(0.01)

    headers = {
        'Expect': '',
        'X-API-Key': util.API_KEY,
    }
    r = requests.get(
        'http://2016sv.icfpcontest.org/api/snapshot/list', headers=headers)
    last_access = time.time()
    print('download_snapshot:', last_access)
    return r.json()


def download_blob(spec_hash):
    global last_access
    while last_access is not None and time.time() < last_access + 1:
        time.sleep(0.01)

    headers = {
        'Expect': '',
        'X-API-Key': util.API_KEY,
    }
    r = requests.get('http://2016sv.icfpcontest.org/api/blob/' +
                     spec_hash, headers=headers)
    last_access = time.time()
    print('download_blob:', last_access)
    return r.text


def save_contest(contest_status):
    db = tinydb.TinyDB('icfpc.json')
    contest_table = db.table('contest')
    contest_table.insert(contest_status)


def download_topk_problems(contest_status, k=1000):
    problems = contest_status['problems']
    # solution_sizeが小さい順にダウンロード
    problems.sort(key=lambda x: x['solution_size'], reverse=True)

    db = tinydb.TinyDB('icfpc.json')
    problem_table = db.table('problem')

    hash_values = {}
    for problem in problems:
        problem_spec_hash = problem['problem_spec_hash']
        if not problem_table.search(where('problem_spec_hash') == problem_spec_hash):
            hash_values[problem_spec_hash] = problem
        if len(hash_values) >= k:
            break

    for problem_spec_hash, problem in hash_values.items():
        print('problem_spec_hash =', problem_spec_hash)
        content = download_blob(problem_spec_hash)
        problem_table.insert(
            {**problem, 'content': content})


def main():
    # snapshot APIからダウンロード
    snapshot = download_snapshot()
    if not snapshot['ok']:
        raise Exception(snapshot)
    # タイムスタンプが最新のハッシュ値を得る
    time_and_hash = [(x['snapshot_time'], x['snapshot_hash'])
                     for x in snapshot['snapshots']]
    time_and_hash.sort(key=lambda x: x[0])
    latest_snapshot_hash = time_and_hash[-1][1]
    print('latest_snapshot_hash =', latest_snapshot_hash)

    # ハッシュ値をもとにコンテスト情報をダウンロード
    contest_status = json.loads(download_blob(latest_snapshot_hash))
    save_contest(contest_status)
    download_topk_problems(contest_status)

if __name__ == '__main__':
    main()
