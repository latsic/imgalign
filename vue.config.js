
// const CopyWebpackPlugin = require('copy-webpack-plugin')

module.exports = {
  
  configureWebpack: {
    // plugins: [
    //   new CopyWebpackPlugin([{
    //     from: './src/opencv/opencv.js',
    //     to: '.'
    //   }])
    // ],
    // module: {
    //   rules: [
    //     {
    //       test: /\.worker\.js$/,
    //       use: { loader: 'worker-loader' }
    //     }
    //   ]
    // }
    // optimization: {
    //   splitChunks: {
    //     chunks: 'all'
    //   }
    // }
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