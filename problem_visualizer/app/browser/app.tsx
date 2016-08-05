import { EventEmitter } from "events";

const width = 970;
const height = 540;
const xScale = d3.scale.linear().domain([0, 1]).range([0, width]);
const yScale = d3.scale.linear().domain([0, 1]).range([0, height]);

const svgElement = d3.select("#d3")
    .append("svg")
    .attr({ width, height })
    .classed("center-block", true);

interface IPoint {
    x: number;
    y: number;
}

interface ISegment {
    st: IPoint;
    en: IPoint;
}

class StringTokenizer {
    public tokens: string[];
    public index: number;
    constructor(text: string) {
        this.tokens = text.trim().split(/\s+/);
        this.index = 0;
    }
    public hasNext() {
        return this.index < this.tokens.length;
    }
    public next() {
        return this.tokens[this.index++];
    }
    public nextNumber() {
        return +this.next();
    }
    public nextPoint(): IPoint {
        const comma = this.next().split(",");
        const x = this.parseRational(comma[0]);
        const y = this.parseRational(comma[1]);
        return { x, y };
    }
    private parseRational(text: string) {
        const slash = text.split("/");

        if (slash.length === 1) {
            return +slash[0];
        } else if (slash.length === 2) {
            return +slash[0] / +slash[1];
        } else {
            return NaN;
        }
    }
}

class Store extends EventEmitter {
    public inputText: string;
    public err: string;
    public vertex: IPoint[][];
    public edge: ISegment[];
    constructor() {
        super();
        this.on("TEXT_UPDATE", this.onTextUpdate.bind(this));
    }
    public getErrorMessage() {
        return this.err;
    }
    private checkTokenError(tokenizer: StringTokenizer) {
        if (tokenizer.hasNext() === false) {
            this.err = tokenizer.index + "th token";
            this.emit("CHANGE");
            return false;
        }
        return true;
    }
    private onTextUpdate(text: string) {
        this.inputText = text;
        const tokenizer = new StringTokenizer(text);

        if (this.checkTokenError(tokenizer) === false) {
            return;
        }

        const numPolygon = tokenizer.nextNumber();
        const numVertex: number[] = [];
        this.vertex = [];

        for (let i = 0; i < numPolygon; ++i) {
            if (this.checkTokenError(tokenizer) === false) {
                return;
            }
            const numV = +tokenizer.next();
            numVertex.push(numV);
            const V: IPoint[] = [];
            for (let k = 0; k < numV; ++k) {
                if (this.checkTokenError(tokenizer) === false) {
                    return;
                }
                V.push(tokenizer.nextPoint());
            }
            this.vertex.push(V);
        }

        const numEdge = tokenizer.nextNumber();
        this.edge = [];
        for (let i = 0; i < numEdge; ++i) {
            if (this.checkTokenError(tokenizer) === false) {
                return;
            }
            const st = tokenizer.nextPoint();
            const en = tokenizer.nextPoint();
            this.edge.push({ st, en });
        }
        this.emit("CHANGE");
    }
}

interface IAppState {
    inputText?: string;
    err?: string;
    vertex?: IPoint[][];
    edge?: ISegment[];
}

const store = new Store();

class App extends React.Component<{}, IAppState> {
    constructor(props: {}) {
        super(props);
        this.state = {
            inputText: "",
            err: "",
            vertex: [],
            edge: [],
        };
        store.on("CHANGE", this.onChange.bind(this));
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
            inputText: store.inputText,
            err: store.err,
            vertex: store.vertex,
            edge: store.edge,
        });
    }
    private onTextUpdate(e: any) {
        store.emit("TEXT_UPDATE", e.target.value);
    }
}

ReactDOM.render(<App />, document.getElementById("app"));
