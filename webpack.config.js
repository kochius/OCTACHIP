const path = require("path");
const HtmlBundlerPlugin = require("html-bundler-webpack-plugin");

module.exports = {
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
      minify: "auto",
      loaderOptions: {
        sources: [
          {
            tag: "script",
            filter: ({ attributes }) => {
              const emscriptenFileName = "octachip.js";
              if (
                "src" in attributes &&
                attributes["src"] === emscriptenFileName
              ) {
                return false;
              }
              return true;
            },
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
        test: /\.(css|sass|scss)$/,
        use: ["css-loader", "sass-loader"],
      },
    ],
  },
};
