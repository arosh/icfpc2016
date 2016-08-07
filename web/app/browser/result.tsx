import * as React from "react";
import { Route, IndexRoute, Link, Router, useRouterHistory } from "react-router";
import { createHashHistory } from "history";
import * as ReactDOM from "react-dom";
import { ResultStore } from "./result-store";
import { Store, IPoint, ISegment } from "./store";
import { D3Renderer } from "./d3renderer";

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
    maxScore: number;
    yazaten_v3: number;
}

class ProblemLine extends React.Component<IProblemLineProps, {}> {
    public render() {
        const detailUrl = `http://2016sv.icfpcontest.org/problem/view/${this.props.Id}`;
        const visualizeTo = `/problem/${this.props.Id}`;
        const maxScore = isNaN(this.props.maxScore) ? "NaN" : (this.props.maxScore + "");
        return (
            <tr key={this.props.Id}>
                <td><a href={detailUrl}>Detail</a></td>
                <td><Link to={visualizeTo}>Visualizer</Link></td>
                <td>{this.props.Id}</td>
                <td>{this.props.problemSize}</td>
                <td>{this.props.solutionSize}</td>
                <td>{maxScore}</td>
                <td>{this.props.yazaten_v3}</td>
            </tr>
        );
    }
}

class Home extends React.Component<{}, {}> {
    private problemLineProps: IProblemLineProps[];
    constructor(props: {}) {
        super(props);
        this.problemLineProps = resultStore.getTable();
    }
    public render() {
        const td = this.problemLineProps.map(x => <ProblemLine {...x} />);
        return (
            <div className="row">
                <table className="table table-hover">
                    <thead>
                        <th></th>
                        <th></th>
                        <th>Id</th>
                        <th>Problem Size</th>
                        <th>Solution Size</th>
                        <th>Max Score</th>
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
    };
}

interface IItemState {
    inputText: string;
    vertex: IPoint[][];
    edge: ISegment[];
}

class Item extends React.Component<IItemProps, IItemState> {
    private store: Store;
    private d3renderer: D3Renderer;
    private problemIdToContent: { [index: string]: string };
    constructor(props: IItemProps) {
        super(props);
        this.store = new Store(this.onChange.bind(this));
        this.problemIdToContent = resultStore.getIdToContent();
        this.state = {
            inputText: "",
            vertex: [],
            edge: [],
        };
    }
    public render() {
        return (
            <div className="container">
                <div className="row">
                    <div className="col-xs-6">
                        <form>
                            <textarea
                                rows={10}
                                value={this.state.inputText}
                                className="form-control"
                                readOnly={true} />
                        </form>
                    </div>
                    <div id="d3" className="col-xs-6"></div>
                </div>
            </div>
        );
    }
    public componentDidMount() {
        this.d3renderer = new D3Renderer("#d3");
        this.store.onTextUpdate(this.problemIdToContent[this.props.params.problemId].trim());
    }
    private onChange() {
        this.d3renderer.render(this.store.vertex, this.store.edge);
        this.setState({
            inputText: this.store.inputText,
            vertex: this.store.vertex,
            edge: this.store.edge,
        });
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
