const path = require("path");
const HtmlBundlerPlugin = require("html-bundler-webpack-plugin");

module.exports = {
  resolve: {
    alias: {
      "@fonts": path.join(__dirname, "web/styles/fonts"),
    },
  },
  plugins: [
    new HtmlBundlerPlugin({
      entry: {
        index: {
          import: "web/index.html",
        },
      },
      js: {
        filename: "js/[name].[contenthash:8].js",
      },
      css: {
        filename: "css/[name].[contenthash:8].css",
      },
      preload: [
        {
          test: /\.(ttf)$/,
          attributes: { as: "font", crossorigin: true },
        },
      ],
      minify: "auto",
      loaderOptions: {
        sources: [
          {
            tag: "script",
            filter: ({ attributes }) => attributes?.src !== "octachip.js",
          },
        ],
      },
    }),
  ],
  output: {
    path: path.resolve(__dirname, "build/dist"),
  },
  module: {
    rules: [
      {
        test: /\.(?:js|mjs|cjs)$/,
        exclude: /node_modules/,
        use: {
          loader: "babel-loader",
          options: {
            presets: [["@babel/preset-env", { targets: "defaults" }]],
          },
        },
      },
      {
        test: /\.(css|sass|scss)$/,
        use: ["css-loader", "sass-loader"],
      },
      {
        test: /\.(ttf)$/,
        type: "asset/resource",
        generator: {
          filename: "fonts/[name][ext]",
        },
      },
    ],
  },
};
