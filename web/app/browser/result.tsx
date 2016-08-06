import * as React from "react";
import { Route, IndexRoute, Link, Router, useRouterHistory } from "react-router";
import { createHashHistory } from "history";
import * as ReactDOM from "react-dom";
import { ResultStore } from "./result-store";

const resultStore = new ResultStore();

class App extends React.Component<{}, {}> {
    public render() {
        return (
            <div className="container">
                <div className="row">
                    <h1 className="text-center"><Link to="/">試運転 @ICFPC 2016</Link></h1>
                </div>
                { this.props.children }
            </div>
        );
    }
}

export interface IProblemLineProps {
    Id: number;
    problemSize: number;
    solutionSize: number;
    yazaten_v1: number;
    yazaten_v2: number;
    yazaten_v3: number;
}

class ProblemLine extends React.Component<IProblemLineProps, {}> {
    public render() {
        const detailUrl = `http://2016sv.icfpcontest.org/problem/view/${this.props.Id}`;
        const visualizeTo = `/problem/${this.props.Id}`;
        return (
            <tr key={this.props.Id}>
                <td><a href={detailUrl}>Detail</a></td>
                <td><Link to={visualizeTo}>Visualizer</Link></td>
                <td>{this.props.Id}</td>
                <td>{this.props.problemSize}</td>
                <td>{this.props.solutionSize}</td>
                <td>{this.props.yazaten_v1}</td>
                <td>{this.props.yazaten_v2}</td>
                <td>{this.props.yazaten_v3}</td>
            </tr>
        );
    }
}

interface IHomeState {
    problemLineProps: IProblemLineProps[];
}

class Home extends React.Component<{}, IHomeState> {
    constructor(props: {}) {
        super(props);
        this.state = {
            problemLineProps: resultStore.getTable(),
        };
    }
    public render() {
        const td = this.state.problemLineProps.map(x => <ProblemLine {...x} />);
        return (
            <div className="row">
                <table className="table table-striped">
                    <thead>
                        <th></th>
                        <th></th>
                        <th>Id</th>
                        <th>Problem Size</th>
                        <th>Solution Size</th>
                        <th>yazaten_v1</th>
                        <th>yazaten_v2</th>
                        <th>yazaten_v3</th>
                    </thead>
                    <tbody>
                        {td}
                    </tbody>
                </table>
            </div>
        );
    }
}

interface IItemProps {
    params: {
        problemId: string;
    }
}

class Item extends React.Component<IItemProps, {}> {
    public render() {
        return (
            <div>
                {this.props.params.problemId}
            </div>
        );
    }
}

const routes = (
    <Route path="/" component={App}>
        <IndexRoute component={Home} />
        <Route path="/problem/:problemId" component={Item} />
    </Route>
);

// https://github.com/reactjs/react-router/blob/master/upgrade-guides/v2.0.0.md#using-custom-histories
const appHistory = useRouterHistory(createHashHistory)({ queryKey: false });
ReactDOM.render(
    <Router routes={routes} history={appHistory}/>,
    document.getElementById("app")
);
