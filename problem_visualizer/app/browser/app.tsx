import {EventEmitter} from "events";

const width = 970;
const height = 540;
const xScale = d3.scale.linear().domain([0, 1]).range([0, width]);
const yScale = d3.scale.linear().domain([0, 1]).range([0, height]);

const svgElement = d3.select("#d3")
    .append("svg")
    .attr({ width, height })
    .classed("center-block", true);

class Store extends EventEmitter {
    private inputText: string;
    private inputTextValid: boolean;
    constructor() {
        super();
        this.on("TEXT_UPDATE", this.onTextUpdate.bind(this));
    }
    public getInputText() {
        return this.inputText;
    }
    public isInputTextValid() {
        return this.inputTextValid;
    }
    private onTextUpdate(text: string) {
        this.inputText = text;
        this.emit("CHANGE");
    }
}

interface IAppState {
    textarea: string;
}

const store = new Store();

class App extends React.Component<{}, IAppState> {
    constructor(props: {}) {
        super(props);
        this.state = { textarea: "" };
        store.on("CHANGE", this.onChange.bind(this));
    }
    public render() {
        return (
            <div className="row">
                <div className="col-xs-6">
                    <textarea
                        value={this.state.textarea}
                        onChange={this.onTextUpdate.bind(this) }
                        className="form-control"/>
                </div>
            </div>
        );
    }
    private onChange() {
        this.setState({ textarea: store.getInputText() });
    }
    private onTextUpdate(e: any) {
        store.emit("TEXT_UPDATE", e.target.value);
    }
}

ReactDOM.render(<App />, document.getElementById("app"));
