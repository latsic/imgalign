
// const CopyWebpackPlugin = require('copy-webpack-plugin')

module.exports = {
  publicPath: process.env.NODE_ENV === 'production'
    ? '/imgalign/'
    : '/'
  // configureWebpack: {
    
    // module: {
    //   rules: [
    //     {
    //       test: /\.worker\.js$/,
    //       use: {
    //         loader: 'worker-loader',
    //         options: {
    //           name: 'ImgAlignWorker.[hash].js'
    //         }
    //       }
    //     }
    //   ]
    // }
  // }
}