import * as React from "react";
import * as ReactDOM from "react-dom";
import { Store, IPoint, ISegment } from "./store";
import { D3Renderer } from "./d3renderer";

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
            <div className="container">
                <div className="row">
                    <div className="col-xs-6">
                        <form>
                            <textarea
                                rows={10}
                                placeholder="入力を貼り付けてね"
                                value={ this.state.inputText }
                                onChange={ this.onTextUpdate.bind(this) }
                                className="form-control"/>
                        </form>
                    </div>
                    <div id="d3" className="col-xs-6">
                    </div>
                </div>
                <div className="row">
                    <table className="table table-condensed">
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
        new D3Renderer("#d3").render(this.store.vertex, this.store.edge);
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
