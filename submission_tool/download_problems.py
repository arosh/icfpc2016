import json
import time

import requests
import tinydb
from tinydb import where
from tinydb.middlewares import CachingMiddleware
from tinydb.storages import JSONStorage
from tqdm import tqdm

import util

last_access = None


def download_snapshot():
    global last_access
    if last_access is not None:
        time.sleep(max(0, last_access + util.SLEEP - time.time()))

    headers = {
        'Expect': '',
        'X-API-Key': util.API_KEY,
    }
    url = 'http://2016sv.icfpcontest.org/api/snapshot/list'
    r = requests.get(url, headers=headers)
    last_access = time.time()
    print('download_snapshot:', time.asctime(time.localtime(last_access)))
    return r.json()


def download_blob(spec_hash):
    global last_access
    if last_access is not None:
        time.sleep(max(0, last_access + util.SLEEP - time.time()))

    headers = {
        'Expect': '',
        'X-API-Key': util.API_KEY,
    }
    url = 'http://2016sv.icfpcontest.org/api/blob/' + spec_hash
    r = requests.get(url, headers=headers)
    last_access = time.time()
    print('download_blob:', time.asctime(time.localtime(last_access)))
    return r.text


def save_contest(contest_status):
    db = tinydb.TinyDB(util.FILENAME)
    contest_table = db.table('contest')
    contest_table.insert(contest_status)


def download_problems(contest_status):
    with tinydb.TinyDB(util.FILENAME, storage=CachingMiddleware(JSONStorage)) as db:
        problem_table = db.table('problem')

        for problem in tqdm(contest_status['problems']):
            if problem['problem_size'] > 500:
                continue

            problem_id = problem['problem_id']

            if not problem_table.contains(where('problem_id') == problem_id):
                print('problem_id =', problem_id)
                problem_spec_hash = problem['problem_spec_hash']
                content = download_blob(problem_spec_hash)
                problem_table.insert({'problem_id': problem_id, 'content': content})


def main():
    # snapshot APIからダウンロード
    snapshot = download_snapshot()
    if not snapshot['ok']:
        raise Exception(snapshot)
    # タイムスタンプが最新のハッシュ値を得る
    time_and_hash = [(x['snapshot_time'], x['snapshot_hash']) for x in snapshot['snapshots']]
    time_and_hash.sort(key=lambda x: x[0], reverse=True)
    latest_snapshot_hash = time_and_hash[0][1]
    print('latest_snapshot_hash =', latest_snapshot_hash)

    # ハッシュ値をもとにコンテスト情報をダウンロード
    contest_status = json.loads(download_blob(latest_snapshot_hash))
    save_contest(contest_status)
    download_problems(contest_status)

if __name__ == '__main__':
    main()
