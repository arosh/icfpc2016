import { IPoint, ISegment } from "./store";

const renderSize = 555;
const margin = 50;
const edgeRadius = 5;

class Color {
    public static BLACK = "#212121";
    public static RED = "#F44336";
}

export class D3Renderer {
    private el: d3.Selection<any>;
    private xScale: d3.scale.Linear<number, number>;
    private yScale: d3.scale.Linear<number, number>;
    private silhouetteVertexEl: d3.Selection<any>;
    private silhouetteEdgeEl: d3.Selection<any>;
    private skeletonEdgeEl: d3.Selection<any>;
    private xAxisEl: d3.Selection<any>;
    private yAxisEl: d3.Selection<any>;

    constructor(elementId: string) {
        const div = d3.select(elementId);
        div.select("svg").remove();
        this.el = div.append("svg")
            .attr({ width: renderSize, height: renderSize })
            .classed("center-block", true);
        // 順番重要
        this.skeletonEdgeEl = this.el.append("g");
        this.silhouetteVertexEl = this.el.append("g");
        this.silhouetteEdgeEl = this.el.append("g");
        this.xAxisEl = this.el.append("g");
        this.yAxisEl = this.el.append("g");
    }

    public render(silhouette: IPoint[][], skeleton: ISegment[]) {
        this.computeScaler(silhouette);
        this.renderSilhouetteVertex(silhouette);
        this.renderSilhouetteEdge(silhouette);
        this.renderSkeletonEdge(skeleton);
        this.xAxisEl.call(d3.svg.axis().scale(this.xScale).orient("bottom"));
        this.yAxisEl.call(d3.svg.axis().scale(this.yScale).orient("right"));
    }
    private renderSilhouetteVertex(silhouette: IPoint[][]) {
        const silhouetteVertex: IPoint[] = [];
        for (const poly of silhouette) {
            silhouetteVertex.push(...poly);
        }
        const silhouetteVertexCircle = this.silhouetteVertexEl.selectAll("circle").data(silhouetteVertex);
        silhouetteVertexCircle.enter().append("circle");
        silhouetteVertexCircle.exit().remove();
        silhouetteVertexCircle.attr({
            cx: (d, i) => this.xScale(d.x),
            cy: (d, i) => this.yScale(d.y),
            fill: Color.BLACK,
            r: edgeRadius,
        });
    }
    private renderSilhouetteEdge(silhouette: IPoint[][]) {
        const silhouetteEdge: ISegment[] = [];
        for (const poly of silhouette) {
            for (let i = 0; i < poly.length; ++i) {
                const j = (i + 1) % poly.length;
                silhouetteEdge.push({ st: poly[i], en: poly[j] });
            }
        }

        const silhouetteEdgeLine = this.silhouetteEdgeEl.selectAll("line").data(silhouetteEdge);
        silhouetteEdgeLine.enter().append("line");
        silhouetteEdgeLine.exit().remove();
        silhouetteEdgeLine.attr({
            stroke: Color.BLACK,
            "stroke-width": 2,
            x1: (d, i) => this.xScale(d.st.x),
            y1: (d, i) => this.yScale(d.st.y),
            x2: (d, i) => this.xScale(d.en.x),
            y2: (d, i) => this.yScale(d.en.y),
        });
    }
    private renderSkeletonEdge(skeleton: ISegment[]) {
        const skeletonEdgeLine = this.skeletonEdgeEl.selectAll("line").data(skeleton);
        skeletonEdgeLine.enter().append("line");
        skeletonEdgeLine.exit().remove();
        skeletonEdgeLine.attr({
            stroke: Color.RED,
            "stroke-width": 2,
            x1: (d, i) => this.xScale(d.st.x),
            y1: (d, i) => this.yScale(d.st.y),
            x2: (d, i) => this.xScale(d.en.x),
            y2: (d, i) => this.yScale(d.en.y),
        });
    }
    private computeScaler(vertex: IPoint[][]) {
        const xs: number[] = [];
        const ys: number[] = [];
        for (const poly of vertex) {
            for (const point of poly) {
                xs.push(point.x);
                ys.push(point.y);
            }
        }
        const xmin = Math.min(...xs);
        const xmax = Math.max(...xs);
        const ymin = Math.min(...ys);
        const ymax = Math.max(...ys);

        const xlen = xmax - xmin;
        const ylen = ymax - ymin;

        // yの上と下の調整をしないといけないなぁ…
        if (xlen > ylen) {
            this.xScale = d3.scale.linear()
                .domain([xmin, xmax])
                .range([0 + margin, renderSize - margin]);
            this.yScale = d3.scale.linear()
                .domain([(ymin + ymax) / 2 - xlen / 2, (ymin + ymax) / 2 + xlen / 2])
                .range([renderSize - margin, 0 + margin]);
        } else {
            this.xScale = d3.scale.linear()
                .domain([(xmin + xmax) / 2 - ylen / 2, (xmin + xmax) / 2 + ylen / 2])
                .range([0 + margin, renderSize - margin]);
            this.yScale = d3.scale.linear()
                .domain([ymin, ymax])
                .range([renderSize - margin, 0 + margin]);
        }
    }
}
