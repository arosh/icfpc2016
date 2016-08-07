import {IProblemLineProps} from "./result";

/// <reference path="./require.d.ts" />

interface IJsonType {
    contest: {
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
        problem_id: number;
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
        this.tinydb = require<IJsonType>("!json!../../../submission_tool/icfpc.json");
    }
    public getTable() {
        const scoreDict: { [solverName: string]: { [problemId: string]: number } } = {};
        for (const index in this.tinydb.submission) {
            if (this.tinydb.submission.hasOwnProperty(index)) {
                const item = this.tinydb.submission[index];
                scoreDict[item.solver_name] = scoreDict[item.solver_name] || {};
                scoreDict[item.solver_name][item.problem_id] = item.resemblance;
            }
        }
        const problemIdToProperty = this.getProblemIdToProperty();
        const retval: IProblemLineProps[] = [];
        for (const index in this.tinydb.problem) {
            if (this.tinydb.problem.hasOwnProperty(index)) {
                const item = this.tinydb.problem[index];
                const property = problemIdToProperty[item.problem_id];
                retval.push({
                    Id: item.problem_id,
                    problemSize: property.problemSize,
                    solutionSize: property.solutionSize,
                    maxScore: property.maxScore,
                    yazaten_v3: scoreDict["yazaten_v3"][item.problem_id],
                });
            }
        }
        retval.sort((x, y) => x.Id - y.Id);
        return retval;
    }
    public getIdToContent() {
        const retval: { [index: string]: string } = {};
        for (const index in this.tinydb.problem) {
            if (this.tinydb.problem.hasOwnProperty(index)) {
                const item = this.tinydb.problem[index];
                retval[item.problem_id] = item.content;
            }
        }
        return retval;
    }
    private getProblemIdToProperty() {
        let latestContest: any;
        for (const index in this.tinydb.contest) {
            if (this.tinydb.contest.hasOwnProperty(index)) {
                latestContest = this.tinydb.contest[index];
            }
        }
        const problemIdToProperty: {
            [problemId: number]: {
                maxScore: number,
                problemSize: number,
                solutionSize: number
            }
        } = {};
        for (const problem of latestContest.problems) {
            let maxScore = NaN;
            if (problem.ranking.length > 0) {
                maxScore = problem.ranking[0].resemblance;
            }
            problemIdToProperty[problem.problem_id] = {
                maxScore: maxScore,
                problemSize: problem.problem_size,
                solutionSize: problem.solution_size,
            };
        }
        return problemIdToProperty;
    }
}
