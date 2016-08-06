import * as React from "react";
import * as ReactDOM from "react-dom";
import {Store,IPoint,ISegment} from "./store";

const width = 970;
const height = 540;
const xScale = d3.scale.linear().domain([0, 1]).range([0, width]);
const yScale = d3.scale.linear().domain([0, 1]).range([0, height]);

const svgElement = d3.select("#d3")
    .append("svg")
    .attr({ width, height })
    .classed("center-block", true);

interface IAppState {
    inputText?: string;
    err?: string;
    vertex?: IPoint[][];
    edge?: ISegment[];
}

class App extends React.Component<{}, IAppState> {
    private store: Store;
    constructor(props: {}) {
        super(props);
        this.store = new Store(this.onChange.bind(this));
        this.state = {
            inputText: "",
            err: "",
            vertex: [],
            edge: [],
        };
    }
    public render() {
        const createTableRecord = (key: string, value: string) => {
            return (
                <tr key={key}>
                    <th scope="row">{key}</th>
                    <td>{value}</td>
                </tr>
            );
        };
        const polygonTables = this.state.vertex.map((arr, index) => {
            return createTableRecord("polygon[" + index + "]", arr.map(p => "(" + p.x + "," + p.y + ")").join(" "));
        });
        const edgeTables = this.state.edge.map((seg, index) => {
            const st = "(" + seg.st.x + "," + seg.st.y + ")";
            const en = "(" + seg.en.x + "," + seg.en.y + ")";
            return createTableRecord("edge[" + index + "]", st + " " + en);
        });

        return (
            <div className="row">
                <div className="col-xs-6">
                    <textarea
                        rows={10}
                        placeholder="入力を貼り付けてね"
                        value={ this.state.inputText }
                        onChange={ this.onTextUpdate.bind(this) }
                        className="form-control"/>
                </div>
                <div className="col-xs-6">
                    <p>Error: {this.state.err}</p>
                    <table className="table">
                        <tbody>
                            { polygonTables }
                            { edgeTables }
                        </tbody>
                    </table>
                </div>
            </div>
        );
    }
    private onChange() {
        this.setState({
            inputText: this.store.inputText,
            err: this.store.err,
            vertex: this.store.vertex,
            edge: this.store.edge,
        });
    }
    private onTextUpdate(e: any) {
        this.store.onTextUpdate(e.target.value);
    }
}

ReactDOM.render(<App />, document.getElementById("app"));
