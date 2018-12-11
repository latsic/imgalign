
// const CopyWebpackPlugin = require('copy-webpack-plugin')

module.exports = {
  
  configureWebpack: {
    baseUrl: process.env.NODE_ENV === 'production'
      ? '/imgalign/'
      : '/'
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
  }
}