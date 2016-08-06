import {IProblemLineProps} from "./result";

/// <reference path="./require.d.ts" />

interface IJsonType {
    contests: {
        [index: string]: {
            users: {
                display_name: string;
                username: string;
            }[];
            problems: {
                ranking: {
                    resemblance: number;
                    solution_size: number;
                }[];
                publish_time: number;
                solution_size: number;
                problem_id: number;
                owner: string;
                problem_size: number;
                problem_spec_hash: string;
            }[];
            snapshot_time: number;
            leaderboard: {
                username: string;
                score: number;
            }[];
        };
    };
    problem: {
        ranking: {
            resemblance: number;
            solution_size: number;
        }[];
        publish_time: number;
        solution_size: number;
        problem_id: number;
        owner: string;
        problem_size: number;
        problem_spec_hash: string;
        content: string;
    }[];
    submission: {
        [index: string]: {
            ok: boolean;
            problem_id: number;
            resemblance: number;
            solution_spec_hash: string;
            solution_size: number;
            solver_name: string;
        }
    };
}

export class ResultStore {
    private tinydb: IJsonType;
    constructor() {
        this.tinydb = <IJsonType>require("!json!../../../submission_tool/icfpc.json");
    }
    public getTable() {
        const scoreDict: { [solver_name: string]: { [problem_id: string]: number } } = {};
        for (const index in this.tinydb.submission) {
            if (this.tinydb.submission.hasOwnProperty(index)) {
                const item = this.tinydb.submission[index];
                scoreDict[item.solver_name] = scoreDict[item.solver_name] || {};
                scoreDict[item.solver_name][item.problem_id] = item.resemblance;
            }
        }
        const retval: IProblemLineProps[] = [];
        for (const index in this.tinydb.problem) {
            if (this.tinydb.problem.hasOwnProperty(index)) {
                const item = this.tinydb.problem[index];
                retval.push({
                    Id: item.problem_id,
                    problemSize: item.problem_size,
                    solutionSize: item.solution_size,
                    yazaten_v1: scoreDict["yazaten_v1"][item.problem_id],
                    yazaten_v2: scoreDict["yazaten_v2"][item.problem_id],
                    yazaten_v3: scoreDict["yazaten_v3"][item.problem_id],
                });
            }
        }
        return retval;
    }
}