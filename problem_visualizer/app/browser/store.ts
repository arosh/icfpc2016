export interface IPoint {
    x: number;
    y: number;
}

export interface ISegment {
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

export class Store {
    private onChange: () => void;
    public inputText: string;
    public err: string;
    public vertex: IPoint[][];
    public edge: ISegment[];
    constructor(onChange: () => void) {
        this.onChange = onChange;
    }
    public getErrorMessage() {
        return this.err;
    }
    public onTextUpdate(text: string) {
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
        this.onChange();
    }
    private checkTokenError(tokenizer: StringTokenizer) {
        if (tokenizer.hasNext() === false) {
            this.err = tokenizer.index + "th token";
            this.onChange();
            return false;
        }
        return true;
    }
}
