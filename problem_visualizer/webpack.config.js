module.exports = {
    entry: {
        app: __dirname + '/app/browser/app.tsx'
    },
    output: {
        path: __dirname + '/dist',
        filename: "[name].bundle.js"
    },
    resolve: {
        extensions: ['', '.js', '.ts', 'tsx']
    },
    // https://webpack.github.io/docs/library-and-externals.html
    externals: {
        "d3": "d3",
        "react": "React",
        "react-dom": "ReactDOM",
    },
    module: {
        loaders: [
            { test: /\.ts(x?)$/, loader: 'ts-loader' },
        ]
    }
};
